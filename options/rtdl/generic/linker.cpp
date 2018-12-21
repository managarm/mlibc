
#include <elf.h>
#include <string.h>

#include <frg/manual_box.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>
#include <internal-config.h>
#include "linker.hpp"

uintptr_t libraryBase = 0x41000000;

bool verbose = false;
bool stillSlightlyVerbose = false;
bool logBaseAddresses = false;
bool eagerBinding = true;

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

// --------------------------------------------------------
// ObjectRepository
// --------------------------------------------------------

ObjectRepository::ObjectRepository()
: _nameMap{frg::hash<frg::string_view>{}, getAllocator()} { }

SharedObject *ObjectRepository::injectObjectFromDts(frg::string_view name,
		uintptr_t base_address, Elf64_Dyn *dynamic, uint64_t rts) {
	__ensure(!_nameMap.get(name));

	auto object = frg::construct<SharedObject>(getAllocator(), name.data(), false, rts);
	object->baseAddress = base_address;
	object->dynamic = dynamic;
	_parseDynamic(object);

	_nameMap.insert(name, object);
	_discoverDependencies(object, rts);

	return object;
}

SharedObject *ObjectRepository::injectObjectFromPhdrs(frg::string_view name,
		void *phdr_pointer, size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
		uint64_t rts) {
	__ensure(!_nameMap.get(name));

	auto object = frg::construct<SharedObject>(getAllocator(), name.data(), true, rts);
	_fetchFromPhdrs(object, phdr_pointer, phdr_entry_size, num_phdrs, entry_pointer);
	_parseDynamic(object);

	_nameMap.insert(name, object);
	_discoverDependencies(object, rts);

	return object;
}

SharedObject *ObjectRepository::requestObjectWithName(frg::string_view name, uint64_t rts) {
	auto it = _nameMap.get(name);
	if(it)
		return *it;

	auto object = frg::construct<SharedObject>(getAllocator(), name.data(), false, rts);

	frg::string<MemoryAllocator> lib_prefix(getAllocator(), "/lib/");
	frg::string<MemoryAllocator> usr_prefix(getAllocator(), "/usr/lib/");

	// open the object file
	auto tryToOpen = [&] (const char *path) {
		int fd;
		if(mlibc::sys_open(path, 0, &fd))
			return -1;
		return fd;
	};

	auto fd = tryToOpen((lib_prefix + name + '\0').data());
	if(fd == -1)
		fd = tryToOpen((usr_prefix + name + '\0').data());
	if(fd == -1)
		return nullptr; // TODO: Free the SharedObject.
	_fetchFromFile(object, fd);
	closeOrDie(fd);

	_parseDynamic(object);

	_nameMap.insert(name, object);
	_discoverDependencies(object, rts);

	return object;
}

SharedObject *ObjectRepository::requestObjectAtPath(frg::string_view path, uint64_t rts) {
	// TODO: Support SONAME correctly.
	auto it = _nameMap.get(path);
	if(it)
		return *it;

	auto object = frg::construct<SharedObject>(getAllocator(), path.data(), false, rts);
	
	frg::string<MemoryAllocator> no_prefix(getAllocator(), path);
	
	int fd;
	if(mlibc::sys_open((no_prefix + '\0').data(), 0, &fd))
		return nullptr; // TODO: Free the SharedObject.
	_fetchFromFile(object, fd);
	closeOrDie(fd);

	_parseDynamic(object);

	_nameMap.insert(path, object);
	_discoverDependencies(object, rts);

	return object;
}

// --------------------------------------------------------
// ObjectRepository: Fetching methods.
// --------------------------------------------------------

void ObjectRepository::_fetchFromPhdrs(SharedObject *object, void *phdr_pointer,
		size_t phdr_entry_size, size_t phdr_count, void *entry_pointer) {
	__ensure(object->isMainObject);
	if(verbose)
		mlibc::infoLogger() << "rtdl: Loading " << object->name << frg::endlog;
	
	object->entry = entry_pointer;

	// segments are already mapped, so we just have to find the dynamic section
	for(size_t i = 0; i < phdr_count; i++) {
		auto phdr = (Elf64_Phdr *)((uintptr_t)phdr_pointer + i * phdr_entry_size);
		switch(phdr->p_type) {
		case PT_DYNAMIC:
			object->dynamic = (Elf64_Dyn *)(object->baseAddress + phdr->p_vaddr);
			break;
		case PT_TLS: {
			object->tlsSegmentSize = phdr->p_memsz;
			object->tlsAlignment = phdr->p_align;
			object->tlsImageSize = phdr->p_filesz;
			object->tlsImagePtr = (void *)(object->baseAddress + phdr->p_vaddr);
		} break;
		default:
			//FIXME warn about unknown phdrs
			break;
		}
	}
}


void ObjectRepository::_fetchFromFile(SharedObject *object, int fd) {
	__ensure(!object->isMainObject);

	object->baseAddress = libraryBase;
	// TODO: handle this dynamically
	libraryBase += 0x1000000; // assume 16 MiB per library

	if(verbose || logBaseAddresses)
		mlibc::infoLogger() << "rtdl: Loading " << object->name
				<< " at " << (void *)object->baseAddress << frg::endlog;

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

	constexpr size_t kPageSize = 0x1000;
	__ensure(!(object->baseAddress & (kPageSize - 1)));
	
	for(int i = 0; i < ehdr.e_phnum; i++) {
		auto phdr = (Elf64_Phdr *)(phdr_buffer + i * ehdr.e_phentsize);
		
		if(phdr->p_type == PT_LOAD) {
			size_t misalign = phdr->p_vaddr & (kPageSize - 1);
			__ensure(phdr->p_memsz > 0);
			__ensure(phdr->p_memsz >= phdr->p_filesz);

			// If the following condition is violated, we cannot use mmap() the segment;
			// however, GCC only generates ELF files that satisfy this.
			__ensure(misalign == (phdr->p_offset & (kPageSize - 1)));

			auto map_address = object->baseAddress + phdr->p_vaddr - misalign;
			auto backed_map_size = (phdr->p_filesz + misalign + kPageSize - 1) & ~(kPageSize - 1);
			auto total_map_size = (phdr->p_memsz + misalign + kPageSize - 1) & ~(kPageSize - 1);

			int prot = 0;
			if(phdr->p_flags & PF_R)
				prot |= PROT_READ;
			if(phdr->p_flags & PF_W)
				prot |= PROT_WRITE;
			if(phdr->p_flags & PF_X)
				prot |= PROT_EXEC;

			#ifdef MLIBC_MAP_DSO_SEGMENTS
				// TODO: Map with (prot | PROT_WRITE) here,
				// then mprotect() to remove PROT_WRITE if that is necessary.
				void *map_pointer;
				if(mlibc::sys_vm_map(reinterpret_cast<void *>(map_address),
						backed_map_size, prot,
						MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset - misalign, &map_pointer))
					__ensure(!"sys_vm_map failed");
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
				|| phdr->p_type == PT_GNU_EH_FRAME
				|| phdr->p_type == PT_GNU_RELRO
				|| phdr->p_type == PT_GNU_STACK) {
			// ignore the phdr
		}else{
			__ensure(!"Unexpected PHDR");
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

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];
		switch(dynamic->d_tag) {
		// handle hash table, symbol table and string table
		case DT_HASH:
			object->hashTableOffset = dynamic->d_ptr;
			break;
		case DT_STRTAB:
			object->stringTableOffset = dynamic->d_ptr;
			break;
		case DT_STRSZ:
			break; // we don't need the size of the string table
		case DT_SYMTAB:
			object->symbolTableOffset = dynamic->d_ptr;
			break;
		case DT_SYMENT:
			__ensure(dynamic->d_val == sizeof(Elf64_Sym));
			break;
		// handle lazy relocation table
		case DT_PLTGOT:
			object->globalOffsetTable = (void **)(object->baseAddress
					+ dynamic->d_ptr);
			break;
		case DT_JMPREL:
			object->lazyRelocTableOffset = dynamic->d_ptr;
			break;
		case DT_PLTRELSZ:
			object->lazyTableSize = dynamic->d_val;
			break;
		case DT_PLTREL:
			if(dynamic->d_val == DT_RELA) {
				object->lazyExplicitAddend = true;
			}else{
				__ensure(dynamic->d_val == DT_REL);
			}
			break;
		// TODO: Implement this correctly!
		case DT_SYMBOLIC:
			object->symbolicResolution = true;
			break;
		case DT_BIND_NOW:
			object->eagerBinding = true;
			break;
		case DT_FLAGS:
			if(dynamic->d_val & DF_SYMBOLIC)
				object->symbolicResolution = true;
			if(dynamic->d_val & DF_STATIC_TLS)
				object->haveStaticTls = true;
			if(dynamic->d_val & ~(DF_SYMBOLIC | DF_STATIC_TLS))
				mlibc::infoLogger() << "\e[31mrtdl: DT_FLAGS(" << frg::hex_fmt{dynamic->d_val}
						<< ") is not implemented correctly!\e[39m"
						<< frg::endlog;
			break;
		case DT_FLAGS_1:
			if(dynamic->d_val & DF_1_NOW)
				object->eagerBinding = true;
			if(dynamic->d_val & ~(DF_1_NOW))
				mlibc::infoLogger() << "\e[31mrtdl: DT_FLAGS_1(" << frg::hex_fmt{dynamic->d_val}
						<< ") is not implemented correctly!\e[39m"
						<< frg::endlog;
			break;
		// ignore unimportant tags
		case DT_SONAME: case DT_NEEDED: case DT_RPATH: // we handle this later
		case DT_INIT: case DT_FINI:
		case DT_INIT_ARRAY: case DT_INIT_ARRAYSZ:
		case DT_FINI_ARRAY: case DT_FINI_ARRAYSZ:
		case DT_DEBUG:
		case DT_RELA: case DT_RELASZ: case DT_RELAENT: case DT_RELACOUNT:
		case DT_VERSYM:
		case DT_VERDEF: case DT_VERDEFNUM:
		case DT_VERNEED: case DT_VERNEEDNUM:
			break;
		default:
			mlibc::panicLogger() << "Unexpected dynamic entry "
					<< (void *)dynamic->d_tag << " in object" << frg::endlog;
		}
	}
}

void ObjectRepository::_discoverDependencies(SharedObject *object, uint64_t rts) {
	// Load required dynamic libraries.
	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];
		if(dynamic->d_tag != DT_NEEDED)
			continue;

		const char *library_str = (const char *)(object->baseAddress
				+ object->stringTableOffset + dynamic->d_val);

		auto library = requestObjectWithName(frg::string_view{library_str}, rts);
		if(!library)
			mlibc::panicLogger() << "Could not satisfy dependency " << library_str << frg::endlog;
		object->dependencies.push(library);
	}
}

// --------------------------------------------------------
// SharedObject
// --------------------------------------------------------

SharedObject::SharedObject(const char *name, bool is_main_object, uint64_t object_rts)
		: name(name), isMainObject(is_main_object), objectRts(object_rts),
		baseAddress(0), loadScope(nullptr),
		dynamic(nullptr), globalOffsetTable(nullptr), entry(nullptr),
		tlsSegmentSize(0), tlsAlignment(0), tlsImageSize(0), tlsImagePtr(nullptr),
		tlsInitialized(false),
		hashTableOffset(0), symbolTableOffset(0), stringTableOffset(0),
		lazyRelocTableOffset(0), lazyTableSize(0), lazyExplicitAddend(false),
		symbolicResolution(false), eagerBinding(false), haveStaticTls(false),
		dependencies(getAllocator()),
		tlsModel(TlsModel::null), tlsOffset(0),
		globalRts(0), wasLinked(false),
		scheduledForInit(false), onInitStack(false), wasInitialized(false),
		objectScope(nullptr) { }

void processCopyRela(SharedObject *object, Elf64_Rela *reloc) {
	Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
	Elf64_Xword symbol_index = ELF64_R_SYM(reloc->r_info);
	if(type != R_X86_64_COPY)
		return;
	
	uintptr_t rel_addr = object->baseAddress + reloc->r_offset;
	
	auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
			+ symbol_index * sizeof(Elf64_Sym));
	ObjectSymbol r(object, symbol);
	frg::optional<ObjectSymbol> p = object->loadScope->resolveSymbol(r, Scope::resolveCopy);
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
			rela_offset = dynamic->d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_val == sizeof(Elf64_Rela));
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
	
	// now initialize the actual object
	typedef void (*InitFuncPtr) ();

	InitFuncPtr init_ptr = nullptr;
	InitFuncPtr *init_array = nullptr;
	size_t array_size = 0;

	for(size_t i = 0; object->dynamic[i].d_tag != DT_NULL; i++) {
		Elf64_Dyn *dynamic = &object->dynamic[i];
		
		switch(dynamic->d_tag) {
		case DT_INIT:
			if(dynamic->d_ptr != 0)
				init_ptr = (InitFuncPtr)(object->baseAddress + dynamic->d_ptr);
			break;
		case DT_INIT_ARRAY:
			if(dynamic->d_ptr != 0)
				init_array = (InitFuncPtr *)(object->baseAddress + dynamic->d_ptr);
			break;
		case DT_INIT_ARRAYSZ:
			array_size = dynamic->d_val;
			break;
		}
	}

	if(verbose)
		mlibc::infoLogger() << "rtdl: Running DT_INIT function" << frg::endlog;
	if(init_ptr != nullptr)
		init_ptr();
	
	if(verbose)
		mlibc::infoLogger() << "rtdl: Running DT_INIT_ARRAY functions" << frg::endlog;
	__ensure((array_size % sizeof(InitFuncPtr)) == 0);
	for(size_t i = 0; i < array_size / sizeof(InitFuncPtr); i++)
		init_array[i]();

	if(verbose)
		mlibc::infoLogger() << "rtdl: Object initialization complete" << frg::endlog;
	object->wasInitialized = true;
}

// --------------------------------------------------------
// RuntimeTlsMap
// --------------------------------------------------------

RuntimeTlsMap::RuntimeTlsMap()
: initialPtr(0), initialLimit(0) { }

struct Tcb {
	Tcb *selfPointer;
};

void allocateTcb() {
	size_t fs_size = runtimeTlsMap->initialLimit + sizeof(Tcb);
	char *fs_buffer = (char *)getAllocator().allocate(fs_size);
	memset(fs_buffer, 0, fs_size);

	auto tcb_ptr = (Tcb *)(fs_buffer + runtimeTlsMap->initialLimit);
	tcb_ptr->selfPointer = tcb_ptr;
	if(mlibc::sys_tcb_set(tcb_ptr))
		__ensure(!"sys_tcb_set() failed");
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
	__ensure(bind == STB_GLOBAL || bind == STB_WEAK);
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

// TODO: move this to some namespace or class?
frg::optional<ObjectSymbol> resolveInObject(SharedObject *object, frg::string_view string) {
	// Checks if the symbol can be used to satisfy the dependency.
	auto eligible = [&] (ObjectSymbol cand) {
		if(cand.symbol()->st_shndx == SHN_UNDEF)
			return false;

		auto bind = ELF64_ST_BIND(cand.symbol()->st_info);
		if(bind != STB_GLOBAL && bind != STB_WEAK)
			return false;

		return true;
	};

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

	return frg::optional<ObjectSymbol>();
}


frg::optional<ObjectSymbol> Scope::resolveWholeScope(Scope *scope,
		frg::string_view string, ResolveFlags flags) {
	for(size_t i = 0; i < scope->_objects.size(); i++) {
		if((flags & resolveCopy) && scope->_objects[i]->isMainObject)
			continue;

		frg::optional<ObjectSymbol> p = resolveInObject(scope->_objects[i], string);
		if(p)
			return p;
	}

	return frg::optional<ObjectSymbol>();
}

Scope::Scope()
: _objects(getAllocator()) { }

void Scope::appendObject(SharedObject *object) {
	_objects.push(object);
}

// TODO: let this return uintptr_t
frg::optional<ObjectSymbol> Scope::resolveSymbol(ObjectSymbol r, ResolveFlags flags) {
	for(size_t i = 0; i < _objects.size(); i++) {
		if((flags & resolveCopy) && _objects[i]->isMainObject)
			continue;

		const char *string = (const char *)(r.object()->baseAddress
				+ r.object()->stringTableOffset + r.symbol()->st_name);
		frg::optional<ObjectSymbol> p = resolveInObject(_objects[i], string);
		if(p)
			return p;
	}

	return frg::optional<ObjectSymbol>();
}


// --------------------------------------------------------
// Loader
// --------------------------------------------------------

Loader::Loader(Scope *scope, bool is_initial_link, uint64_t rts)
: _globalScope{scope}, _isInitialLink{is_initial_link}, _linkRts{rts},
		_linkSet{frg::hash<SharedObject *>{}, getAllocator()},
		_linkBfs{getAllocator()}, _initQueue{getAllocator()} { }

// TODO: Use an explicit vector to reduce stack usage to O(1)?
void Loader::submitObject(SharedObject *object) {
	if(_linkSet.get(object))
		return;

	_linkSet.insert(object, Token{});
	_linkBfs.push(object);

	for(size_t i = 0; i < object->dependencies.size(); i++)
		submitObject(object->dependencies[i]);
}

void Loader::linkObjects() {
	_buildTlsMaps();

	// Promote objects to the global scope.
	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
		if((*it)->globalRts)
			continue;
		(*it)->globalRts = _linkRts;
		_globalScope->appendObject(*it);
	}

	// Process regular relocations.
	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
		// Some objects have already been linked before.
		if((*it)->objectRts < _linkRts)
			continue;

		if(verbose)
			mlibc::infoLogger() << "rtdl: Linking " << (*it)->name << frg::endlog;

		__ensure(!(*it)->wasLinked);
		(*it)->loadScope = _globalScope;

		// TODO: Support this.
		if((*it)->symbolicResolution)
			mlibc::infoLogger() << "\e[31mrtdl: DT_SYMBOLIC is not implemented correctly!\e[39m"
					<< frg::endlog;

		_processStaticRelocations(*it);
		_processLazyRelocations(*it);
	}
	
	// Process copy relocations.
	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
		if(!(*it)->isMainObject)
			continue;

		// Some objects have already been linked before.
		if((*it)->objectRts < _linkRts)
			continue;

		processCopyRelocations(*it);
	}
	
	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it)
		(*it)->wasLinked = true;
}

void Loader::_buildTlsMaps() {
	if(_isInitialLink) {
		__ensure(runtimeTlsMap->initialPtr == 0);
		__ensure(runtimeTlsMap->initialLimit == 0);

		__ensure(!_linkBfs.empty());
		__ensure(_linkBfs.front()->isMainObject);

		for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
			SharedObject *object = *it;
			__ensure(object->tlsModel == TlsModel::null);

			if(object->tlsSegmentSize == 0)
				continue;

			__ensure(16 % object->tlsAlignment == 0);
			runtimeTlsMap->initialPtr += object->tlsSegmentSize;
			size_t misalign = runtimeTlsMap->initialPtr % object->tlsAlignment;
			if(misalign)
				runtimeTlsMap->initialPtr += object->tlsAlignment - misalign;

			object->tlsModel = TlsModel::initial;
			object->tlsOffset = -runtimeTlsMap->initialPtr;
			
			if(verbose)
				mlibc::infoLogger() << "rtdl: TLS of " << object->name
						<< " mapped to 0x" << frg::hex_fmt{object->tlsOffset}
						<< ", size: " << object->tlsSegmentSize
						<< ", alignment: " << object->tlsAlignment << frg::endlog;
		}

		// Reserve some additional space for future libraries.
		runtimeTlsMap->initialLimit = runtimeTlsMap->initialPtr + 64;
	}else{
		for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
			SharedObject *object = *it;
			
			if(object->tlsModel != TlsModel::null)
				continue;
			if(object->tlsSegmentSize == 0)
				continue;

			// There are some libraries (e.g. Mesa) that require static TLS even though
			// they expect to be dynamically loaded.
			if(object->haveStaticTls) {
				__ensure(16 % object->tlsAlignment == 0);
				auto ptr = runtimeTlsMap->initialPtr + object->tlsSegmentSize;
				size_t misalign = ptr % object->tlsAlignment;
				if(misalign)
					ptr += object->tlsAlignment - misalign;

				if(ptr > runtimeTlsMap->initialLimit)
					mlibc::panicLogger() << "rtdl: Static TLS space exhausted while while"
							" allocating TLS for " << object->name << frg::endlog;
				runtimeTlsMap->initialPtr = ptr;

				object->tlsModel = TlsModel::initial;
				object->tlsOffset = -runtimeTlsMap->initialPtr;
				
//				if(verbose)
					mlibc::infoLogger() << "rtdl: TLS of " << object->name
							<< " mapped to 0x" << frg::hex_fmt{object->tlsOffset}
							<< ", size: " << object->tlsSegmentSize
							<< ", alignment: " << object->tlsAlignment << frg::endlog;
			}else{
				// TODO: Implement dynamic TLS.
				mlibc::panicLogger() << "rtdl: Dynamic TLS is not supported" << frg::endlog;
			}
		}
	}
}

void Loader::initObjects() {
	// Initialize TLS segments that follow the static model.
	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
		SharedObject *object = *it;
		
		if(object->tlsModel == TlsModel::initial) {
			if(object->tlsInitialized)
				continue;

			char *tcb_ptr;
			asm volatile ("mov %%fs:(0), %0" : "=r"(tcb_ptr));
			auto tls_ptr = tcb_ptr + object->tlsOffset;
			memcpy(tls_ptr, object->tlsImagePtr, object->tlsImageSize);

			object->tlsInitialized = true;
		}
	}

	for(auto it = _linkBfs.begin(); it != _linkBfs.end(); ++it) {
		if(!(*it)->scheduledForInit)
			_scheduleInit((*it));
	}

	// TODO: We don't we initialize in _linkBfs order?
	for(auto it = _initQueue.begin(); it != _initQueue.end(); ++it) {
		SharedObject *object = *it;
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
	if(type == R_X86_64_COPY)
		return;
	
	// resolve the symbol if there is a symbol
	frg::optional<ObjectSymbol> p;
	if(symbol_index) {
		auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
				+ symbol_index * sizeof(Elf64_Sym));
		ObjectSymbol r(object, symbol);
		p = object->loadScope->resolveSymbol(r, 0);
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
		__ensure(p->object()->tlsModel == TlsModel::initial);
		*((uint64_t *)rel_addr) = p->symbol()->st_value;
	} break;
	case R_X86_64_TPOFF64: {
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
				mlibc::infoLogger() << "rtdl: Warning: TPOFF64 with no symbol"
						" in object " << object->name << frg::endlog;
			if(object->tlsModel != TlsModel::initial)
				mlibc::panicLogger() << "rtdl: In object " << object->name
						<< ": Static TLS relocation to dynamically loaded object "
						<< object->name << frg::endlog;
			*((uint64_t *)rel_addr) = object->tlsOffset;
		}
	} break;
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
			rela_offset = dynamic->d_ptr;
			break;
		case DT_RELASZ:
			rela_length = dynamic->d_val;
			break;
		case DT_RELAENT:
			__ensure(dynamic->d_val == sizeof(Elf64_Rela));
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

		__ensure(type == R_X86_64_JUMP_SLOT);
		if(eagerBinding) {
			auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
					+ symbol_index * sizeof(Elf64_Sym));
			ObjectSymbol r(object, symbol);
			frg::optional<ObjectSymbol> p = object->loadScope->resolveSymbol(r, 0);
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
	}
}

