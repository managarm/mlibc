#include <mlibc/arch-defs.hpp>
#include <stdint.h>
#include <string.h>

// keep a list of optional generic relocation types
enum {
	R_OFFSET =  (uintptr_t) -1,
};


#include <frg/manual_box.hpp>
#include <frg/small_vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/rtld-sysdeps.hpp>
#include <mlibc/rtld-abi.hpp>
#include <mlibc/thread.hpp>
#include <abi-bits/fcntl.h>
#include <internal-config.h>

#include "elf.hpp"
#include "linker.hpp"

#if !MLIBC_MMAP_ALLOCATE_DSO
uintptr_t libraryBase = 0x41000000;
#endif

constexpr bool verbose = false;
constexpr bool stillSlightlyVerbose = false;
constexpr bool logBaseAddresses = false;
constexpr bool logRpath = false;
constexpr bool logLdPath = false;
constexpr bool logSymbolVersions = false;
constexpr bool eagerBinding = true;

#if defined(__x86_64__) || defined(__i386__)
constexpr inline bool tlsAboveTp = false;
constexpr inline uintptr_t tlsOffsetFromTp = 0;
#elif defined(__aarch64__)
constexpr inline bool tlsAboveTp = true;
constexpr inline uintptr_t tlsOffsetFromTp = 16;
#elif defined(__riscv)
constexpr inline bool tlsAboveTp = true;
constexpr inline uintptr_t tlsOffsetFromTp = 0;
#elif defined(__m68k__)
constexpr inline bool tlsAboveTp = true;
constexpr inline ptrdiff_t tlsOffsetFromTp = -0x7000;
#elif defined(__loongarch64)
constexpr inline bool tlsAboveTp = true;
constexpr inline uintptr_t tlsOffsetFromTp = 0;
#else
#	error Unknown architecture
#endif

extern DebugInterface globalDebugInterface;
extern uintptr_t __stack_chk_guard;

extern frg::manual_box<frg::small_vector<frg::string_view, MLIBC_NUM_DEFAULT_LIBRARY_PATHS, MemoryAllocator>> libraryPaths;
extern frg::manual_box<frg::vector<frg::string_view, MemoryAllocator>> preloads;

#if MLIBC_STATIC_BUILD
extern "C" size_t __init_array_start[];
extern "C" size_t __init_array_end[];
extern "C" size_t __fini_array_start[];
extern "C" size_t __fini_array_end[];
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

bool trySeek(int fd, int64_t offset) {
	off_t noff;
	return mlibc::sys_seek(fd, offset, SEEK_SET, &noff) == 0;
}

bool tryReadExactly(int fd, void *data, size_t length) {
	size_t offset = 0;
	while(offset < length) {
		ssize_t chunk;
		if(mlibc::sys_read(fd, reinterpret_cast<char *>(data) + offset,
				length - offset, &chunk))
			return false;
		__ensure(chunk > 0);
		offset += chunk;
	}
	__ensure(offset == length);
	return true;
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
	dependencyQueue{getAllocator()},
	_nameMap{frg::hash<frg::string_view>{}, getAllocator()},
	_destructQueue{getAllocator()} {}

SharedObject *ObjectRepository::injectObjectFromDts(frg::string_view name,
		frg::string<MemoryAllocator> path, uintptr_t base_address,
		elf_dyn *dynamic, uint64_t rts) {
	__ensure(!findLoadedObject(name));

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), false, globalScope.get(), rts);
	object->baseAddress = base_address;
	object->dynamic = dynamic;
	_parseDynamic(object);
	_parseVerdef(object);

	object->wasVisited = true;
	dependencyQueue.push_back(object);
	_addLoadedObject(object);

	return object;
}

SharedObject *ObjectRepository::injectObjectFromPhdrs(frg::string_view name,
		frg::string<MemoryAllocator> path, void *phdr_pointer,
		size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
		uint64_t rts) {
	__ensure(!findLoadedObject(name));

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), true, globalScope.get(), rts);
	_fetchFromPhdrs(object, phdr_pointer, phdr_entry_size, num_phdrs, entry_pointer);
	_parseDynamic(object);
	_parseVerdef(object);

	object->wasVisited = true;
	dependencyQueue.push_back(object);
	_addLoadedObject(object);

	return object;
}

SharedObject *ObjectRepository::injectStaticObject(frg::string_view name,
		frg::string<MemoryAllocator> path, void *phdr_pointer,
		size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
		uint64_t rts) {
	__ensure(!findLoadedObject(name));
	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(path), true, globalScope.get(), rts);
	_fetchFromPhdrs(object, phdr_pointer, phdr_entry_size, num_phdrs, entry_pointer);

#if MLIBC_STATIC_BUILD
	object->initArray = reinterpret_cast<InitFuncPtr*>(__init_array_start);
	object->initArraySize = static_cast<size_t>((uintptr_t)__init_array_end -
			(uintptr_t)__init_array_start);
	object->finiArray = reinterpret_cast<InitFuncPtr*>(__fini_array_start);
	object->finiArraySize = static_cast<size_t>((uintptr_t)__fini_array_end -
			(uintptr_t)__fini_array_start);
	object->preInitArray = reinterpret_cast<InitFuncPtr*>(__preinit_array_start);
	object->preInitArraySize = static_cast<size_t>((uintptr_t)__preinit_array_end -
			(uintptr_t)__preinit_array_start);
#endif

	_addLoadedObject(object);

	return object;
}

frg::expected<LinkerError, SharedObject *> ObjectRepository::requestObjectWithName(frg::string_view name,
		SharedObject *origin, Scope *localScope, bool createScope, uint64_t rts) {
	if (auto obj = findLoadedObject(name))
		return obj;

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
			if (lastsl != size_t(-1)) {
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
			mlibc::infoLogger() << "rtld: trying in rpath " << sPath << frg::endlog;
		int fd = tryToOpen(sPath.data());
		if (logRpath && fd >= 0)
			mlibc::infoLogger() << "rtld: found in rpath" << frg::endlog;
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
		mlibc::infoLogger() << "rtld: no rpath set for object" << frg::endlog;
	}

	for(size_t i = 0; i < libraryPaths->size() && fd == -1; i++) {
		auto ldPath = (*libraryPaths)[i];
		auto path = frg::string<MemoryAllocator>{getAllocator(), ldPath} + '/' + name;
		if(logLdPath)
			mlibc::infoLogger() << "rtld: Trying to load " << name << " from ldpath " << ldPath << "/" << frg::endlog;
		fd = tryToOpen(path.data());
		if(fd >= 0) {
			chosenPath = std::move(path);
			break;
		}
	}
	if(fd == -1)
		return LinkerError::notFound;

	if (createScope) {
		__ensure(localScope == nullptr);

		// TODO: Free this when the scope is no longer needed.
		localScope = frg::construct<Scope>(getAllocator());
	}

	__ensure(localScope != nullptr);

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), std::move(chosenPath), false, localScope, rts);

	auto result = _fetchFromFile(object, fd);
	closeOrDie(fd);
	if(!result) {
		frg::destruct(getAllocator(), object);
		return result.error();
	}

	_parseDynamic(object);
	_parseVerdef(object);
	_addLoadedObject(object);

	return object;
}

frg::expected<LinkerError, SharedObject *> ObjectRepository::requestObjectAtPath(frg::string_view path,
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

	__ensure(localScope != nullptr);

	auto object = frg::construct<SharedObject>(getAllocator(),
		name.data(), path.data(), false, localScope, rts);

	frg::string<MemoryAllocator> no_prefix(getAllocator(), path);

	int fd;
	if(mlibc::sys_open((no_prefix + '\0').data(), O_RDONLY, 0, &fd)) {
		frg::destruct(getAllocator(), object);
		return LinkerError::notFound;
	}
	auto result = _fetchFromFile(object, fd);
	closeOrDie(fd);
	if(!result) {
		frg::destruct(getAllocator(), object);
		return result.error();
	}

	_parseDynamic(object);
	_parseVerdef(object);
	_addLoadedObject(object);

	return object;
}

void ObjectRepository::discoverDependenciesFromLoadedObject(SharedObject *object) {
	_discoverDependencies(object, object->localScope, object->objectRts);
	_parseVerneed(object);
}

SharedObject *ObjectRepository::findCaller(void *addr) {
	uintptr_t target = reinterpret_cast<uintptr_t>(addr);

	for (auto [name, object] : _nameMap) {
		// Search all PT_LOAD segments for the specified address.
		for(size_t j = 0; j < object->phdrCount; j++) {
			auto phdr = (elf_phdr *)((uintptr_t)object->phdrPointer + j * object->phdrEntrySize);
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

void ObjectRepository::addObjectToDestructQueue(SharedObject *object) {
	_destructQueue.push(object);
}

void doDestruct(SharedObject *object);

void ObjectRepository::destructObjects() {
	while (_destructQueue.size() > 0) {
		auto top = _destructQueue.top();
		doDestruct(top);
		_destructQueue.pop();
	}
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
		mlibc::infoLogger() << "rtld: Loading " << object->name << frg::endlog;

	// Note: the entry pointer is absolute and not relative to the base address.
	object->entry = entry_pointer;

	frg::optional<ptrdiff_t> dynamic_offset;
	frg::optional<ptrdiff_t> tls_offset;

	// segments are already mapped, so we just have to find the dynamic section
	for(size_t i = 0; i < phdr_count; i++) {
		auto phdr = (elf_phdr *)((uintptr_t)phdr_pointer + i * phdr_entry_size);
		switch(phdr->p_type) {
		case PT_PHDR:
			// Determine the executable's base address (in the PIE case) by comparing
			// the PHDR segment's load address against it's address in the ELF file.
			object->baseAddress = reinterpret_cast<uintptr_t>(phdr_pointer) - phdr->p_vaddr;
			if(verbose)
				mlibc::infoLogger() << "rtld: Executable is loaded at "
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
		object->dynamic = (elf_dyn *)(object->baseAddress + *dynamic_offset);
	if(tls_offset)
		object->tlsImagePtr = (void *)(object->baseAddress + *tls_offset);
}


frg::expected<LinkerError, void> ObjectRepository::_fetchFromFile(SharedObject *object, int fd) {
	__ensure(!object->isMainObject);

	// read the elf file header
	elf_ehdr ehdr;
	if(!tryReadExactly(fd, &ehdr, sizeof(elf_ehdr)))
		return LinkerError::fileTooShort;

	if(ehdr.e_ident[0] != 0x7F
			|| ehdr.e_ident[1] != 'E'
			|| ehdr.e_ident[2] != 'L'
			|| ehdr.e_ident[3] != 'F')
		return LinkerError::notElf;

	if((ehdr.e_type != ET_EXEC && ehdr.e_type != ET_DYN)
			|| ehdr.e_machine != ELF_MACHINE
			|| ehdr.e_ident[EI_CLASS] != ELF_CLASS)
		return LinkerError::wrongElfType;

	// read the elf program headers
	auto phdr_buffer = (char *)getAllocator().allocate(ehdr.e_phnum * ehdr.e_phentsize);
	if(!phdr_buffer)
		return LinkerError::outOfMemory;

	if(!trySeek(fd, ehdr.e_phoff)) {
		getAllocator().deallocate(phdr_buffer, ehdr.e_phnum * ehdr.e_phentsize);
		return LinkerError::invalidProgramHeader;
	}
	if(!tryReadExactly(fd, phdr_buffer, ehdr.e_phnum * ehdr.e_phentsize)) {
		getAllocator().deallocate(phdr_buffer, ehdr.e_phnum * ehdr.e_phentsize);
		return LinkerError::invalidProgramHeader;
	}

	object->phdrPointer = phdr_buffer;
	object->phdrCount = ehdr.e_phnum;
	object->phdrEntrySize = ehdr.e_phentsize;

	// Allocate virtual address space for the DSO.
	constexpr size_t hugeSize = 0x200000;

	uintptr_t highest_address = 0;
	for(int i = 0; i < ehdr.e_phnum; i++) {
		auto phdr = (elf_phdr *)(phdr_buffer + i * ehdr.e_phentsize);

		if(phdr->p_type != PT_LOAD)
			continue;

		auto limit = phdr->p_vaddr + phdr->p_memsz;
		if(limit > highest_address)
			highest_address = limit;
	}

	__ensure(!(object->baseAddress & (hugeSize - 1)));

	highest_address = (highest_address + mlibc::page_size - 1) & ~(mlibc::page_size - 1);

#if MLIBC_MMAP_ALLOCATE_DSO
	void *mappedAddr = nullptr;

	if (mlibc::sys_vm_map(nullptr,
			highest_address - object->baseAddress, PROT_NONE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, &mappedAddr)) {
		mlibc::infoLogger() << "sys_vm_map failed when allocating address space for DSO \""
				<< object->name << "\""
				<< ", base " << (void *)object->baseAddress
				<< ", requested " << (highest_address - object->baseAddress) << " bytes"
				<< frg::endlog;
		getAllocator().deallocate(phdr_buffer, ehdr.e_phnum * ehdr.e_phentsize);
		return LinkerError::outOfMemory;
	}

	object->baseAddress = reinterpret_cast<uintptr_t>(mappedAddr);
#else
	object->baseAddress = libraryBase;
	libraryBase += (highest_address + (hugeSize - 1)) & ~(hugeSize - 1);
#endif

	if(verbose || logBaseAddresses)
		mlibc::infoLogger() << "rtld: Loading " << object->name
				<< " at " << (void *)object->baseAddress << frg::endlog;

	// Load all segments.
	constexpr size_t pageSize = 0x1000;
	for(int i = 0; i < ehdr.e_phnum; i++) {
		auto phdr = (elf_phdr *)(phdr_buffer + i * ehdr.e_phentsize);

		if(phdr->p_type == PT_LOAD) {
			size_t misalign = phdr->p_vaddr & (pageSize - 1);
			if(!phdr->p_memsz)
				continue;
			__ensure(phdr->p_memsz >= phdr->p_filesz);

			// If the following condition is violated, we cannot use mmap() the segment;
			// however, GCC only generates ELF files that satisfy this.
			__ensure(misalign == (phdr->p_offset & (pageSize - 1)));

			auto map_address = object->baseAddress + phdr->p_vaddr - misalign;
			auto backed_map_size = (phdr->p_filesz + misalign + pageSize - 1) & ~(pageSize - 1);
			auto total_map_size = (phdr->p_memsz + misalign + pageSize - 1) & ~(pageSize - 1);
			auto initial_prot = PROT_READ | PROT_WRITE;

			int prot = 0;
			if(phdr->p_flags & PF_R)
				prot |= PROT_READ;
			if(phdr->p_flags & PF_W)
				prot |= PROT_WRITE;
			if(phdr->p_flags & PF_X)
				prot |= PROT_EXEC;

		#if MLIBC_MAP_DSO_SEGMENTS
			// we can avoid the vm_protect call if we don't have to write to the segment
			if(phdr->p_memsz == phdr->p_filesz)
				initial_prot = prot;

			void *map_pointer;
			if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address),
					backed_map_size, initial_prot,
					MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset - misalign, &map_pointer))
				__ensure(!"sys_vm_map failed");
			if(total_map_size > backed_map_size)
				if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address + backed_map_size),
						total_map_size - backed_map_size, initial_prot,
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
					total_map_size, initial_prot,
					MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0, &map_pointer))
				__ensure(!"sys_vm_map failed");

			__ensure(trySeek(fd, phdr->p_offset));
			__ensure(tryReadExactly(fd, reinterpret_cast<char *>(map_address) + misalign,
					phdr->p_filesz));
		#endif
			if(initial_prot != prot) {
				if (!mlibc::sys_vm_protect)
					__ensure(!"sys_vm_protect not provided");

				if (mlibc::sys_vm_protect(reinterpret_cast<void *>(map_address), total_map_size, prot))
					__ensure(!"sys_vm_protect failed");
			}
		}else if(phdr->p_type == PT_TLS) {
			object->tlsSegmentSize = phdr->p_memsz;
			object->tlsAlignment = phdr->p_align;
			object->tlsImageSize = phdr->p_filesz;
			object->tlsImagePtr = (void *)(object->baseAddress + phdr->p_vaddr);
		}else if(phdr->p_type == PT_DYNAMIC) {
			object->dynamic = (elf_dyn *)(object->baseAddress + phdr->p_vaddr);
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

	return frg::success;
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
	/* If true, ignore the RPATH.  */
	bool runpath_found = false;
	frg::optional<ptrdiff_t> soname_offset;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		elf_dyn *dynamic = &object->dynamic[i];
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
			__ensure(dynamic->d_un.d_val == sizeof(elf_sym));
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
				object->lazyExplicitAddend = false;
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
#else
			if(dynamic->d_un.d_val & DF_TEXTREL)
				mlibc::panicLogger() << "\e[31mrtld: DF_TEXTREL is unimplemented" << frg::endlog;
#endif
			if(dynamic->d_un.d_val & ~ignored)
				mlibc::infoLogger() << "\e[31mrtld: DT_FLAGS(" << frg::hex_fmt{dynamic->d_un.d_val & ~ignored}
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
				mlibc::infoLogger() << "\e[31mrtld: DT_FLAGS_1(" << frg::hex_fmt{dynamic->d_un.d_val}
						<< ") is not implemented correctly!\e[39m"
						<< frg::endlog;
			break;
		case DT_RPATH:
			if (runpath_found) {
				/* Ignore RPATH if RUNPATH was present.  */
				break;
			}
			[[fallthrough]];
		case DT_RUNPATH:
			runpath_found = dynamic->d_tag == DT_RUNPATH;
			runpath_offset = dynamic->d_un.d_val;
			break;
		case DT_INIT:
			if(dynamic->d_un.d_ptr != 0)
				object->initPtr = (InitFuncPtr)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_FINI:
			if(dynamic->d_un.d_ptr != 0)
				object->finiPtr = (InitFuncPtr)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_INIT_ARRAY:
			if(dynamic->d_un.d_ptr != 0)
				object->initArray = (InitFuncPtr *)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_FINI_ARRAY:
			if(dynamic->d_un.d_ptr != 0)
				object->finiArray = (InitFuncPtr *)(object->baseAddress + dynamic->d_un.d_ptr);
			break;
		case DT_INIT_ARRAYSZ:
			object->initArraySize = dynamic->d_un.d_val;
			break;
		case DT_FINI_ARRAYSZ:
			object->finiArraySize = dynamic->d_un.d_val;
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
#if ELF_CLASS == ELFCLASS32
			dynamic->d_un.d_val = reinterpret_cast<Elf32_Word>(&globalDebugInterface);
#elif ELF_CLASS == ELFCLASS64
			dynamic->d_un.d_val = reinterpret_cast<Elf64_Xword>(&globalDebugInterface);
#endif
			break;
		case DT_SONAME:
			soname_offset = dynamic->d_un.d_val;
			break;
		// handle version information
		case DT_VERSYM:
			object->versionTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_VERDEF:
			object->versionDefinitionTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_VERDEFNUM:
			object->versionDefinitionCount = dynamic->d_un.d_val;
			break;
		case DT_VERNEED:
			object->versionRequirementTableOffset = dynamic->d_un.d_ptr;
			break;
		case DT_VERNEEDNUM:
			object->versionRequirementCount = dynamic->d_un.d_val;
			break;
		// ignore unimportant tags
		case DT_NEEDED: // we handle this later
		case DT_RELA: case DT_RELASZ: case DT_RELAENT: case DT_RELACOUNT:
		case DT_REL: case DT_RELSZ: case DT_RELENT: case DT_RELCOUNT:
		case DT_RELR: case DT_RELRSZ: case DT_RELRENT:
#ifdef __riscv
		case DT_TEXTREL: // Work around https://sourceware.org/bugzilla/show_bug.cgi?id=24673.
#endif
			break;
		case DT_TLSDESC_PLT: case DT_TLSDESC_GOT:
			break;
		default:
			// Ignore unknown entries in the os-specific area as we don't use them.
			if(dynamic->d_tag < DT_LOOS || dynamic->d_tag > DT_HIOS) {
				mlibc::panicLogger() << "Unexpected dynamic entry "
					<< (void *)dynamic->d_tag << " in object" << frg::endlog;
			}
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

void ObjectRepository::_parseVerdef(SharedObject *object) {
	if(!object->versionDefinitionTableOffset) {
		if(verbose)
			mlibc::infoLogger()
				<< "mlibc: Object " << object->name
				<< " defines no versions" << frg::endlog;
		return;
	}

	if(verbose)
		mlibc::infoLogger()
			<< "mlibc: Object " << object->name
			<< " defines " << object->versionDefinitionCount
			<< " version(s)" << frg::endlog;

	uintptr_t address =
		object->baseAddress
		+ object->versionDefinitionTableOffset;

	for(size_t i = 0; i < object->versionDefinitionCount; i++) {
		elf_verdef def;
		memcpy(&def, reinterpret_cast<void *>(address), sizeof(elf_verdef));

		// Required by spec.
		__ensure(def.vd_version == 1);
		__ensure(def.vd_cnt >= 1);
		__ensure(!(def.vd_flags & ~(VER_FLG_BASE | VER_FLG_WEAK)));

		// NOTE(qookie): glibc also ignores any additional Verdaux entries after the
		// first one.
		elf_verdaux aux;
		memcpy(&aux, reinterpret_cast<void *>(address + def.vd_aux), sizeof(elf_verdaux));

		const char *name =
			reinterpret_cast<const char *>(
				object->baseAddress
				+ object->stringTableOffset + aux.vda_name);

		if(verbose)
			mlibc::infoLogger()
				<< "mlibc: Object " << object->name
				<< " defines version " << name
				<< " (index " << def.vd_ndx << ")"
				<< frg::endlog;

		if(!(def.vd_flags & VER_FLG_BASE)) {
			SymbolVersion ver{name, def.vd_hash};
			object->definedVersions.push(ver);
			object->knownVersions.insert(def.vd_ndx, ver);
		}

		address += def.vd_next;
	}
}

void ObjectRepository::_parseVerneed(SharedObject *object) {
	if(!object->versionRequirementTableOffset) {
		if(verbose)
			mlibc::infoLogger() << "mlibc: Object " << object->name << " requires no versions" << frg::endlog;
		return;
	}

	if(verbose)
		mlibc::infoLogger()
			<< "mlibc: Object " << object->name
			<< " requires " << object->versionRequirementCount
			<< " version(s)" << frg::endlog;

	uintptr_t address =
		object->baseAddress
		+ object->versionRequirementTableOffset;

	for(size_t i = 0; i < object->versionRequirementCount; i++) {
		elf_verneed need;
		memcpy(&need, reinterpret_cast<void *>(address), sizeof(elf_verneed));

		// Required by spec.
		__ensure(need.vn_version == 1);

		frg::string_view file =
			reinterpret_cast<const char *>(
				object->baseAddress
				+ object->stringTableOffset + need.vn_file);

		// Figure out the target object from file
		SharedObject *target = nullptr;
		for(auto dep : object->dependencies) {
			if(verbose)
				mlibc::infoLogger()
					<< "mlibc: Trying " << dep->name << " (SONAME: "
					<< dep->soName << ") to satisfy " << file << frg::endlog;
			if(dep->name == file || (dep->soName && dep->soName == file)) {
				target = dep;
				break;
			}
		}
		if(!target)
			mlibc::panicLogger()
				<< "mlibc: No object named \""
				<< file
				<< "\" found for VERNEED entry of object "
				<< object->name << frg::endlog;

		if(verbose)
			mlibc::infoLogger()
				<< "mlibc: Object " << object->name
				<< " requires " << need.vn_cnt
				<< " version(s) from DSO "
				<< file << frg::endlog;

		uintptr_t auxAddr = address + need.vn_aux;
		for(size_t j = 0; j < need.vn_cnt; j++) {
			elf_vernaux aux;
			memcpy(&aux, reinterpret_cast<void *>(auxAddr), sizeof(elf_vernaux));

			// TODO(qookie): Handle weak versions.
			__ensure(!aux.vna_flags);

			const char *name =
				reinterpret_cast<const char *>(
					object->baseAddress
					+ object->stringTableOffset + aux.vna_name);

			if(verbose)
				mlibc::infoLogger()
					<< "mlibc:   Object " << object->name
					<< " requires version " << name
					<< " (index " << aux.vna_other
					<< ") from DSO " << file
					<< frg::endlog;

			frg::optional<SymbolVersion> ver;
			for(auto &def : target->definedVersions) {
				if(def.hash() != aux.vna_hash) continue;
				if(def.name() == name) {
					ver = def;
					break;
				}
			}

			if(!ver)
				mlibc::panicLogger()
					<< "mlibc: Object " << target->name
					<< " does not define version \""
					<< name << "\" needed by object "
					<< object->name << frg::endlog;

			bool isDefault = !(aux.vna_other & 0x8000);
			// Bit 15 indicates whether the static linker should ignore this version.
			object->knownVersions.insert(aux.vna_other & 0x7FFF, isDefault ? ver->makeDefault() : *ver);

			auxAddr += aux.vna_next;
		}
		address += need.vn_next;
	}
}

void ObjectRepository::_discoverDependencies(SharedObject *object,
		Scope *localScope, uint64_t rts) {
	if(object->isMainObject) {
		for(auto preload : *preloads) {
			frg::expected<LinkerError, SharedObject *> libraryResult;
			if (preload.find_first('/') == size_t(-1)) {
				libraryResult = requestObjectWithName(preload, object, globalScope.get(), false, 1);
			} else {
				libraryResult = requestObjectAtPath(preload, globalScope.get(), false, 1);
			}
			if(!libraryResult)
				mlibc::panicLogger() << "rtld: Could not load preload " << preload << frg::endlog;

			if(verbose)
				mlibc::infoLogger() << "rtld: Preloading " << preload << frg::endlog;

			auto library = libraryResult.value();
			object->dependencies.push_back(library);
			if (library->wasVisited)
				continue;
			library->wasVisited = true;
			dependencyQueue.push_back(library);
		}
	}

	// Load required dynamic libraries.
	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		elf_dyn *dynamic = &object->dynamic[i];
		if(dynamic->d_tag != DT_NEEDED)
			continue;

		const char *library_str = (const char *)(object->baseAddress
				+ object->stringTableOffset + dynamic->d_un.d_val);

		auto libraryResult = requestObjectWithName(frg::string_view{library_str},
				object, localScope, false, rts);
		if(!libraryResult)
			mlibc::panicLogger() << "Could not satisfy dependency " << library_str << frg::endlog;

		auto library = libraryResult.value();
		object->dependencies.push(library);
		if (library->wasVisited)
			continue;
		library->wasVisited = true;
		dependencyQueue.push_back(library);
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
		stringTableOffset(0),
		knownVersions({}, getAllocator()), definedVersions(getAllocator()),
		lazyRelocTableOffset(0), lazyTableSize(0),
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

frg::tuple<ObjectSymbol, SymbolVersion> SharedObject::getSymbolByIndex(size_t index) {
	SymbolVersion ver{1}; // If we don't have any version information, treat all symbols as global.
	ObjectSymbol sym{
		this,
		reinterpret_cast<elf_sym *>(
			baseAddress
			+ symbolTableOffset
			+ index * sizeof(elf_sym))};

	if(versionTableOffset) {
		// Pull out the VERSYM entry for this symbol
		elf_version verIdx;
		memcpy(
			&verIdx,
			reinterpret_cast<void *>(
				baseAddress
				+ versionTableOffset
				+ index * sizeof(elf_version)),
			sizeof(elf_version)
		);

		// Bit 15 indicates that this version is not the default one.
		bool isDefault = !(verIdx & 0x8000);
		verIdx &= 0x7FFF;

		// 0 and 1 are special, 0 is local, 1 is global (not in VERDEF/VERNEED)
		if(verIdx != 0 && verIdx != 1) {
			auto maybeVer = knownVersions.find(verIdx);
			if(maybeVer == knownVersions.end())
				mlibc::panicLogger()
					<< "mlibc: Symbol " << sym.getString()
					<< " of object " << name
					<< " has invalid version index " << verIdx
					<< frg::endlog;

			ver = maybeVer->get<1>();
		} else {
			ver = SymbolVersion{verIdx};
		}

		if(isDefault)
			ver = ver.makeDefault();

		if(logSymbolVersions)
			mlibc::infoLogger()
				<< "mlibc: Symbol " << sym.getString()
				<< " of object " << name
				<< " has version " << ver.name()
				<< " and " << (ver.isDefault() ? "is" : "isn't")
				<< " the default version"
				<< frg::endlog;
	} else {
		// If we have no version information, the only symbol we've got is the default.
		ver = ver.makeDefault();
	}

	return {sym, ver};
}

void processLateRelocation(Relocation rel) {
	// resolve the symbol if there is a symbol
	frg::optional<ObjectSymbol> p;
	if(rel.symbol_index()) {
		auto [sym, ver] = rel.object()->getSymbolByIndex(rel.symbol_index());

		p = Scope::resolveGlobalOrLocal(*globalScope, rel.object()->localScope,
				sym.getString(), rel.object()->objectRts, Scope::resolveCopy, ver);
	}

	switch(rel.type()) {
	case R_COPY:
		__ensure(p);
		memcpy(rel.destination(), (void *)p->virtualAddress(), p->symbol()->st_size);
		break;

// TODO: R_IRELATIVE also exists on other architectures but will likely need a different implementation.
#if defined(__x86_64__) || defined(__i386__)
	case R_IRELATIVE: {
		uintptr_t addr = rel.object()->baseAddress + rel.addend_rel();
		auto* fn = reinterpret_cast<uintptr_t (*)()>(addr);
		rel.relocate(fn());
	} break;
#elif defined(__aarch64__)
	case R_IRELATIVE: {
		uintptr_t addr = rel.object()->baseAddress + rel.addend_rel();
		auto* fn = reinterpret_cast<uintptr_t (*)(uint64_t)>(addr);
		// TODO: the function should get passed AT_HWCAP value.
		rel.relocate(fn(0));
	} break;
#endif

	default:
		break;
	}
}

void processLateRelocations(SharedObject *object) {
	frg::optional<uintptr_t> rel_offset;
	frg::optional<size_t> rel_length;

	frg::optional<uintptr_t> rela_offset;
	frg::optional<size_t> rela_length;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		elf_dyn *dynamic = &object->dynamic[i];

		switch(dynamic->d_tag) {
		case DT_REL:
			rel_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELSZ:
			rel_length = dynamic->d_un.d_val;
			break;
		case DT_RELENT:
			__ensure(dynamic->d_un.d_val == sizeof(elf_rel));
			break;
		case DT_RELA:
			rela_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_un.d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_un.d_val == sizeof(elf_rela));
			break;
		}
	}

	if(rela_offset && rela_length) {
		for(size_t offset = 0; offset < *rela_length; offset += sizeof(elf_rela)) {
			auto reloc = (elf_rela *)(object->baseAddress + *rela_offset + offset);
			auto r = Relocation(object, reloc);
			processLateRelocation(r);
		}
	} else if(rel_offset && rel_length) {
		for(size_t offset = 0; offset < *rel_length; offset += sizeof(elf_rel)) {
			auto reloc = (elf_rel *)(object->baseAddress + *rel_offset + offset);
			auto r = Relocation(object, reloc);
			processLateRelocation(r);
		}
	}else{
		__ensure(!rela_offset && !rela_length);
		__ensure(!rel_offset && !rel_length);
	}
}

void doInitialize(SharedObject *object) {
	__ensure(object->wasLinked);
	__ensure(!object->wasInitialized);

	if(verbose)
		mlibc::infoLogger() << "rtld: Initialize " << object->name << frg::endlog;

	if(verbose)
		mlibc::infoLogger() << "rtld: Running DT_INIT function" << frg::endlog;
	if(object->initPtr != nullptr)
		object->initPtr();

	if(verbose)
		mlibc::infoLogger() << "rtld: Running DT_INIT_ARRAY functions" << frg::endlog;
	__ensure((object->initArraySize % sizeof(InitFuncPtr)) == 0);
	for(size_t i = 0; i < object->initArraySize / sizeof(InitFuncPtr); i++)
		object->initArray[i]();

	if(verbose)
		mlibc::infoLogger() << "rtld: Object initialization complete" << frg::endlog;
	object->wasInitialized = true;
}

void doDestruct(SharedObject *object) {
	if(!object->wasInitialized || object->wasDestroyed)
		return;

	if(verbose)
		mlibc::infoLogger() << "rtld: Destruct " << object->name << frg::endlog;

	if(verbose)
		mlibc::infoLogger() << "rtld: Running DT_FINI_ARRAY functions" << frg::endlog;
	__ensure((object->finiArraySize % sizeof(InitFuncPtr)) == 0);
	for(size_t i = object->finiArraySize / sizeof(InitFuncPtr); i > 0; i--)
		object->finiArray[i - 1]();

	if(verbose)
		mlibc::infoLogger() << "rtld: Running DT_FINI function" << frg::endlog;
	if(object->finiPtr != nullptr)
		object->finiPtr();

	if(verbose)
		mlibc::infoLogger() << "rtld: Object destruction complete" << frg::endlog;
	object->wasDestroyed = true;
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

			if constexpr (tlsAboveTp) {
				tls_ptr += sizeof(Tcb);
			}

			memset(tls_ptr, 0, object->tlsSegmentSize);
			memcpy(tls_ptr, object->tlsImagePtr, object->tlsImageSize);

			if (verbose) {
				mlibc::infoLogger() << "rtld: wrote tls image at " << (void *)tls_ptr
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
	size_t alignOverhead = frg::max(alignof(Tcb), tlsMaxAlignment);
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
		mlibc::infoLogger() << "rtld: tcb allocated at " << (void *)tcbAddress
				<< ", size = 0x" << frg::hex_fmt{sizeof(Tcb)} << frg::endlog;
		mlibc::infoLogger() << "rtld: tls allocated at " << (void *)tlsAddress
				<< ", size = 0x" << frg::hex_fmt{tlsInitialSize} << frg::endlog;
	}

	Tcb *tcb_ptr = new ((char *)tcbAddress) Tcb;
	tcb_ptr->selfPointer = tcb_ptr;

	tcb_ptr->stackCanary = __stack_chk_guard;
	tcb_ptr->cancelBits = tcbCancelEnableBit;
	tcb_ptr->didExit = 0;
	tcb_ptr->isJoinable = 1;
	memset(&tcb_ptr->returnValue, 0, sizeof(tcb_ptr->returnValue));
	tcb_ptr->localKeys = frg::construct<frg::array<Tcb::LocalKey, PTHREAD_KEYS_MAX>>(getAllocator());
	tcb_ptr->dtvSize = runtimeTlsMap->indices.size();
	tcb_ptr->dtvPointers = frg::construct_n<void *>(getAllocator(), runtimeTlsMap->indices.size());
	memset(tcb_ptr->dtvPointers, 0, sizeof(void *) * runtimeTlsMap->indices.size());
	for(size_t i = 0; i < runtimeTlsMap->indices.size(); ++i) {
		auto object = runtimeTlsMap->indices[i];
		if(object->tlsModel != TlsModel::initial)
			continue;

		if constexpr (tlsAboveTp) {
			tcb_ptr->dtvPointers[i] = reinterpret_cast<char *>(tcb_ptr) + sizeof(Tcb) + object->tlsOffset;
		} else {
			tcb_ptr->dtvPointers[i] = reinterpret_cast<char *>(tcb_ptr) + object->tlsOffset;
		}
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
			mlibc::infoLogger() << "rtld: accessDtv wrote tls image at " << buffer
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

ObjectSymbol::ObjectSymbol(SharedObject *object, const elf_sym *symbol)
: _object(object), _symbol(symbol) { }

const char *ObjectSymbol::getString() {
	__ensure(_symbol->st_name != 0);
	return (const char *)(_object->baseAddress
			+ _object->stringTableOffset + _symbol->st_name);
}

uintptr_t ObjectSymbol::virtualAddress() {
	auto bind = ELF_ST_BIND(_symbol->st_info);
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
frg::optional<ObjectSymbol> resolveInObject(SharedObject *object, frg::string_view string,
		frg::optional<SymbolVersion> version) {
	// Checks if the symbol can be used to satisfy the dependency.
	auto eligible = [&] (ObjectSymbol cand) {
		if(cand.symbol()->st_shndx == SHN_UNDEF)
			return false;

		auto bind = ELF_ST_BIND(cand.symbol()->st_info);
		if(bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_GNU_UNIQUE)
			return false;

		return true;
	};

	// Checks if the symbol's version matches the desired version.
	auto correctVersion = [&] (SymbolVersion candVersion) {
		// TODO(qookie): Not sure if local symbols should participate in dynamic symbol resolution
		if(!version && (candVersion.isDefault() || candVersion.isLocal() || candVersion.isGlobal()))
			return true;
		// Caller requested default version, but this isn't it.
		if(!version)
			return false;
		// If the requested version is global (caller has VERNEED but not for this symbol),
		// use the default one.
		if(version->isGlobal() && !candVersion.isGlobal() && !candVersion.isLocal() && candVersion.isDefault())
			return true;
		return *version == candVersion;
	};

	if (object->hashStyle == HashStyle::systemV) {
		auto hash_table = (Elf64_Word *)(object->baseAddress + object->hashTableOffset);
		Elf64_Word num_buckets = hash_table[0];
		auto bucket = elf64Hash(string) % num_buckets;

		auto index = hash_table[2 + bucket];
		while(index != 0) {
			auto [cand, ver] = object->getSymbolByIndex(index);
			if(eligible(cand) && frg::string_view{cand.getString()} == string && correctVersion(ver))
				return cand;

			index = hash_table[2 + num_buckets + index];
		}

		return frg::optional<ObjectSymbol>{};
	}else{
		__ensure(object->hashStyle == HashStyle::gnu);

		struct GnuTable {
			uint32_t nBuckets;
			uint32_t symbolOffset;
			uint32_t bloomSize;
			uint32_t bloomShift;
		};

		auto hash_table = reinterpret_cast<const GnuTable *>(object->baseAddress
				+ object->hashTableOffset);
		auto buckets = reinterpret_cast<const uint32_t *>(object->baseAddress
				+ object->hashTableOffset + sizeof(GnuTable)
				+ hash_table->bloomSize * sizeof(elf_addr));
		auto chains = reinterpret_cast<const uint32_t *>(object->baseAddress
				+ object->hashTableOffset + sizeof(GnuTable)
				+ hash_table->bloomSize * sizeof(elf_addr)
				+ hash_table->nBuckets * sizeof(uint32_t));

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
				auto [cand, ver] = object->getSymbolByIndex(index);
				if(eligible(cand) && frg::string_view{cand.getString()} == string && correctVersion(ver))
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
		SharedObject *target, frg::optional<SymbolVersion> version) {
	// Skip objects until we find the target, and only look for symbols after that.
	size_t i;
	for (i = 0; i < _objects.size(); i++) {
		if (_objects[i] == target)
			break;
	}

	if (i == _objects.size()) {
		mlibc::infoLogger() << "rtld: object passed to Scope::resolveAfter was not found" << frg::endlog;
		return frg::optional<ObjectSymbol>();
	}

	for (i = i + 1; i < _objects.size(); i++) {
		if(_objects[i]->isMainObject)
			continue;

		frg::optional<ObjectSymbol> p = resolveInObject(_objects[i], string, version);
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
		Scope *localScope, frg::string_view string, uint64_t skipRts, ResolveFlags flags,
		frg::optional<SymbolVersion> version) {
	auto sym = globalScope.resolveSymbol(string, skipRts, flags | skipGlobalAfterRts, version);
	if(!sym && localScope)
		sym = localScope->resolveSymbol(string, skipRts, flags | skipGlobalAfterRts, version);
	return sym;
}

frg::optional<ObjectSymbol> Scope::resolveGlobalOrLocalNext(Scope &globalScope,
		Scope *localScope, frg::string_view string, SharedObject *origin,
		frg::optional<SymbolVersion> version) {
	auto sym = globalScope._resolveNext(string, origin, version);
	if(!sym && localScope) {
		sym = localScope->_resolveNext(string, origin, version);
	}
	return sym;
}

// TODO: let this return uintptr_t
frg::optional<ObjectSymbol> Scope::resolveSymbol(frg::string_view string,
		uint64_t skipRts, ResolveFlags flags,
		frg::optional<SymbolVersion> version) {
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

		frg::optional<ObjectSymbol> p = resolveInObject(object, string, version);
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
			mlibc::infoLogger() << "rtld: Linking " << object->name << frg::endlog;

		__ensure(!object->wasLinked);

		// TODO: Support this.
		if(object->symbolicResolution)
			mlibc::infoLogger() << "\e[31mrtld: DT_SYMBOLIC is not implemented correctly!\e[39m"
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

		processLateRelocations(object);
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
				size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
				if(misalign)
					runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

				object->tlsOffset = runtimeTlsMap->initialPtr;
				runtimeTlsMap->initialPtr += object->tlsSegmentSize;
			} else {
				runtimeTlsMap->initialPtr += object->tlsSegmentSize;

				size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
				if(misalign)
					runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

				object->tlsOffset = -runtimeTlsMap->initialPtr;
			}

			if(verbose)
				mlibc::infoLogger() << "rtld: TLS of " << object->name
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
				object->tlsModel = TlsModel::initial;

				if constexpr (tlsAboveTp) {
					size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
					if(misalign)
						runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

					object->tlsOffset = runtimeTlsMap->initialPtr;
					runtimeTlsMap->initialPtr += object->tlsSegmentSize;
				} else {
					runtimeTlsMap->initialPtr += object->tlsSegmentSize;

					size_t misalign = runtimeTlsMap->initialPtr & (object->tlsAlignment - 1);
					if(misalign)
						runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

					object->tlsOffset = -runtimeTlsMap->initialPtr;
				}

				if(runtimeTlsMap->initialPtr > runtimeTlsMap->initialLimit)
						mlibc::panicLogger() << "rtld: Static TLS space exhausted while while"
								" allocating TLS for " << object->name << frg::endlog;

				if(verbose)
					mlibc::infoLogger() << "rtld: TLS of " << object->name
							<< " mapped to 0x" << frg::hex_fmt{object->tlsOffset}
							<< ", size: " << object->tlsSegmentSize
							<< ", alignment: " << object->tlsAlignment << frg::endlog;
			}else{
				object->tlsModel = TlsModel::dynamic;
			}
		}
	}
}

void Loader::initObjects(ObjectRepository *repository) {
	initTlsObjects(mlibc::get_current_tcb(), _linkBfs, true);

	if (_mainExecutable && _mainExecutable->preInitArray) {
		if (verbose)
			mlibc::infoLogger() << "rtld: Running DT_PREINIT_ARRAY functions" << frg::endlog;

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
		if(!object->wasInitialized) {
			doInitialize(object);
			repository->addObjectToDestructQueue(object);
		}
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

void Loader::_processRelocations(Relocation &rel) {
	// copy and irelative relocations have to be performed after all other relocations
	if(rel.type() == R_COPY || rel.type() == R_IRELATIVE)
		return;

	// resolve the symbol if there is a symbol
	frg::optional<ObjectSymbol> p;
	if(rel.symbol_index()) {
		auto [sym, ver] = rel.object()->getSymbolByIndex(rel.symbol_index());

		p = Scope::resolveGlobalOrLocal(*globalScope, rel.object()->localScope,
				sym.getString(), rel.object()->objectRts, 0, ver);
		if(!p) {
			if(ELF_ST_BIND(sym.symbol()->st_info) != STB_WEAK)
				mlibc::panicLogger() << "Unresolved load-time symbol "
						<< sym.getString() << " in object " << rel.object()->name << frg::endlog;

			if(verbose)
				mlibc::infoLogger() << "rtld: Unresolved weak load-time symbol "
						<< sym.getString() << " in object " << rel.object()->name << frg::endlog;
		}
	}

	switch(rel.type()) {
	case R_NONE:
		break;

	case R_JUMP_SLOT: {
		__ensure(!rel.addend_norel());
		uintptr_t symbol_addr = p ? p->virtualAddress() : 0;
		rel.relocate(symbol_addr);
	} break;

#if !defined(__riscv) && !defined(__loongarch64)
	// on some architectures, R_GLOB_DAT can be defined to other relocations
	case R_GLOB_DAT: {
		__ensure(rel.symbol_index());
		uintptr_t symbol_addr = p ? p->virtualAddress() : 0;
		rel.relocate(symbol_addr + rel.addend_norel());
	} break;
#endif

	case R_ABSOLUTE: {
		__ensure(rel.symbol_index());
		uintptr_t symbol_addr = p ? p->virtualAddress() : 0;
		rel.relocate(symbol_addr + rel.addend_rel());
	} break;

	case R_RELATIVE: {
		__ensure(!rel.symbol_index());
		rel.relocate(rel.object()->baseAddress + rel.addend_rel());
	} break;

	// DTPMOD and DTPREL are dynamic TLS relocations (for __tls_get_addr()).
	// TPOFF is a relocation to the initial TLS model.
	case R_TLS_DTPMOD: {
		// sets the first `sizeof(uintptr_t)` bytes of `struct __abi_tls_entry`
		// this means that we can just use the `SharedObject *` to resolve whatever we need
		__ensure(!rel.addend_rel());
		if(rel.symbol_index()) {
			__ensure(p);
			rel.relocate(elf_addr(p->object()));
		}else{
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtld: Warning: TLS_DTPMOD64 with no symbol in object "
					<< rel.object()->name << frg::endlog;
			rel.relocate(elf_addr(rel.object()));
		}
	} break;
	case R_TLS_DTPREL: {
		__ensure(rel.symbol_index());
		__ensure(p);
		rel.relocate(p->symbol()->st_value + rel.addend_rel() - TLS_DTV_OFFSET);
	} break;
	case R_TLS_TPREL: {
		uintptr_t off = rel.addend_rel();
		ssize_t tls_offset = 0;

		if(rel.symbol_index()) {
			__ensure(p);
			if(p->object()->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtld: In object " << rel.object()->name
						<< ": Static TLS relocation to symbol " << p->getString()
						<< " in dynamically loaded object "
						<< p->object()->name << frg::endlog;
			off += p->symbol()->st_value;
			tls_offset = p->object()->tlsOffset;
		}else{
			if(stillSlightlyVerbose)
				mlibc::infoLogger() << "rtld: Warning: TPOFF64 with no symbol"
						" in object " << rel.object()->name << frg::endlog;
			if(rel.object()->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtld: In object " << rel.object()->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< rel.object()->name << frg::endlog;
			tls_offset = rel.object()->tlsOffset;
		}

		off += tls_offset + tlsOffsetFromTp;
		rel.relocate(off);
	} break;
	default:
		mlibc::panicLogger() << "Unexpected relocation type "
				<< (void *) rel.type() << frg::endlog;
	}
}

void Loader::_processStaticRelocations(SharedObject *object) {
	frg::optional<uintptr_t> rela_offset;
	frg::optional<size_t> rela_length;

	frg::optional<uintptr_t> rel_offset;
	frg::optional<size_t> rel_length;

	frg::optional<uintptr_t> relr_offset;
	frg::optional<size_t> relr_length;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		elf_dyn *dynamic = &object->dynamic[i];

		switch(dynamic->d_tag) {
		case DT_RELA:
			rela_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_un.d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_un.d_val == sizeof(elf_rela));
			break;
		case DT_REL:
			rel_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELSZ:
			rel_length = dynamic->d_un.d_val;
			break;
		case DT_RELENT:
			__ensure(dynamic->d_un.d_val == sizeof(elf_rel));
			break;
		case DT_RELR:
			relr_offset = dynamic->d_un.d_ptr;
			break;
		case DT_RELRSZ:
			relr_length = dynamic->d_un.d_val;
			break;
		case DT_RELRENT:
			__ensure(dynamic->d_un.d_val == sizeof(elf_relr));
			break;
		}
	}

	if(rela_offset && rela_length) {
		__ensure(!rel_offset && !rel_length);

		for(size_t offset = 0; offset < *rela_length; offset += sizeof(elf_rela)) {
			auto reloc = (elf_rela *)(object->baseAddress + *rela_offset + offset);
			auto r = Relocation(object, reloc);

			_processRelocations(r);
		}
	}else if(rel_offset && rel_length) {
		__ensure(!rela_offset && !rela_length);

		for(size_t offset = 0; offset < *rel_length; offset += sizeof(elf_rel)) {
			auto reloc = (elf_rel *)(object->baseAddress + *rel_offset + offset);
			auto r = Relocation(object, reloc);

			_processRelocations(r);
		}
	}

	if(relr_offset && relr_length) {
		elf_addr *addr = nullptr;

		for(size_t offset = 0; offset < *relr_length; offset += sizeof(elf_relr)) {
			auto entry = *(elf_relr *)(object->baseAddress + *relr_offset + offset);

			// Even entry indicates the beginning address.
			if(!(entry & 1)) {
				addr = (elf_addr *)(object->baseAddress + entry);
				__ensure(addr);
				*addr++ += object->baseAddress;
			}else {
				// Odd entry indicates entry is a bitmap of the subsequent locations to be relocated.

				// The first bit of an entry is always a marker about whether the entry is an address or a bitmap,
				// discard it.
				entry >>= 1;

				for(int i = 0; entry; ++i) {
					if(entry & 1) {
						addr[i] += object->baseAddress;
					}
					entry >>= 1;
				}

				// Each entry describes at max 63 (on 64bit) or 31 (on 32bit) subsequent locations.
				addr += CHAR_BIT * sizeof(elf_relr) - 1;
			}
		}
	}
}

// TODO: TLSDESC relocations aren't aarch64/x86_64 specific
#if defined(__aarch64__) || defined(__x86_64__)
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
	__ensure(object->lazyExplicitAddend.has_value());
	size_t rel_size = (*object->lazyExplicitAddend) ? sizeof(elf_rela) : sizeof(elf_rel);

	for(size_t offset = 0; offset < object->lazyTableSize; offset += rel_size) {
		elf_info type;
		elf_info symbol_index;

		uintptr_t rel_addr;
		uintptr_t addend [[maybe_unused]] = 0;

		if(*object->lazyExplicitAddend) {
			auto reloc = (elf_rela *)(object->baseAddress + object->lazyRelocTableOffset + offset);
			type = ELF_R_TYPE(reloc->r_info);
			symbol_index = ELF_R_SYM(reloc->r_info);
			rel_addr = object->baseAddress + reloc->r_offset;
			addend = reloc->r_addend;
		} else {
			auto reloc = (elf_rel *)(object->baseAddress + object->lazyRelocTableOffset + offset);
			type = ELF_R_TYPE(reloc->r_info);
			symbol_index = ELF_R_SYM(reloc->r_info);
			rel_addr = object->baseAddress + reloc->r_offset;
		}

		switch (type) {
		case R_JUMP_SLOT:
			if(eagerBinding) {
				auto [sym, ver] = object->getSymbolByIndex(symbol_index);
				auto p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, sym.getString(), object->objectRts, 0, ver);

				if(!p) {
					if(ELF_ST_BIND(sym.symbol()->st_info) != STB_WEAK)
						mlibc::panicLogger() << "rtld: Unresolved JUMP_SLOT symbol "
								<< sym.getString() << " in object " << object->name << frg::endlog;

					if(verbose)
						mlibc::infoLogger() << "rtld: Unresolved weak JUMP_SLOT symbol "
							<< sym.getString() << " in object " << object->name << frg::endlog;
					*((uintptr_t *)rel_addr) = 0;
				}else{
					*((uintptr_t *)rel_addr) = p->virtualAddress();
				}
			}else{
				*((uintptr_t *)rel_addr) += object->baseAddress;
			}
			break;
#if defined(__x86_64__)
		case R_X86_64_IRELATIVE: {
			auto ptr = object->baseAddress + addend;
			auto target = reinterpret_cast<uintptr_t (*)(void)>(ptr)();
			*((uintptr_t *)rel_addr) = target;
			break;
		}
#endif
// TODO: TLSDESC relocations aren't aarch64/x86_64 specific
#if defined(__aarch64__) || defined(__x86_64__)
		case R_TLSDESC: {
			size_t symValue = 0;
			SharedObject *target = nullptr;

			if (symbol_index) {
				auto [sym, ver] = object->getSymbolByIndex(symbol_index);
				auto p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, sym.getString(), object->objectRts, 0, ver);

				if (!p) {
					__ensure(ELF_ST_BIND(sym.symbol()->st_info) != STB_WEAK);
					mlibc::panicLogger() << "rtld: Unresolved TLSDESC for symbol "
						<< sym.getString() << " in object " << object->name << frg::endlog;
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
				uint64_t value = symValue + target->tlsOffset + tlsOffsetFromTp + addend;
				((uint64_t *)rel_addr)[1] = value;
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
				data->addend = symValue + addend;

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

