
#include <elf.h>
#include <string.h>

#include <frg/manual_box.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/rtdl-sysdeps.hpp>
#include <mlibc/rtdl-abi.hpp>
#include <mlibc/thread.hpp>
#include <abi-bits/fcntl.h>
#include <internal-config.h>
#include "linker.hpp"

uintptr_t libraryBase = 0x41000000;

constexpr bool verbose = false;
constexpr bool stillSlightlyVerbose = false;
constexpr bool logBaseAddresses = false;
constexpr bool logRpath = false;
constexpr bool eagerBinding = true;

#if defined(__x86_64__)
constexpr inline bool tlsAboveTp = false;
#elif defined(__aarch64__)
constexpr inline bool tlsAboveTp = true;
#elif defined(__riscv)
constexpr inline bool tlsAboveTp = true;
#else
#	error Unknown architecture
#endif

extern DebugInterface globalDebugInterface;
extern uintptr_t __stack_chk_guard;

#if MLIBC_STATIC_BUILD
extern "C" size_t __init_array_start[];
extern "C" size_t __init_array_end[];
extern "C" size_t __preinit_array_start[];
extern "C" size_t __preinit_array_end[];
#endif

size_t tlsMaxAlignment = 16;

// This is the global "resolution timestamp" (RTS) counter.
// It is incremented each time __dlapi_open() (i.e. dlopen()) is called.
// Each DSO stores its objectRts (i.e. RTS at the time the object was loaded).
// DSOs in the global scope also store a globalRts (i.e. RTS at the time the
// object became global). This mechanism is used to determine which
// part of the global scope is considered for symbol resolution.
uint64_t rtsCounter = 2;

void seekOrDie(int fd, int64_t offset) {
	off_t noff;
	if(mlibc::sys_seek(fd, offset, SEEK_SET, &noff))
		__ensure(!"sys_seek() failed");
}

void readExactlyOrDie(int fd, void *data, size_t length) {
	size_t offset = 0;
	while(offset < length) {
		ssize_t chunk;
		if(mlibc::sys_read(fd, reinterpret_cast<char *>(data) + offset,
				length - offset, &chunk))
			__ensure(!"sys_read() failed");
		__ensure(chunk > 0);
		offset += chunk;
	}
	__ensure(offset == length);
}

void closeOrDie(int fd) {
	if(mlibc::sys_close(fd))
		__ensure(!"sys_close() failed");
}

uintptr_t alignUp(uintptr_t address, size_t align) {
	return (address + align - 1) & ~(align - 1);
}

// --------------------------------------------------------
// ObjectRepository
// --------------------------------------------------------

ObjectRepository::ObjectRepository()
: loadedObjects{getAllocator()},
	_nameMap{frg::hash<frg::string_view>{}, getAllocator()} {}

SharedObject *ObjectRepository::injectObjectFromDts(frg::string_view name,
		frg::string<MemoryAllocator> path, uintptr_t base_address,
		Elf64_Dyn *dynamic, uint64_t rts) {
	__ensure(!findLoadedObject(name));

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), false, nullptr, rts);
	object->baseAddress = base_address;
	object->dynamic = dynamic;
	_parseDynamic(object);

	_addLoadedObject(object);
	_discoverDependencies(object, nullptr, rts);

	return object;
}

SharedObject *ObjectRepository::injectObjectFromPhdrs(frg::string_view name,
		frg::string<MemoryAllocator> path, void *phdr_pointer,
		size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
		uint64_t rts) {
	__ensure(!findLoadedObject(name));

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), true, nullptr, rts);
	_fetchFromPhdrs(object, phdr_pointer, phdr_entry_size, num_phdrs, entry_pointer);
	_parseDynamic(object);

	_addLoadedObject(object);
	_discoverDependencies(object, nullptr, rts);

	return object;
}

SharedObject *ObjectRepository::injectStaticObject(frg::string_view name,
		frg::string<MemoryAllocator> path, void *phdr_pointer,
		size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
		uint64_t rts) {
	__ensure(!findLoadedObject(name));
	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), true, nullptr, rts);
	_fetchFromPhdrs(object, phdr_pointer, phdr_entry_size, num_phdrs, entry_pointer);

#if MLIBC_STATIC_BUILD
	object->initArray = reinterpret_cast<InitFuncPtr*>(__init_array_start);
	object->initArraySize = static_cast<size_t>((uintptr_t)__init_array_end -
			(uintptr_t)__init_array_start);
	object->preInitArray = reinterpret_cast<InitFuncPtr*>(__preinit_array_start);
	object->preInitArraySize = static_cast<size_t>((uintptr_t)__preinit_array_end -
			(uintptr_t)__preinit_array_start);
#endif

	_addLoadedObject(object);

	return object;
}

SharedObject *ObjectRepository::requestObjectWithName(frg::string_view name,
		SharedObject *origin, Scope *localScope, bool createScope, uint64_t rts) {
	if (auto obj = findLoadedObject(name))
		return obj;

	const char *libdirs[4] = {
		"/lib/",
		"/lib64/",
		"/usr/lib/",
		"/usr/lib64/"
	};

	auto tryToOpen = [&] (const char *path) {
		int fd;
		if(auto x = mlibc::sys_open(path, O_RDONLY, 0, &fd); x) {
			return -1;
		}
		return fd;
	};

	// TODO(arsen): this process can probably undergo heavy optimization, by
	// preprocessing the rpath only once on parse
	auto processRpath = [&] (frg::string_view path) {
		frg::string<MemoryAllocator> sPath { getAllocator() };
		if (path.starts_with("$ORIGIN")) {
			frg::string_view dirname = origin->path;
			auto lastsl = dirname.find_last('/');
			if (lastsl != (uint64_t)-1) {
				dirname = dirname.sub_string(0, lastsl);
			} else {
				dirname = ".";
			}
			sPath = frg::string<MemoryAllocator>{ getAllocator(), dirname };
			sPath += path.sub_string(7, path.size() - 7);
		} else {
			sPath = frg::string<MemoryAllocator>{ getAllocator(), path };
		}
		if (sPath[sPath.size() - 1] != '/') {
			sPath += '/';
		}
		sPath += name;
		if (logRpath)
			mlibc::infoLogger() << "rtdl: trying in rpath " << sPath << frg::endlog;
		int fd = tryToOpen(sPath.data());
		if (logRpath && fd >= 0)
			mlibc::infoLogger() << "rtdl: found in rpath" << frg::endlog;
		return frg::tuple { fd, std::move(sPath) };
	};

	frg::string<MemoryAllocator> chosenPath { getAllocator() };
	int fd = -1;
	if (origin && origin->runPath) {
		size_t start = 0;
		size_t idx = 0;
		frg::string_view rpath { origin->runPath };
		auto next = [&] () {
			idx = rpath.find_first(':', start);
			if (idx == (size_t)-1)
				idx = rpath.size();
		};
		for (next(); idx < rpath.size(); next()) {
			auto path = rpath.sub_string(start, idx - start);
			start = idx + 1;
			auto [fd_, fullPath] = processRpath(path);
			if (fd_ != -1) {
				fd = fd_;
				chosenPath = std::move(fullPath);
				break;
			}
		}
		if (fd == -1) {
			auto path = rpath.sub_string(start, rpath.size() - start);
			auto [fd_, fullPath] = processRpath(path);
			if (fd_ != -1) {
				fd = fd_;
				chosenPath = std::move(fullPath);
			}
		}
	} else if (logRpath) {
		mlibc::infoLogger() << "rtdl: no rpath set for object" << frg::endlog;
	}
	for(int i = 0; i < 4 && fd == -1; i++) {
		auto path = frg::string<MemoryAllocator>{getAllocator(), libdirs[i]} + name + '\0';
		fd = tryToOpen(path.data());
		if(fd >= 0) {
			chosenPath = std::move(path);
			break;
		}
	}
	if(fd == -1)
		return nullptr;

	if (createScope) {
		__ensure(localScope == nullptr);

		// TODO: Free this when the scope is no longer needed.
		localScope = frg::construct<Scope>(getAllocator());
	}

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(chosenPath), false, localScope, rts);

	_fetchFromFile(object, fd);
	closeOrDie(fd);

	_parseDynamic(object);

	_addLoadedObject(object);
	_discoverDependencies(object, localScope, rts);

	return object;
}

SharedObject *ObjectRepository::requestObjectAtPath(frg::string_view path,
		Scope *localScope, bool createScope, uint64_t rts) {
	// TODO: Support SONAME correctly.
	auto lastSlash = path.find_last('/') + 1;
	auto name = path;
	if (!lastSlash) {
		name = name.sub_string(lastSlash, path.size() - lastSlash);
	}
	if (auto obj = findLoadedObject(name))
		return obj;

	if (createScope) {
		__ensure(localScope == nullptr);

		// TODO: Free this when the scope is no longer needed.
		localScope = frg::construct<Scope>(getAllocator());
	}

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), path.data(), false, localScope, rts);

	frg::string<MemoryAllocator> no_prefix(getAllocator(), path);

	int fd;
	if(mlibc::sys_open((no_prefix + '\0').data(), O_RDONLY, 0, &fd))
		return nullptr; // TODO: Free the SharedObject.
	_fetchFromFile(object, fd);
	closeOrDie(fd);

	_parseDynamic(object);

	_addLoadedObject(object);
	_discoverDependencies(object, localScope, rts);

	return object;
}

SharedObject *ObjectRepository::findCaller(void *addr) {
	uintptr_t target = reinterpret_cast<uintptr_t>(addr);

	for (auto [name, object] : _nameMap) {
		// Search all PT_LOAD segments for the specified address.
		for(size_t j = 0; j < object->phdrCount; j++) {
			auto phdr = (Elf64_Phdr *)((uintptr_t)object->phdrPointer + j * object->phdrEntrySize);
			if (phdr->p_type == PT_LOAD) {
				uintptr_t start = object->baseAddress + phdr->p_vaddr;
				uintptr_t end = start + phdr->p_memsz;
				if (start <= target && target < end)
					return object;
			}
		}
	}

	return nullptr;
}

SharedObject *ObjectRepository::findLoadedObject(frg::string_view name) {
	auto it = _nameMap.get(name);
	if (it)
		return *it;

	for (auto object : loadedObjects) {
		// See if any object has a matching SONAME.
		if (object->soName && name == object->soName)
			return object;	
	}

	// TODO: We should also look at the device and inode here as a fallback.
	return nullptr;
}

// --------------------------------------------------------
// ObjectRepository: Fetching methods.
// --------------------------------------------------------

void ObjectRepository::_fetchFromPhdrs(SharedObject *object, void *phdr_pointer,
		size_t phdr_entry_size, size_t phdr_count, void *entry_pointer) {
	__ensure(object->isMainObject);
	object->phdrPointer = phdr_pointer;
	object->phdrEntrySize = phdr_entry_size;
	object->phdrCount = phdr_count;
	if(verbose)
		mlibc::infoLogger() << "rtdl: Loading " << object->name << frg::endlog;

	// Note: the entry pointer is absolute and not relative to the base address.
	object->entry = entry_pointer;

	frg::optional<ptrdiff_t> dynamic_offset;
	frg::optional<ptrdiff_t> tls_offset;

	// segments are already mapped, so we just have to find the dynamic section
	for(size_t i = 0; i < phdr_count; i++) {
		auto phdr = (Elf64_Phdr *)((uintptr_t)phdr_pointer + i * phdr_entry_size);
		switch(phdr->p_type) {
		case PT_PHDR:
			// Determine the executable's base address (in the PIE case) by comparing
			// the PHDR segment's load address against it's address in the ELF file.
			object->baseAddress = reinterpret_cast<uintptr_t>(phdr_pointer) - phdr->p_vaddr;
			if(verbose)
				mlibc::infoLogger() << "rtdl: Executable is loaded at "
						<< (void *)object->baseAddress << frg::endlog;
			break;
		case PT_DYNAMIC:
			dynamic_offset = phdr->p_vaddr;
			break;
		case PT_TLS: {
			object->tlsSegmentSize = phdr->p_memsz;
			object->tlsAlignment = phdr->p_align;
			object->tlsImageSize = phdr->p_filesz;
			tls_offset = phdr->p_vaddr;
			break;
		case PT_INTERP:
			object->interpreterPath = frg::string<MemoryAllocator>{
				(char*)(object->baseAddress + phdr->p_vaddr),
					getAllocator()
			};
		} break;
		default:
			//FIXME warn about unknown phdrs
			break;
		}
	}

	if(dynamic_offset)
		object->dynamic = (Elf64_Dyn *)(object->baseAddress + *dynamic_offset);
	if(tls_offset)
		object->tlsImagePtr = (void *)(object->baseAddress + *tls_offset);
}


void ObjectRepository::_fetchFromFile(SharedObject *object, int fd) {
	__ensure(!object->isMainObject);

	// read the elf file header
	Elf64_Ehdr ehdr;
	readExactlyOrDie(fd, &ehdr, sizeof(Elf64_Ehdr));

	__ensure(ehdr.e_ident[0] == 0x7F
			&& ehdr.e_ident[1] == 'E'
			&& ehdr.e_ident[2] == 'L'
			&& ehdr.e_ident[3] == 'F');
	__ensure(ehdr.e_type == ET_EXEC || ehdr.e_type == ET_DYN);

	// read the elf program headers
	auto phdr_buffer = (char *)getAllocator().allocate(ehdr.e_phnum * ehdr.e_phentsize);
	seekOrDie(fd, ehdr.e_phoff);
	readExactlyOrDie(fd, phdr_buffer, ehdr.e_phnum * ehdr.e_phentsize);

	object->phdrPointer = phdr_buffer;
	object->phdrCount = ehdr.e_phnum;
	object->phdrEntrySize = ehdr.e_phentsize;

	// Allocate virtual address space for the DSO.
	constexpr size_t hugeSize = 0x200000;

	uintptr_t highest_address = 0;
	for(int i = 0; i < ehdr.e_phnum; i++) {
		auto phdr = (Elf64_Phdr *)(phdr_buffer + i * ehdr.e_phentsize);

		if(phdr->p_type != PT_LOAD)
			continue;

		auto limit = phdr->p_vaddr + phdr->p_memsz;
		if(limit > highest_address)
			highest_address = limit;
	}

	__ensure(!(object->baseAddress & (hugeSize - 1)));
	object->baseAddress = libraryBase;
	libraryBase += (highest_address + (hugeSize - 1)) & ~(hugeSize - 1);

	if(verbose || logBaseAddresses)
		mlibc::infoLogger() << "rtdl: Loading " << object->name
				<< " at " << (void *)object->baseAddress << frg::endlog;

	// Load all segments.
	constexpr size_t pageSize = 0x1000;
	for(int i = 0; i < ehdr.e_phnum; i++) {
		auto phdr = (Elf64_Phdr *)(phdr_buffer + i * ehdr.e_phentsize);

		if(phdr->p_type == PT_LOAD) {
			size_t misalign = phdr->p_vaddr & (pageSize - 1);
			__ensure(phdr->p_memsz > 0);
			__ensure(phdr->p_memsz >= phdr->p_filesz);

			// If the following condition is violated, we cannot use mmap() the segment;
			// however, GCC only generates ELF files that satisfy this.
			__ensure(misalign == (phdr->p_offset & (pageSize - 1)));

			auto map_address = object->baseAddress + phdr->p_vaddr - misalign;
			auto backed_map_size = (phdr->p_filesz + misalign + pageSize - 1) & ~(pageSize - 1);
			auto total_map_size = (phdr->p_memsz + misalign + pageSize - 1) & ~(pageSize - 1);

			int prot = 0;
			if(phdr->p_flags & PF_R)
				prot |= PROT_READ;
			if(phdr->p_flags & PF_W)
				prot |= PROT_WRITE;
			if(phdr->p_flags & PF_X)
				prot |= PROT_EXEC;

			#if MLIBC_MAP_DSO_SEGMENTS
				// TODO: Map with (prot | PROT_WRITE) here,
				// then mprotect() to remove PROT_WRITE if that is necessary.
				void *map_pointer;
				if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address),
						backed_map_size, prot,
						MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset - misalign, &map_pointer))
					__ensure(!"sys_vm_map failed");
				if(total_map_size > backed_map_size)
					if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address + backed_map_size),
							total_map_size - backed_map_size, prot,
							MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0, &map_pointer))
						__ensure(!"sys_vm_map failed");

				if(mlibc::sys_vm_readahead)
					if(mlibc::sys_vm_readahead(reinterpret_cast<void *>(map_address),
							backed_map_size))
						mlibc::infoLogger() << "mlibc: sys_vm_readahead() failed in ld.so"
								<< frg::endlog;

				// Clear the trailing area at the end of the backed mapping.
				// We do not clear the leading area; programs are not supposed to access it.
				memset(reinterpret_cast<void *>(map_address + misalign + phdr->p_filesz),
						0, phdr->p_memsz - phdr->p_filesz);
			#else
				(void)backed_map_size;

				void *map_pointer;
				if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address),
						total_map_size, prot | PROT_WRITE,
						MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0, &map_pointer))
					__ensure(!"sys_vm_map failed");

				seekOrDie(fd, phdr->p_offset);
				readExactlyOrDie(fd, reinterpret_cast<char *>(map_address) + misalign,
						phdr->p_filesz);
			#endif
		}else if(phdr->p_type == PT_TLS) {
			object->tlsSegmentSize = phdr->p_memsz;
			object->tlsAlignment = phdr->p_align;
			object->tlsImageSize = phdr->p_filesz;
			object->tlsImagePtr = (void *)(object->baseAddress + phdr->p_vaddr);
		}else if(phdr->p_type == PT_DYNAMIC) {
			object->dynamic = (Elf64_Dyn *)(object->baseAddress + phdr->p_vaddr);
		}else if(phdr->p_type == PT_INTERP
				|| phdr->p_type == PT_PHDR
				|| phdr->p_type == PT_NOTE
				|| phdr->p_type == PT_RISCV_ATTRIBUTES
				|| phdr->p_type == PT_GNU_EH_FRAME
				|| phdr->p_type == PT_GNU_RELRO
				|| phdr->p_type == PT_GNU_STACK
				|| phdr->p_type == PT_GNU_PROPERTY) {
			// ignore the phdr
		}else{
			mlibc::panicLogger() << "Unexpected PHDR type 0x"
					<< frg::hex_fmt(phdr->p_type) << " in DSO " << object->name << frg::endlog;
		}
	}
}

// --------------------------------------------------------
// ObjectRepository: Parsing methods.
// --------------------------------------------------------

void ObjectRepository::_parseDynamic(SharedObject *object) {
	if(!object->dynamic)
		mlibc::infoLogger() << "ldso: Object '" << object->name
				<< "' does not have a dynamic section" << frg::endlog;
	__ensure(object->dynamic);

	// Fix up these offsets to addresses after the loop, since the
	// addresses depend on the value of DT_STRTAB.
	frg::optional<ptrdiff_t> runpath_offset;
	frg::optional<ptrdiff_t> soname_offset;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];
		switch(dynamic->d_tag) {
		// handle hash table, symbol table and string table
		case DT_HASH:
			object->hashStyle = HashStyle::systemV;
			object->hashTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_GNU_HASH:
			object->hashStyle = HashStyle::gnu;
			object->hashTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_STRTAB:
			object->stringTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_STRSZ:
			break; // we don't need the size of the string table
		case DT_SYMTAB:
			object->symbolTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_SYMENT:
			__ensure(dynamic->d_un.d_val == sizeof(Elf64_Sym));
			break;
		// handle lazy relocation table
		case DT_PLTGOT:
			object->globalOffsetTable = (void **)(object->baseAddress
					+ dynamic->d_un.d_ptr);
			break;
		case DT_JMPREL:
			object->lazyRelocTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_PLTRELSZ:
			object->lazyTableSize = dynamic->d_un.d_val;
			break;
		case DT_PLTREL:
			if(dynamic->d_un.d_val == DT_RELA) {
				object->lazyExplicitAddend = true;
			}else{
				__ensure(dynamic->d_un.d_val == DT_REL);
			}
			break;
		// TODO: Implement this correctly!
		case DT_SYMBOLIC:
			object->symbolicResolution = true;
			break;
		case DT_BIND_NOW:
			object->eagerBinding = true;
			break;
		case DT_FLAGS: {
			if(dynamic->d_un.d_val & DF_SYMBOLIC)
				object->symbolicResolution = true;
			if(dynamic->d_un.d_val & DF_STATIC_TLS)
				object->haveStaticTls = true;
			if(dynamic->d_un.d_val & DF_BIND_NOW)
				object->eagerBinding = true;

			auto ignored = DF_BIND_NOW | DF_SYMBOLIC | DF_STATIC_TLS;
#ifdef __riscv
			// Work around https://sourceware.org/bugzilla/show_bug.cgi?id=24673.
			ignored |= DF_TEXTREL;
#endif
			if(dynamic->d_un.d_val & ~ignored)
				mlibc::infoLogger() << "\e[31mrtdl: DT_FLAGS(" << frg::hex_fmt{dynamic->d_un.d_val & ~ignored}
						<< ") is not implemented correctly!\e[39m"
						<< frg::endlog;
		} break;
		case DT_FLAGS_1:
			if(dynamic->d_un.d_val & DF_1_NOW)
				object->eagerBinding = true;
			// The DF_1_PIE flag is informational only. It is used by e.g file(1).
			// The DF_1_NODELETE flag has a similar effect to RTLD_NODELETE, both of which we
			// ignore because we don't implement dlclose().
			if(dynamic->d_un.d_val & ~(DF_1_NOW | DF_1_PIE | DF_1_NODELETE))
				mlibc::infoLogger() << "\e[31mrtdl: DT_FLAGS_1(" << frg::hex_fmt{dynamic->d_un.d_val}
						<< ") is not implemented correctly!\e[39m"
						<< frg::endlog;
			break;
		case DT_RPATH:
			mlibc::infoLogger() << "\e[31mrtdl: RUNPATH not preferred over RPATH properly\e[39m" << frg::endlog;
			[[fallthrough]];
		case DT_RUNPATH:
			runpath_offset = dynamic->d_un.d_val;
			break;
		case DT_INIT:
			if(dynamic->d_un.d_ptr != 0)
				object->initPtr = (InitFuncPtr)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_INIT_ARRAY:
			if(dynamic->d_un.d_ptr != 0)
				object->initArray = (InitFuncPtr *)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_INIT_ARRAYSZ:
			object->initArraySize = dynamic->d_un.d_val;
			break;
		case DT_PREINIT_ARRAY:
			if(dynamic->d_un.d_ptr != 0) {
				// Only the main object is allowed pre-initializers.
				__ensure(object->isMainObject);
				object->preInitArray = (InitFuncPtr *)(object->baseAddress + dynamic->d_un.d_ptr);
			}
			break;
		case DT_PREINIT_ARRAYSZ:
			// Only the main object is allowed pre-initializers.
			__ensure(object->isMainObject);
			object->preInitArraySize = dynamic->d_un.d_val;
			break;
		case DT_DEBUG:
			dynamic->d_un.d_val = reinterpret_cast<Elf64_Xword>(&globalDebugInterface);
			break;
		case DT_SONAME:
			soname_offset = dynamic->d_un.d_val;
			break;
		// ignore unimportant tags
		case DT_NEEDED: // we handle this later
		case DT_FINI: case DT_FINI_ARRAY: case DT_FINI_ARRAYSZ:
		case DT_RELA: case DT_RELASZ: case DT_RELAENT: case DT_RELACOUNT:
		case DT_VERSYM:
		case DT_VERDEF: case DT_VERDEFNUM:
		case DT_VERNEED: case DT_VERNEEDNUM:
#ifdef __riscv
		case DT_TEXTREL: // Work around https://sourceware.org/bugzilla/show_bug.cgi?id=24673.
#endif
			break;
		case DT_TLSDESC_PLT: case DT_TLSDESC_GOT:
			break;
		default:
			mlibc::panicLogger() << "Unexpected dynamic entry "
					<< (void *)dynamic->d_tag << " in object" << frg::endlog;
		}
	}

	if(runpath_offset) {
		object->runPath = reinterpret_cast<const char *>(object->baseAddress
				+ object->stringTableOffset + *runpath_offset);
	}
	if(soname_offset) {
		object->soName = reinterpret_cast<const char *>(object->baseAddress
				+ object->stringTableOffset + *soname_offset);
	}
}

void ObjectRepository::_discoverDependencies(SharedObject *object,
		Scope *localScope, uint64_t rts) {
	// Load required dynamic libraries.
	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];
		if(dynamic->d_tag != DT_NEEDED)
			continue;

		const char *library_str = (const char *)(object->baseAddress
				+ object->stringTableOffset + dynamic->d_un.d_val);

		auto library = requestObjectWithName(frg::string_view{library_str}, 
				object, localScope, false, rts);
		if(!library)
			mlibc::panicLogger() << "Could not satisfy dependency " << library_str << frg::endlog;
		object->dependencies.push(library);
	}
}

void ObjectRepository::_addLoadedObject(SharedObject *object) {
	_nameMap.insert(object->name, object);
	loadedObjects.push_back(object);
}

// --------------------------------------------------------
// SharedObject
// --------------------------------------------------------

SharedObject::SharedObject(const char *name, frg::string<MemoryAllocator> path,
	bool is_main_object, Scope *local_scope, uint64_t object_rts)
		: name(name, getAllocator()), path(std::move(path)),
		interpreterPath(getAllocator()), soName(nullptr),
		isMainObject(is_main_object), objectRts(object_rts), inLinkMap(false),
		baseAddress(0), localScope(local_scope), dynamic(nullptr),
		globalOffsetTable(nullptr), entry(nullptr), tlsSegmentSize(0),
		tlsAlignment(0), tlsImageSize(0), tlsImagePtr(nullptr),
		tlsInitialized(false), hashTableOffset(0), symbolTableOffset(0),
		stringTableOffset(0), lazyRelocTableOffset(0), lazyTableSize(0),
		lazyExplicitAddend(false), symbolicResolution(false),
		eagerBinding(false), haveStaticTls(false),
		dependencies(getAllocator()), tlsModel(TlsModel::null),
		tlsOffset(0), globalRts(0), wasLinked(false),
		scheduledForInit(false), onInitStack(false),
		wasInitialized(false) { }

SharedObject::SharedObject(const char *name, const char *path,
	bool is_main_object, Scope *localScope, uint64_t object_rts)
		: SharedObject(name,
			frg::string<MemoryAllocator> { path, getAllocator() },
			is_main_object, localScope, object_rts) {}

void processCopyRela(SharedObject *object, Elf64_Rela *reloc) {
	Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
	Elf64_Xword symbol_index = ELF64_R_SYM(reloc->r_info);
#if defined(__x86_64__)
	if(type != R_X86_64_COPY)
		return;
#elif defined(__aarch64__)
	if(type != R_AARCH64_COPY)
		return;
#elif defined(__riscv)
	if (type != R_RISCV_COPY)
		return;
#endif

	uintptr_t rel_addr = object->baseAddress + reloc->r_offset;

	auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
			+ symbol_index * sizeof(Elf64_Sym));
	ObjectSymbol r(object, symbol);
	frg::optional<ObjectSymbol> p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, r.getString(), object->objectRts, Scope::resolveCopy);
	__ensure(p);

	memcpy((void *)rel_addr, (void *)p->virtualAddress(), symbol->st_size);
}

void processCopyRelocations(SharedObject *object) {
	frg::optional<uintptr_t> rela_offset;
	frg::optional<size_t> rela_length;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];

		switch(dynamic->d_tag) {
		case DT_RELA:
			rela_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_un.d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_un.d_val == sizeof(Elf64_Rela));
			break;
		}
	}

	if(rela_offset && rela_length) {
		for(size_t offset = 0; offset < *rela_length; offset += sizeof(Elf64_Rela)) {
			auto reloc = (Elf64_Rela *)(object->baseAddress + *rela_offset + offset);
			processCopyRela(object, reloc);
		}
	}else{
		__ensure(!rela_offset && !rela_length);
	}
}

void doInitialize(SharedObject *object) {
	__ensure(object->wasLinked);
	__ensure(!object->wasInitialized);

	// if the object has dependencies we initialize them first
	for(size_t i = 0; i < object->dependencies.size(); i++)
		__ensure(object->dependencies[i]->wasInitialized);

	if(verbose)
		mlibc::infoLogger() << "rtdl: Initialize " << object->name << frg::endlog;

	if(verbose)
		mlibc::infoLogger() << "rtdl: Running DT_INIT function" << frg::endlog;
	if(object->initPtr != nullptr)
		object->initPtr();

	if(verbose)
		mlibc::infoLogger() << "rtdl: Running DT_INIT_ARRAY functions" << frg::endlog;
	__ensure((object->initArraySize % sizeof(InitFuncPtr)) == 0);
	for(size_t i = 0; i < object->initArraySize / sizeof(InitFuncPtr); i++)
		object->initArray[i]();

	if(verbose)
		mlibc::infoLogger() << "rtdl: Object initialization complete" << frg::endlog;
	object->wasInitialized = true;
}

// --------------------------------------------------------
// RuntimeTlsMap
// --------------------------------------------------------

RuntimeTlsMap::RuntimeTlsMap()
: initialPtr{0}, initialLimit{0}, indices{getAllocator()} { }

void initTlsObjects(Tcb *tcb, const frg::vector<SharedObject *, MemoryAllocator> &objects, bool checkInitialized) {
	// Initialize TLS segments that follow the static model.
	for(auto object : objects) {
		if(object->tlsModel == TlsModel::initial) {
			if(checkInitialized && object->tlsInitialized)
				continue;

			char *tcb_ptr = reinterpret_cast<char *>(tcb);
			auto tls_ptr = tcb_ptr + object->tlsOffset;
			memset(tls_ptr, 0, object->tlsSegmentSize);
			memcpy(tls_ptr, object->tlsImagePtr, object->tlsImageSize);

			if (verbose) {
				mlibc::infoLogger() << "rtdl: wrote tls image at " << (void *)tls_ptr
						<< ", size = 0x" << frg::hex_fmt{object->tlsSegmentSize} << frg::endlog;
			}

			if (checkInitialized)
				object->tlsInitialized = true;
		}
	}
}

Tcb *allocateTcb() {
	size_t tlsInitialSize = runtimeTlsMap->initialLimit;

	// To make sure that both the TCB and TLS data are sufficiently aligned, allocate
	// slightly more than necessary and adjust alignment afterwards.
	size_t alignOverhead = frg::min(alignof(Tcb), tlsMaxAlignment);
	size_t allocSize = tlsInitialSize + sizeof(Tcb) + alignOverhead;
	auto allocation = reinterpret_cast<uintptr_t>(getAllocator().allocate(allocSize));
	memset(reinterpret_cast<void *>(allocation), 0, allocSize);

	uintptr_t tlsAddress, tcbAddress;
	if constexpr (tlsAboveTp) {
		// Here we must satisfy two requirements of the TCB and the TLS data:
		//   1. One should follow the other immediately in memory. We do this so that
		//      we can simply add or subtract sizeof(Tcb) to obtain the address of the other.
		//   2. Both should be sufficiently aligned.
		// To do this, we will fix whichever address has stricter alignment requirements, and
		// derive the other from it.
		if (tlsMaxAlignment > alignof(Tcb)) {
			tlsAddress = alignUp(allocation + sizeof(Tcb), tlsMaxAlignment);
			tcbAddress = tlsAddress - sizeof(Tcb);
		} else {
			tcbAddress = alignUp(allocation, alignof(Tcb));
			tlsAddress = tcbAddress + sizeof(Tcb);
		}
		__ensure((tlsAddress & (tlsMaxAlignment - 1)) == 0);
		__ensure(tlsAddress == tcbAddress + sizeof(Tcb));
	} else {
		// The TCB should be aligned such that the preceding blocks are aligned too.
		tcbAddress = alignUp(allocation + tlsInitialSize, alignOverhead);
		tlsAddress = tcbAddress - tlsInitialSize;
	}
	__ensure((tcbAddress & (alignof(Tcb) - 1)) == 0);

	if (verbose) {
		mlibc::infoLogger() << "rtdl: tcb allocated at " << (void *)tcbAddress
				<< ", size = 0x" << frg::hex_fmt{sizeof(Tcb)} << frg::endlog;
		mlibc::infoLogger() << "rtdl: tls allocated at " << (void *)tlsAddress
				<< ", size = 0x" << frg::hex_fmt{tlsInitialSize} << frg::endlog;
	}

	Tcb *tcb_ptr = new ((char *)tcbAddress) Tcb;
	tcb_ptr->selfPointer = tcb_ptr;

	tcb_ptr->stackCanary = __stack_chk_guard;
	tcb_ptr->cancelBits = tcbCancelEnableBit;
	tcb_ptr->didExit = 0;
	tcb_ptr->isJoinable = 1;
	tcb_ptr->returnValue = nullptr;
	tcb_ptr->localKeys = frg::construct<frg::array<Tcb::LocalKey, PTHREAD_KEYS_MAX>>(getAllocator());
	tcb_ptr->dtvSize = runtimeTlsMap->indices.size();
	tcb_ptr->dtvPointers = frg::construct_n<void *>(getAllocator(), runtimeTlsMap->indices.size());
	memset(tcb_ptr->dtvPointers, 0, sizeof(void *) * runtimeTlsMap->indices.size());
	for(size_t i = 0; i < runtimeTlsMap->indices.size(); ++i) {
		auto object = runtimeTlsMap->indices[i];
		if(object->tlsModel != TlsModel::initial)
			continue;
		tcb_ptr->dtvPointers[i] = reinterpret_cast<char *>(tcb_ptr) + object->tlsOffset;
	}

	return tcb_ptr;
}

void *accessDtv(SharedObject *object) {
	Tcb *tcb_ptr = mlibc::get_current_tcb();

	// We might need to reallocate the DTV.
	if(object->tlsIndex >= tcb_ptr->dtvSize) {
		// TODO: need to protect runtimeTlsMap against concurrent access.
		auto ndtv = frg::construct_n<void *>(getAllocator(), runtimeTlsMap->indices.size());
		memset(ndtv, 0, sizeof(void *) * runtimeTlsMap->indices.size());
		memcpy(ndtv, tcb_ptr->dtvPointers, sizeof(void *) * tcb_ptr->dtvSize);
		frg::destruct_n(getAllocator(), tcb_ptr->dtvPointers, tcb_ptr->dtvSize);
		tcb_ptr->dtvSize = runtimeTlsMap->indices.size();
		tcb_ptr->dtvPointers = ndtv;
	}

	// We might need to fill in a new DTV entry.
	if(!tcb_ptr->dtvPointers[object->tlsIndex]) {
		__ensure(object->tlsModel == TlsModel::dynamic);

		auto buffer = getAllocator().allocate(object->tlsSegmentSize);
		__ensure(!(reinterpret_cast<uintptr_t>(buffer) & (object->tlsAlignment - 1)));
		memset(buffer, 0, object->tlsSegmentSize);
		memcpy(buffer, object->tlsImagePtr, object->tlsImageSize);
		tcb_ptr->dtvPointers[object->tlsIndex] = buffer;

		if (verbose) {
			mlibc::infoLogger() << "rtdl: accessDtv wrote tls image at " << buffer
					<< ", size = 0x" << frg::hex_fmt{object->tlsSegmentSize} << frg::endlog;
		}
	}

	return (void *)((char *)tcb_ptr->dtvPointers[object->tlsIndex] + TLS_DTV_OFFSET);
}

void *tryAccessDtv(SharedObject *object) {
	Tcb *tcb_ptr = mlibc::get_current_tcb();

	if (object->tlsIndex >= tcb_ptr->dtvSize)
		return nullptr;
	if (!tcb_ptr->dtvPointers[object->tlsIndex])
		return nullptr;

	return (void *)((char *)tcb_ptr->dtvPointers[object->tlsIndex] + TLS_DTV_OFFSET);
}

// --------------------------------------------------------
// ObjectSymbol
// --------------------------------------------------------

ObjectSymbol::ObjectSymbol(SharedObject *object, const Elf64_Sym *symbol)
: _object(object), _symbol(symbol) { }

const char *ObjectSymbol::getString() {
	__ensure(_symbol->st_name != 0);
	return (const char *)(_object->baseAddress
			+ _object->stringTableOffset + _symbol->st_name);
}

uintptr_t ObjectSymbol::virtualAddress() {
	auto bind = ELF64_ST_BIND(_symbol->st_info);
	__ensure(bind == STB_GLOBAL || bind == STB_WEAK || bind == STB_GNU_UNIQUE);
	__ensure(_symbol->st_shndx != SHN_UNDEF);
	return _object->baseAddress + _symbol->st_value;
}

// --------------------------------------------------------
// Scope
// --------------------------------------------------------

uint32_t elf64Hash(frg::string_view string) {
	uint32_t h = 0, g;

	for(size_t i = 0; i < string.size(); ++i) {
		h = (h << 4) + (uint32_t)string[i];
		g = h & 0xF0000000;
		if(g)
			h ^= g >> 24;
		h &= 0x0FFFFFFF;
	}

	return h;
}

uint32_t gnuHash(frg::string_view string) {
    uint32_t h = 5381;
	for(size_t i = 0; i < string.size(); ++i)
        h = (h << 5) + h + string[i];
    return h;
}

// TODO: move this to some namespace or class?
frg::optional<ObjectSymbol> resolveInObject(SharedObject *object, frg::string_view string) {
	// Checks if the symbol can be used to satisfy the dependency.
	auto eligible = [&] (ObjectSymbol cand) {
		if(cand.symbol()->st_shndx == SHN_UNDEF)
			return false;

		auto bind = ELF64_ST_BIND(cand.symbol()->st_info);
		if(bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_GNU_UNIQUE)
			return false;

		return true;
	};

	if (object->hashStyle == HashStyle::systemV) {
		auto hash_table = (Elf64_Word *)(object->baseAddress + object->hashTableOffset);
		Elf64_Word num_buckets = hash_table[0];
		auto bucket = elf64Hash(string) % num_buckets;

		auto index = hash_table[2 + bucket];
		while(index != 0) {
			ObjectSymbol cand{object, (Elf64_Sym *)(object->baseAddress
					+ object->symbolTableOffset + index * sizeof(Elf64_Sym))};
			if(eligible(cand) && frg::string_view{cand.getString()} == string)
				return cand;

			index = hash_table[2 + num_buckets + index];
		}

		return frg::optional<ObjectSymbol>{};
	}else{
		__ensure(object->hashStyle == HashStyle::gnu);

		struct GnuTable {
			Elf64_Word nBuckets;
			Elf64_Word symbolOffset;
			Elf64_Word bloomSize;
			Elf64_Word bloomShift;
		};

		auto hash_table = reinterpret_cast<const GnuTable *>(object->baseAddress
				+ object->hashTableOffset);
		auto buckets = reinterpret_cast<const Elf64_Word *>(object->baseAddress
				+ object->hashTableOffset + sizeof(GnuTable)
				+ hash_table->bloomSize * sizeof(Elf64_Addr));
		auto chains = reinterpret_cast<const Elf64_Word *>(object->baseAddress
				+ object->hashTableOffset + sizeof(GnuTable)
				+ hash_table->bloomSize * sizeof(Elf64_Addr)
				+ hash_table->nBuckets * sizeof(Elf64_Word));

		// TODO: Use the bloom filter.

		// The symbols of a given bucket are contiguous in the table.
		auto hash = gnuHash(string);
		auto index = buckets[hash % hash_table->nBuckets];

		if(!index)
			return frg::optional<ObjectSymbol>{};

		while(true) {
			// chains[] contains an array of hashes, parallel to the symbol table.
			auto chash = chains[index - hash_table->symbolOffset];
			if ((chash & ~1) == (hash & ~1)) {
				ObjectSymbol cand{object, (Elf64_Sym *)(object->baseAddress
						+ object->symbolTableOffset + index * sizeof(Elf64_Sym))};
				if(eligible(cand) && frg::string_view{cand.getString()} == string)
					return cand;
			}

			// If we hit the end of the chain, the symbol is not present.
			if(chash & 1)
				return frg::optional<ObjectSymbol>{};
			index++;
		}
	}
}

frg::optional<ObjectSymbol> Scope::_resolveNext(frg::string_view string,
		SharedObject *target) {
	// Skip objects until we find the target, and only look for symbols after that.
	size_t i;
	for (i = 0; i < _objects.size(); i++) {
		if (_objects[i] == target)
			break;
	}

	if (i == _objects.size()) {
		mlibc::infoLogger() << "rtdl: object passed to Scope::resolveAfter was not found" << frg::endlog;
		return frg::optional<ObjectSymbol>();
	}

	for (i = i + 1; i < _objects.size(); i++) {
		if(_objects[i]->isMainObject)
			continue;

		frg::optional<ObjectSymbol> p = resolveInObject(_objects[i], string);
		if(p)
			return p;
	}

	return frg::optional<ObjectSymbol>();
}

Scope::Scope(bool isGlobal)
: isGlobal{isGlobal}, _objects(getAllocator()) { }

void Scope::appendObject(SharedObject *object) {
	// Don't insert duplicates.
	for (auto obj : _objects) {
		if (obj == object)
			return;
	}

	_objects.push(object);
}

frg::optional<ObjectSymbol> Scope::resolveGlobalOrLocal(Scope &globalScope,
		Scope *localScope, frg::string_view string, uint64_t skipRts, ResolveFlags flags) {
	auto sym = globalScope.resolveSymbol(string, skipRts, flags | skipGlobalAfterRts);
	if(!sym && localScope) 
		sym = localScope->resolveSymbol(string, skipRts, flags | skipGlobalAfterRts);
	return sym;
}

frg::optional<ObjectSymbol> Scope::resolveGlobalOrLocalNext(Scope &globalScope,
		Scope *localScope, frg::string_view string, SharedObject *origin) {
	auto sym = globalScope._resolveNext(string, origin);
	if(!sym && localScope) {
		sym = localScope->_resolveNext(string, origin);
	}
	return sym;
}

// TODO: let this return uintptr_t
frg::optional<ObjectSymbol> Scope::resolveSymbol(frg::string_view string,
		uint64_t skipRts, ResolveFlags flags) {
	for (auto object : _objects) {
		if((flags & resolveCopy) && object->isMainObject)
			continue;
		if((flags & skipGlobalAfterRts) && object->globalRts > skipRts) {
			// globalRts should be monotone increasing for objects in the global scope,
			// so as an optimization we can break early here.
			// TODO: If we implement DT_SYMBOLIC, this assumption fails.
			if(isGlobal)
				break;
			else
				continue;
		}

		frg::optional<ObjectSymbol> p = resolveInObject(object, string);
		if(p)
			return p;
	}

	return frg::optional<ObjectSymbol>();
}

// --------------------------------------------------------
// Loader
// --------------------------------------------------------

Loader::Loader(Scope *scope, SharedObject *mainExecutable, bool is_initial_link, uint64_t rts)
: _mainExecutable{mainExecutable}, _loadScope{scope}, _isInitialLink{is_initial_link},
		_linkRts{rts}, _linkBfs{getAllocator()}, _initQueue{getAllocator()} { }

void Loader::_buildLinkBfs(SharedObject *root) {
	__ensure(_linkBfs.size() == 0);

	struct Token {};
	using Set = frg::hash_map<SharedObject *, Token,
			frg::hash<SharedObject *>, MemoryAllocator>;
	Set set{frg::hash<SharedObject *>{}, getAllocator()};
	_linkBfs.push(root);

	// Loop over indices (not iterators) here: We are adding elements in the loop!
	for(size_t i = 0; i < _linkBfs.size(); i++) {
		auto current = _linkBfs[i];

		// At this point the object is loaded and we can fill in its debug struct,
		// the linked list fields will be filled later.
		current->linkMap.base = current->baseAddress;
		current->linkMap.name = current->path.data();
		current->linkMap.dynv = current->dynamic;

		__ensure((current->tlsAlignment & (current->tlsAlignment - 1)) == 0);

		if (_isInitialLink && current->tlsAlignment > tlsMaxAlignment) {
			tlsMaxAlignment = current->tlsAlignment;
		}

		for (auto dep : current->dependencies) {
			if (!set.get(dep)) {
				set.insert(dep, Token{});
				_linkBfs.push(dep);
			}
		}
	}
}

void Loader::linkObjects(SharedObject *root) {
	_buildLinkBfs(root);
	_buildTlsMaps();

	// Promote objects to the desired scope.
	for(auto object : _linkBfs) {
		if (object->globalRts == 0 && _loadScope->isGlobal)
			object->globalRts = _linkRts;

		_loadScope->appendObject(object);
	}

	// Process regular relocations.
	for(auto object : _linkBfs) {
		// Some objects have already been linked before.
		if(object->objectRts < _linkRts)
			continue;

		if(object->dynamic == nullptr)
			continue;

		if(verbose)
			mlibc::infoLogger() << "rtdl: Linking " << object->name << frg::endlog;

		__ensure(!object->wasLinked);

		// TODO: Support this.
		if(object->symbolicResolution)
			mlibc::infoLogger() << "\e[31mrtdl: DT_SYMBOLIC is not implemented correctly!\e[39m"
					<< frg::endlog;

		_processStaticRelocations(object);
		_processLazyRelocations(object);
	}

	// Process copy relocations.
	for(auto object : _linkBfs) {
		if(!object->isMainObject)
			continue;

		// Some objects have already been linked before.
		if(object->objectRts < _linkRts)
			continue;

		if(object->dynamic == nullptr)
			continue;

		processCopyRelocations(object);
	}

	for(auto object : _linkBfs) {
		object->wasLinked = true;

		if(object->inLinkMap)
			continue;

		auto linkMap = reinterpret_cast<LinkMap*>(globalDebugInterface.head);

		object->linkMap.prev = linkMap;
		object->linkMap.next = linkMap->next;
		if(linkMap->next)
			linkMap->next->prev = &(object->linkMap);
		linkMap->next = &(object->linkMap);
		object->inLinkMap = true;
	}
}

void Loader::_buildTlsMaps() {
	if(_isInitialLink) {
		__ensure(runtimeTlsMap->initialPtr == 0);
		__ensure(runtimeTlsMap->initialLimit == 0);

		__ensure(!_linkBfs.empty());
		__ensure(_linkBfs.front()->isMainObject);

		for(auto object : _linkBfs) {
			__ensure(object->tlsModel == TlsModel::null);

			if(object->tlsSegmentSize == 0)
				continue;

			// Allocate an index for the object.
			object->tlsIndex = runtimeTlsMap->indices.size();
			runtimeTlsMap->indices.push_back(object);

			object->tlsModel = TlsModel::initial;

			if constexpr (tlsAboveTp) {
				// As per the comment in allocateTcb(), we may simply add sizeof(Tcb) to
				// reach the TLS data.
				object->tlsOffset = runtimeTlsMap->initialPtr + sizeof(Tcb);
				runtimeTlsMap->initialPtr += object->tlsSegmentSize;

				size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
				if(misalign)
					runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;
			} else {
				runtimeTlsMap->initialPtr += object->tlsSegmentSize;

				size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
				if(misalign)
					runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

				object->tlsOffset = -runtimeTlsMap->initialPtr;
			}

			if(verbose)
				mlibc::infoLogger() << "rtdl: TLS of " << object->name
						<< " mapped to 0x" << frg::hex_fmt{object->tlsOffset}
						<< ", size: " << object->tlsSegmentSize
						<< ", alignment: " << object->tlsAlignment << frg::endlog;
		}

		// Reserve some additional space for future libraries.
		runtimeTlsMap->initialLimit = runtimeTlsMap->initialPtr + 64;
	}else{
		for(auto object : _linkBfs) {
			if(object->tlsModel != TlsModel::null)
				continue;
			if(object->tlsSegmentSize == 0)
				continue;

			// Allocate an index for the object.
			object->tlsIndex = runtimeTlsMap->indices.size();
			runtimeTlsMap->indices.push_back(object);

			// There are some libraries (e.g. Mesa) that require static TLS even though
			// they expect to be dynamically loaded.
			if(object->haveStaticTls) {
				auto ptr = runtimeTlsMap->initialPtr + object->tlsSegmentSize;
				size_t misalign = ptr & (object->tlsAlignment - 1);
				if(misalign)
					ptr += object->tlsAlignment - misalign;

				if(ptr > runtimeTlsMap->initialLimit)
					mlibc::panicLogger() << "rtdl: Static TLS space exhausted while while"
							" allocating TLS for " << object->name << frg::endlog;

				object->tlsModel = TlsModel::initial;

				if constexpr (tlsAboveTp) {
					size_t tcbSize = ((sizeof(Tcb) + tlsMaxAlignment - 1) & ~(tlsMaxAlignment - 1));

					object->tlsOffset = runtimeTlsMap->initialPtr + tcbSize;
					runtimeTlsMap->initialPtr = ptr;
				} else {
					runtimeTlsMap->initialPtr = ptr;
					object->tlsOffset = -runtimeTlsMap->initialPtr;
				}

				if(verbose)
					mlibc::infoLogger() << "rtdl: TLS of " << object->name
							<< " mapped to 0x" << frg::hex_fmt{object->tlsOffset}
							<< ", size: " << object->tlsSegmentSize
							<< ", alignment: " << object->tlsAlignment << frg::endlog;
			}else{
				object->tlsModel = TlsModel::dynamic;
			}
		}
	}
}

void Loader::initObjects() {
	initTlsObjects(mlibc::get_current_tcb(), _linkBfs, true);

	if (_mainExecutable && _mainExecutable->preInitArray) {
		if (verbose)
			mlibc::infoLogger() << "rtdl: Running DT_PREINIT_ARRAY functions" << frg::endlog;

		__ensure(_mainExecutable->isMainObject);
		__ensure(!_mainExecutable->wasInitialized);
		__ensure((_mainExecutable->preInitArraySize % sizeof(InitFuncPtr)) == 0);
		for(size_t i = 0; i < _mainExecutable->preInitArraySize / sizeof(InitFuncPtr); i++)
			_mainExecutable->preInitArray[i]();
	}

	// Convert the breadth-first representation to a depth-first post-order representation,
	// so that every object is initialized *after* its dependencies.
	for(auto object : _linkBfs) {
		if(!object->scheduledForInit)
			_scheduleInit(object);
	}

	for(auto object : _initQueue) {
		if(!object->wasInitialized)
			doInitialize(object);
	}
}

// TODO: Use an explicit vector to reduce stack usage to O(1)?
void Loader::_scheduleInit(SharedObject *object) {
	// Here we detect cyclic dependencies.
	__ensure(!object->onInitStack);
	object->onInitStack = true;

	__ensure(!object->scheduledForInit);
	object->scheduledForInit = true;

	for(size_t i = 0; i < object->dependencies.size(); i++) {
		if(!object->dependencies[i]->scheduledForInit)
			_scheduleInit(object->dependencies[i]);
	}

	_initQueue.push(object);
	object->onInitStack = false;
}

void Loader::_processRela(SharedObject *object, Elf64_Rela *reloc) {
	Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
	Elf64_Xword symbol_index = ELF64_R_SYM(reloc->r_info);

	// copy relocations have to be performed after all other relocations
#if defined(__x86_64__)
	if(type == R_X86_64_COPY)
		return;
#elif defined(__aarch64__)
	if(type == R_AARCH64_COPY)
		return;
#elif defined(__riscv)
	if(type == R_RISCV_COPY)
		return;
#endif

	// resolve the symbol if there is a symbol
	frg::optional<ObjectSymbol> p;
	if(symbol_index) {
		auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
				+ symbol_index * sizeof(Elf64_Sym));
		ObjectSymbol r(object, symbol);

		p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope,
				r.getString(), object->objectRts, 0);
		if(!p) {
			if(ELF64_ST_BIND(symbol->st_info) != STB_WEAK)
				mlibc::panicLogger() << "Unresolved load-time symbol "
						<< r.getString() << " in object " << object->name << frg::endlog;

			if(verbose)
				mlibc::infoLogger() << "rtdl: Unresolved weak load-time symbol "
						<< r.getString() << " in object " << object->name << frg::endlog;
		}
	}

	uintptr_t rel_addr = object->baseAddress + reloc->r_offset;

	switch(type) {
#if defined(__x86_64__)
	case R_X86_64_64: {
		__ensure(symbol_index);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr + reloc->r_addend;
	} break;
	case R_X86_64_GLOB_DAT: {
		__ensure(symbol_index);
		__ensure(!reloc->r_addend);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr;
	} break;
	case R_X86_64_JUMP_SLOT: {
		__ensure(symbol_index);
		__ensure(!reloc->r_addend);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr;
	} break;
	case R_X86_64_RELATIVE: {
		__ensure(!symbol_index);
		*((uint64_t *)rel_addr) = object->baseAddress + reloc->r_addend;
	} break;
	// DTPMOD and DTPOFF are dynamic TLS relocations (for __tls_get_addr()).
	// TPOFF is a relocation to the initial TLS model.
	case R_X86_64_DTPMOD64: {
		__ensure(!reloc->r_addend);
		if(symbol_index) {
			__ensure(p);
			*((uint64_t *)rel_addr) = (uint64_t)p->object();
		}else{
			// TODO: is this behaviour actually documented anywhere?
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtdl: Warning: DTPOFF64 with no symbol"
						" in object " << object->name << frg::endlog;
			*((uint64_t *)rel_addr) = (uint64_t)object;
		}
	} break;
	case R_X86_64_DTPOFF64: {
		__ensure(p);
		__ensure(!reloc->r_addend);
		*((uint64_t *)rel_addr) = p->symbol()->st_value;
	} break;
	case R_X86_64_TPOFF64: {
		if(symbol_index) {
			__ensure(p);
			__ensure(!reloc->r_addend);
			if(p->object()->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to symbol " << p->getString()
						<< " in dynamically loaded object "
						<< p->object()->name << frg::endlog;
			*((uint64_t *)rel_addr) = p->object()->tlsOffset + p->symbol()->st_value;
		}else{
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtdl: Warning: TPOFF64 with no symbol"
						" in object " << object->name << frg::endlog;
			if(object->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< object->name << frg::endlog;
			*((uint64_t *)rel_addr) = object->tlsOffset + reloc->r_addend;
		}
	} break;
#elif defined(__aarch64__)
	case R_AARCH64_ABS64: {
		__ensure(symbol_index);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr + reloc->r_addend;
	} break;
	case R_AARCH64_GLOB_DAT: {
		__ensure(symbol_index);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr + reloc->r_addend;
	} break;
	case R_AARCH64_RELATIVE: {
		__ensure(!symbol_index);
		*((uint64_t *)rel_addr) = object->baseAddress + reloc->r_addend;
	} break;
	case R_AARCH64_TLS_TPREL: {
		if(symbol_index) {
			__ensure(p);
			__ensure(!reloc->r_addend);
			if(p->object()->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< p->object()->name << frg::endlog;
			*((uint64_t *)rel_addr) = p->object()->tlsOffset + p->symbol()->st_value;
		}else{
			__ensure(!reloc->r_addend);
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtdl: Warning: TLS_TPREL with no symbol"
						" in object " << object->name << frg::endlog;
			if(object->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< object->name << frg::endlog;
			*((uint64_t *)rel_addr) = object->tlsOffset;
		}
	} break;
#elif defined(__riscv)
	case R_RISCV_64: {
		__ensure(symbol_index);
		__ensure(p);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr + reloc->r_addend;
	} break;
	case R_RISCV_JUMP_SLOT: {
		__ensure(symbol_index);
		__ensure(p);
		__ensure(!reloc->r_addend);
		uint64_t symbol_addr = p ? p->virtualAddress() : 0;
		*((uint64_t *)rel_addr) = symbol_addr;
	} break;
	case R_RISCV_RELATIVE: {
		__ensure(!symbol_index);
		*((uint64_t *)rel_addr) = object->baseAddress + reloc->r_addend;
	} break;
	case R_RISCV_TLS_DTPMOD64: {
		__ensure(!reloc->r_addend);
		if(symbol_index) {
			__ensure(p);
			*((uint64_t *)rel_addr) = (uint64_t)p->object();
		}else{
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtdl: Warning: TLS_DTPMOD64 with no symbol"
						" in object " << object->name << frg::endlog;
			*((uint64_t *)rel_addr) = (uint64_t)object;
		}
	} break;
	case R_RISCV_TLS_DTPREL64: {
		__ensure(symbol_index);
		__ensure(p);
		*((uint64_t *)rel_addr) = p->symbol()->st_value + reloc->r_addend - TLS_DTV_OFFSET;
	} break;
	case R_RISCV_TLS_TPREL64: {
		// Note: this assumes that tcb_ptr + sizeof(Tcb) == tp.
		if(symbol_index) {
			__ensure(p);
			__ensure(!reloc->r_addend);
			if(p->object()->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< p->object()->name << frg::endlog;
			*((uint64_t *)rel_addr) = p->object()->tlsOffset - sizeof(Tcb) + p->symbol()->st_value;
		}else{
			__ensure(!reloc->r_addend);
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtdl: Warning: TLS_TPREL64 with no symbol"
						" in object " << object->name << frg::endlog;
			if(object->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< object->name << frg::endlog;
			*((uint64_t *)rel_addr) = object->tlsOffset - sizeof(Tcb);
		}
	} break;
	case R_RISCV_NONE:
		// Work around https://sourceware.org/bugzilla/show_bug.cgi?id=24673.
		break;
#endif
	default:
		mlibc::panicLogger() << "Unexpected relocation type "
				<< (void *)type << frg::endlog;
	}
}

void Loader::_processStaticRelocations(SharedObject *object) {
	frg::optional<uintptr_t> rela_offset;
	frg::optional<size_t> rela_length;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];

		switch(dynamic->d_tag) {
		case DT_RELA:
			rela_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_un.d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_un.d_val == sizeof(Elf64_Rela));
			break;
		}
	}

	if(rela_offset && rela_length) {
		for(size_t offset = 0; offset < *rela_length; offset += sizeof(Elf64_Rela)) {
			auto reloc = (Elf64_Rela *)(object->baseAddress + *rela_offset + offset);
			_processRela(object, reloc);
		}
	}else{
		__ensure(!rela_offset && !rela_length);
	}
}

// TODO: TLSDESC relocations aren't aarch64 specific
#ifdef __aarch64__
extern "C" void *__mlibcTlsdescStatic(void *);
extern "C" void *__mlibcTlsdescDynamic(void *);
#endif

void Loader::_processLazyRelocations(SharedObject *object) {
	if(object->globalOffsetTable == nullptr) {
		__ensure(object->lazyRelocTableOffset == 0);
		return;
	}
	object->globalOffsetTable[1] = object;
	object->globalOffsetTable[2] = (void *)&pltRelocateStub;

	if(!object->lazyTableSize)
		return;

	// adjust the addresses of JUMP_SLOT relocations
	__ensure(object->lazyExplicitAddend);
	for(size_t offset = 0; offset < object->lazyTableSize; offset += sizeof(Elf64_Rela)) {
		auto reloc = (Elf64_Rela *)(object->baseAddress + object->lazyRelocTableOffset + offset);
		Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
		Elf64_Xword symbol_index = ELF64_R_SYM(reloc->r_info);
		uintptr_t rel_addr = object->baseAddress + reloc->r_offset;

		switch (type) {
#if defined(__x86_64__)
		case R_X86_64_JUMP_SLOT:
#elif defined(__aarch64__)
		case R_AARCH64_JUMP_SLOT:
#elif defined(__riscv)
		case R_RISCV_JUMP_SLOT:
#endif
			if(eagerBinding) {
				auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
						+ symbol_index * sizeof(Elf64_Sym));
				ObjectSymbol r(object, symbol);
				auto p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, r.getString(), object->objectRts, 0);

				if(!p) {
					if(ELF64_ST_BIND(symbol->st_info) != STB_WEAK)
						mlibc::panicLogger() << "rtdl: Unresolved JUMP_SLOT symbol "
								<< r.getString() << " in object " << object->name << frg::endlog;

					if(verbose)
						mlibc::infoLogger() << "rtdl: Unresolved weak JUMP_SLOT symbol "
							<< r.getString() << " in object " << object->name << frg::endlog;
					*((uint64_t *)rel_addr) = 0;
				}else{
					*((uint64_t *)rel_addr) = p->virtualAddress();
				}
			}else{
				*((uint64_t *)rel_addr) += object->baseAddress;
			}
			break;
// TODO: TLSDESC relocations aren't aarch64 specific
#if defined(__aarch64__)
		case R_AARCH64_TLSDESC: {
			size_t symValue = 0;
			SharedObject *target = nullptr;

			if (symbol_index) {
				auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
						+ symbol_index * sizeof(Elf64_Sym));
				ObjectSymbol r(object, symbol);
				auto p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, r.getString(), object->objectRts, 0);

				if (!p) {
					__ensure(ELF64_ST_BIND(symbol->st_info) != STB_WEAK);
					mlibc::panicLogger() << "rtdl: Unresolved TLSDESC for symbol "
						<< r.getString() << " in object " << object->name << frg::endlog;
				} else {
					target = p->object();
					if (p->symbol())
						symValue = p->symbol()->st_value;
				}
			} else {
				target = object;
			}

			__ensure(target);

			if (target->tlsModel == TlsModel::initial) {
				((uint64_t *)rel_addr)[0] = reinterpret_cast<uintptr_t>(&__mlibcTlsdescStatic);
				((uint64_t *)rel_addr)[1] = symValue + target->tlsOffset + reloc->r_addend;
			} else {
				struct TlsdescData {
					uintptr_t tlsIndex;
					uintptr_t addend;
				};

				// Access DTV for object to force the entry to be allocated and initialized
				accessDtv(target);

				__ensure(target->tlsIndex < mlibc::get_current_tcb()->dtvSize);

				// TODO: We should free this when the DSO gets destroyed
				auto data = frg::construct<TlsdescData>(getAllocator());
				data->tlsIndex = target->tlsIndex;
				data->addend = symValue + reloc->r_addend;

				((uint64_t *)rel_addr)[0] = reinterpret_cast<uintptr_t>(&__mlibcTlsdescDynamic);
				((uint64_t *)rel_addr)[1] = reinterpret_cast<uintptr_t>(data);
			}
		} break;
#endif
		default:
			mlibc::panicLogger() << "unimplemented lazy relocation type " << type << frg::endlog;
			break;
		}
	}
}

