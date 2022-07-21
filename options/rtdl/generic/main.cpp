
#include <elf.h>
#include <link.h>

#include <frg/manual_box.hpp>

#include <abi-bits/auxv.h>
#include <mlibc/debug.hpp>
#include <mlibc/rtdl-sysdeps.hpp>
#include <mlibc/rtdl-config.hpp>
#include <mlibc/rtdl-abi.hpp>
#include <mlibc/stack_protector.hpp>
#include <internal-config.h>
#include "linker.hpp"

#ifdef __MLIBC_POSIX_OPTION
#include <dlfcn.h>
#endif

#define HIDDEN  __attribute__ ((visibility ("hidden")))
#define EXPORT  __attribute__ ((visibility ("default")))

static constexpr bool logEntryExit = false;
static constexpr bool logStartup = false;
static constexpr bool logDlCalls = false;

#ifndef MLIBC_STATIC_BUILD
extern HIDDEN void *_GLOBAL_OFFSET_TABLE_[];
extern HIDDEN Elf64_Dyn _DYNAMIC[];
#endif

namespace mlibc {
	// Declared in options/internal/mlibc/tcb.hpp.
	bool tcb_available_flag = false;
}

mlibc::RtdlConfig rtdlConfig;

uintptr_t *entryStack;
frg::manual_box<ObjectRepository> initialRepository;
frg::manual_box<Scope> globalScope;

frg::manual_box<RuntimeTlsMap> runtimeTlsMap;

static SharedObject *executableSO;
extern HIDDEN char __ehdr_start[];

// Global debug interface variable
DebugInterface globalDebugInterface;

#ifndef MLIBC_STATIC_BUILD

// Use a PC-relative instruction sequence to find our runtime load address.
uintptr_t getLdsoBase() {
#if defined(__x86_64__) || defined(__aarch64__)
	// On x86_64, the first GOT entry holds the link-time address of _DYNAMIC.
	// TODO: This isn't guaranteed on AArch64, so this might fail with some linkers.
	auto linktime_dynamic = reinterpret_cast<uintptr_t>(_GLOBAL_OFFSET_TABLE_[0]);
	auto runtime_dynamic = reinterpret_cast<uintptr_t>(_DYNAMIC);
	return runtime_dynamic - linktime_dynamic;
#elif defined(__riscv)
	return reinterpret_cast<uintptr_t>(&__ehdr_start);
#endif
}

// Relocates the dynamic linker (i.e. this DSO) itself.
// Assumptions:
// - There are no references to external symbols.
// Note that this code is fragile in the sense that it must not contain relocations itself.
// TODO: Use tooling to verify this at compile time.
extern "C" void relocateSelf() {
	size_t rela_offset = 0;
	size_t rela_size = 0;
	for(size_t i = 0; _DYNAMIC[i].d_tag != DT_NULL; i++) {
		auto ent = &_DYNAMIC[i];
		switch(ent->d_tag) {
		case DT_RELA: rela_offset = ent->d_un.d_ptr; break;
		case DT_RELASZ: rela_size = ent->d_un.d_val; break;
		}
	}

	auto ldso_base = getLdsoBase();

	for(size_t disp = 0; disp < rela_size; disp += sizeof(Elf64_Rela)) {
		auto reloc = reinterpret_cast<Elf64_Rela *>(ldso_base + rela_offset + disp);

		Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
		if(ELF64_R_SYM(reloc->r_info))
			__builtin_trap();

		auto p = reinterpret_cast<uint64_t *>(ldso_base + reloc->r_offset);
		switch(type) {
#if defined(__x86_64__)
		case R_X86_64_RELATIVE:
#elif defined(__aarch64__)
		case R_AARCH64_RELATIVE:
#elif defined(__riscv)
		case R_RISCV_RELATIVE:
#endif
			*p = ldso_base + reloc->r_addend;
			break;
		default:
			__builtin_trap();
		}
	}
}
#endif

extern "C" void *lazyRelocate(SharedObject *object, unsigned int rel_index) {
	__ensure(object->lazyExplicitAddend);
	auto reloc = (Elf64_Rela *)(object->baseAddress + object->lazyRelocTableOffset
			+ rel_index * sizeof(Elf64_Rela));
	Elf64_Xword type = ELF64_R_TYPE(reloc->r_info);
	Elf64_Xword symbol_index = ELF64_R_SYM(reloc->r_info);

	__ensure(type == R_X86_64_JUMP_SLOT);

	auto symbol = (Elf64_Sym *)(object->baseAddress + object->symbolTableOffset
			+ symbol_index * sizeof(Elf64_Sym));
	ObjectSymbol r(object, symbol);
	auto p = object->localScope->resolveSymbol(r.getString(), 0);
	if(!p)
		mlibc::panicLogger() << "Unresolved JUMP_SLOT symbol" << frg::endlog;

	//mlibc::infoLogger() << "Lazy relocation to " << symbol_str
	//		<< " resolved to " << pointer << frg::endlog;

	*(uint64_t *)(object->baseAddress + reloc->r_offset) = p->virtualAddress();
	return (void *)p->virtualAddress();
}

extern "C" [[ gnu::visibility("default") ]] void *__rtdl_allocateTcb() {
	auto tcb = allocateTcb();
	initTlsObjects(tcb, globalScope->_objects, false);
	return tcb;
}

extern "C" {
	[[ gnu::visibility("hidden") ]] void dl_debug_state() {
		// This function is used to signal changes in the debugging link map,
		// GDB just sets a breakpoint on this function and we can call it
		// everytime we update the link map. We don't need to implement
		// anything besides defining and calling it.
	}
}

extern "C" [[gnu::alias("dl_debug_state"), gnu::visibility("default")]] void _dl_debug_state() noexcept;

// This symbol can be used by GDB to find the global interface structure
[[ gnu::visibility("default") ]] DebugInterface *_dl_debug_addr = &globalDebugInterface;

extern "C" void *interpreterMain(uintptr_t *entry_stack) {
	if(logEntryExit)
		mlibc::infoLogger() << "Entering ld.so" << frg::endlog;
	entryStack = entry_stack;
	runtimeTlsMap.initialize();

	void *phdr_pointer = 0;
	size_t phdr_entry_size = 0;
	size_t phdr_count = 0;
	void *entry_pointer = 0;
	void *stack_entropy = nullptr;

	const char *execfn = "(executable)";

#ifndef MLIBC_STATIC_BUILD
	auto ldso_base = getLdsoBase();
	if(logStartup) {
		mlibc::infoLogger() << "ldso: Own base address is: 0x"
				<< frg::hex_fmt(ldso_base) << frg::endlog;
		mlibc::infoLogger() << "ldso: Own dynamic section is at: " << _DYNAMIC << frg::endlog;
	}

#ifdef __x86_64__
	// These entries are reserved on x86_64.
	// TODO: Use a fake PLT stub that reports an error message?
	_GLOBAL_OFFSET_TABLE_[1] = 0;
	_GLOBAL_OFFSET_TABLE_[2] = 0;
#endif

	// Validate our own dynamic section.
	// Here, we make sure that the dynamic linker does not need relocations itself.
	uintptr_t strtab_offset = 0;
	uintptr_t soname_str = 0;
	for(size_t i = 0; _DYNAMIC[i].d_tag != DT_NULL; i++) {
		auto ent = &_DYNAMIC[i];
		switch(ent->d_tag) {
		case DT_STRTAB: strtab_offset = ent->d_un.d_ptr; break;
		case DT_SONAME: soname_str = ent->d_un.d_val; break;
		case DT_HASH:
		case DT_GNU_HASH:
		case DT_STRSZ:
		case DT_SYMTAB:
		case DT_SYMENT:
		case DT_RELA:
		case DT_RELASZ:
		case DT_RELAENT:
		case DT_RELACOUNT:
		case DT_DEBUG:
			continue;
		default:
			__ensure(!"Unexpected dynamic entry in program interpreter");
		}
	}
	__ensure(strtab_offset);
	__ensure(soname_str);

	// Find the auxiliary vector by skipping args and environment.
	auto aux = entryStack;
	aux += *aux + 1; // First, we skip argc and all args.
	__ensure(!*aux);
	aux++;
	while(*aux) // Now, we skip the environment.
		aux++;
	aux++;

	// Parse the actual vector.
	while(true) {
		auto value = aux + 1;
		if(!(*aux))
			break;

		// TODO: Whitelist auxiliary vector entries here?
		switch(*aux) {
			case AT_PHDR: phdr_pointer = reinterpret_cast<void *>(*value); break;
			case AT_PHENT: phdr_entry_size = *value; break;
			case AT_PHNUM: phdr_count = *value; break;
			case AT_ENTRY: entry_pointer = reinterpret_cast<void *>(*value); break;
			case AT_EXECFN: execfn = reinterpret_cast<const char *>(*value); break;
			case AT_RANDOM: stack_entropy = reinterpret_cast<void*>(*value); break;
			case AT_SECURE: rtdlConfig.secureRequired = reinterpret_cast<uintptr_t>(*value); break;
		}

		aux += 2;
	}
	globalDebugInterface.base = reinterpret_cast<void*>(ldso_base);
#else
	auto ehdr = reinterpret_cast<Elf64_Ehdr*>(__ehdr_start);
	phdr_pointer = reinterpret_cast<void*>((uintptr_t)ehdr->e_phoff + (uintptr_t)ehdr);
	phdr_entry_size = ehdr->e_phentsize;
	phdr_count = ehdr->e_phnum;
	entry_pointer = reinterpret_cast<void*>(ehdr->e_entry);
#endif
	__ensure(phdr_pointer);
	__ensure(entry_pointer);

	if(logStartup)
		mlibc::infoLogger() << "ldso: Executable PHDRs are at " << phdr_pointer
				<< frg::endlog;

	// perform the initial dynamic linking
	initialRepository.initialize();

	globalScope.initialize(true);

	// Add the dynamic linker, as well as the exectuable to the repository.
#ifndef MLIBC_STATIC_BUILD
	auto ldso_soname = reinterpret_cast<const char *>(ldso_base + strtab_offset + soname_str);
	auto ldso = initialRepository->injectObjectFromDts(ldso_soname,
		frg::string<MemoryAllocator> { getAllocator() },
		ldso_base, _DYNAMIC, 1);
	ldso->phdrPointer = phdr_pointer;
	ldso->phdrCount = phdr_count;
	ldso->phdrEntrySize = phdr_entry_size;

	// TODO: support non-zero base addresses?
	executableSO = initialRepository->injectObjectFromPhdrs(execfn,
		frg::string<MemoryAllocator> { execfn, getAllocator() },
		phdr_pointer, phdr_entry_size, phdr_count, entry_pointer, 1);

	// We can't initialise the ldso object after the executable SO,
	// so we have to set the ldso path after loading both.
	ldso->path = executableSO->interpreterPath;

#else
	executableSO = initialRepository->injectStaticObject(execfn,
			frg::string<MemoryAllocator>{ execfn, getAllocator() },
			phdr_pointer, phdr_entry_size, phdr_count, entry_pointer, 1);
	globalDebugInterface.base = (void*)executableSO->baseAddress;
#endif

	globalDebugInterface.head = &executableSO->linkMap;
	executableSO->inLinkMap = true;
	Loader linker{globalScope.get(), executableSO, true, 1};
	linker.linkObjects(executableSO);

	mlibc::initStackGuard(stack_entropy);

	auto tcb = allocateTcb();
	if(mlibc::sys_tcb_set(tcb))
		__ensure(!"sys_tcb_set() failed");
	tcb->tid = mlibc::this_tid();
	mlibc::tcb_available_flag = true;

	globalDebugInterface.ver = 1;
	globalDebugInterface.brk = &dl_debug_state;
	globalDebugInterface.state = 0;
	dl_debug_state();

	linker.initObjects();

	if(logEntryExit)
		mlibc::infoLogger() << "Leaving ld.so, jump to "
				<< (void *)executableSO->entry << frg::endlog;
	return executableSO->entry;
}

const char *lastError;

extern "C" [[ gnu::visibility("default") ]] uintptr_t *__dlapi_entrystack() {
	return entryStack;
}

extern "C" [[ gnu::visibility("default") ]]
const char *__dlapi_error() {
	auto error = lastError;
	lastError = nullptr;
	return error;
}

extern "C" [[ gnu::visibility("default") ]]
void *__dlapi_get_tls(struct __abi_tls_entry *entry) {
	return reinterpret_cast<char *>(accessDtv(entry->object)) + entry->offset;
}

extern "C" [[ gnu::visibility("default") ]]
const mlibc::RtdlConfig &__dlapi_get_config() {
	return rtdlConfig;
}

#ifdef __MLIBC_POSIX_OPTION

extern "C" [[ gnu::visibility("default") ]]
void *__dlapi_open(const char *file, int flags, void *returnAddress) {
	if (logDlCalls)
		mlibc::infoLogger() << "rtdl: __dlapi_open(" << (file ? file : "nullptr") << ")" << frg::endlog;

	auto unhandled = RTLD_NOLOAD | RTLD_LOCAL | RTLD_DEEPBIND;
	if (flags & unhandled) {
		mlibc::infoLogger() << "rtdl: dlopen flag " << (flags & unhandled)
			<< "is unsupported" << frg::endlog;
	}

	if(!file)
		return executableSO;

	// TODO: Thread-safety!
	auto rts = rtsCounter++;

	SharedObject *object;
	if(frg::string_view{file}.find_first('/') == size_t(-1)) {
		// In order to know which RUNPATH / RPATH to process, we must find the calling object.
		SharedObject *origin = initialRepository->findCaller(returnAddress);
		if (!origin) {
			mlibc::panicLogger() << "rtdl: unable to determine calling object of dlopen "
				<< "(ra = " << returnAddress << ")" << frg::endlog;
		}

		object = initialRepository->requestObjectWithName(file, origin, rts);
	}else{
		object = initialRepository->requestObjectAtPath(file, rts);
	}
	if(!object) {
		lastError = "Cannot locate requested DSO";
		return nullptr;
	}

	Loader linker{globalScope.get(), nullptr, false, rts};
	linker.linkObjects(object);
	linker.initObjects();

	// Build the object scope. TODO: Use the Loader object to do this.
	if(!object->localScope) {
		struct Token { };

		using Set = frg::hash_map<SharedObject *, Token,
				frg::hash<SharedObject *>, MemoryAllocator>;
		Set set{frg::hash<SharedObject *>{}, getAllocator()};

		object->localScope = frg::construct<Scope>(getAllocator());
		frg::vector<SharedObject *, MemoryAllocator> queue{getAllocator()};

		object->localScope->appendObject(object);
		set.insert(object, Token{});
		queue.push(object);

		// Loop over indices (not iterators) here: We are adding elements in the loop!
		for(size_t i = 0; i < queue.size(); i++) {
			auto current = queue[i];
			if(set.get(current))
				continue;

			object->localScope->appendObject(current);
			set.insert(current, Token{});
			queue.push(current);
		}
	}

	dl_debug_state();

	return object;
}

extern "C" [[ gnu::visibility("default") ]]
void *__dlapi_resolve(void *handle, const char *string, void *returnAddress) {
	if (logDlCalls) {
		const char *name;
		bool quote = false;
		if (handle == RTLD_DEFAULT) {
			name = "RTLD_DEFAULT";
		} else if (handle == RTLD_NEXT) {
			name = "RTLD_NEXT";
		} else {
			name = ((SharedObject *)handle)->name.data();
			quote = true;
		}

		mlibc::infoLogger() << "rtdl: __dlapi_resolve(" << (quote ? "\"" : "") << name
			<< (quote ? "\"" : "") << ", \"" << string << "\")" << frg::endlog;
	}

	frg::optional<ObjectSymbol> target;

	if (handle == RTLD_DEFAULT) {
		target = globalScope->resolveSymbol(string, 0);
	} else if (handle == RTLD_NEXT) {
		SharedObject *origin = initialRepository->findCaller(returnAddress);
		if (!origin) {
			mlibc::panicLogger() << "rtdl: unable to determine calling object of dlsym "
				<< "(ra = " << returnAddress << ")" << frg::endlog;
		}

		target = globalScope->resolveNext(string, origin);
	} else {
		// POSIX does not unambiguously state how dlsym() is supposed to work; it just
		// states that "The symbol resolution algorithm used shall be dependency order
		// as described in dlopen()".
		//
		// Linux libc's lookup the symbol in the given DSO and all of its dependencies
		// in breadth-first order. That is also what we implement here.
		//
		// Note that this *differs* from the algorithm that is used for relocations
		// (since the algorithm used for relocations takes (i) the global scope,
		// and (ii) the local scope of the DSO into account (which can contain more objects
		// than just the dependencies of the DSO, if the DSO was loaded as a dependency
		// of a dlopen()ed DSO).

		frg::vector<SharedObject *, MemoryAllocator> queue{getAllocator()};

		struct Token { };
		frg::hash_map<
				SharedObject *, Token,
				frg::hash<SharedObject *>, MemoryAllocator
		> visited{frg::hash<SharedObject *>{}, getAllocator()};

		auto root = reinterpret_cast<SharedObject *>(handle);
		visited.insert(root, Token{});
		queue.push_back(root);

		for(size_t i = 0; i < queue.size(); i++) {
			auto current = queue[i];

			target = resolveInObject(current, string);
			if(target)
				break;

			for(auto dep : current->dependencies) {
				if(visited.get(dep))
					continue;
				visited.insert(dep, Token{});
				queue.push_back(dep);
			}
		}
	}

	if (!target) {
		if (logDlCalls)
			mlibc::infoLogger() << "rtdl: could not resolve \"" << string << "\"" << frg::endlog;

		lastError = "Cannot resolve requested symbol";
		return nullptr;
	}
	return reinterpret_cast<void *>(target->virtualAddress());
}

struct __dlapi_symbol {
	const char *file;
	void *base;
	const char *symbol;
	void *address;
};

extern "C" [[ gnu::visibility("default") ]]
int __dlapi_reverse(const void *ptr, __dlapi_symbol *info) {
	if (logDlCalls)
		mlibc::infoLogger() << "rtdl: __dlapi_reverse(" << ptr << ")" << frg::endlog;

	for(size_t i = 0; i < globalScope->_objects.size(); i++) {
		auto object = globalScope->_objects[i];

		auto eligible = [&] (ObjectSymbol cand) {
			if(cand.symbol()->st_shndx == SHN_UNDEF)
				return false;

			auto bind = ELF64_ST_BIND(cand.symbol()->st_info);
			if(bind != STB_GLOBAL && bind != STB_WEAK)
				return false;

			return true;
		};

		auto hash_table = (Elf64_Word *)(object->baseAddress + object->hashTableOffset);
		auto num_symbols = hash_table[1];
		for(size_t i = 0; i < num_symbols; i++) {
			ObjectSymbol cand{object, (Elf64_Sym *)(object->baseAddress
					+ object->symbolTableOffset + i * sizeof(Elf64_Sym))};
			if(eligible(cand) && cand.virtualAddress() == reinterpret_cast<uintptr_t>(ptr)) {
				if (logDlCalls)
					mlibc::infoLogger() << "rtdl: Found symbol " << cand.getString() << " in object "
							<< object->path << frg::endlog;

				info->file = object->path.data();
				info->base = reinterpret_cast<void *>(object->baseAddress);
				info->symbol = cand.getString();
				info->address = reinterpret_cast<void *>(cand.virtualAddress());
				return 0;
			}
		}
	}

	// Not found, find the DSO it should be in.
	for(size_t i = 0; i < globalScope->_objects.size(); i++) {
		auto object = globalScope->_objects[i];

		for(size_t j = 0; j < object->phdrCount; j++) {
			auto phdr = (Elf64_Phdr *)((uintptr_t)object->phdrPointer + j * object->phdrEntrySize);
			if(phdr->p_type != PT_LOAD) {
				continue;
			}
			uintptr_t start = object->baseAddress + phdr->p_vaddr;
			uintptr_t end = start + phdr->p_memsz;
			if(reinterpret_cast<uintptr_t>(ptr) >= start && reinterpret_cast<uintptr_t>(ptr) < end) {
				mlibc::infoLogger() << "rtdl: Found DSO " << object->path << frg::endlog;
				info->file = object->path.data();
				info->base = reinterpret_cast<void *>(object->baseAddress);
				info->symbol = nullptr;
				info->address = 0;
				return 0;
			}
		}
	}

	if (logDlCalls)
		mlibc::infoLogger() << "rtdl: Could not find symbol in __dlapi_reverse()" << frg::endlog;

	return -1;
}

extern "C" [[ gnu::visibility("default") ]]
int __dlapi_close(void *) {
	if (logDlCalls)
		mlibc::infoLogger() << "mlibc: dlclose() is a no-op" << frg::endlog;
	return 0;
}

#endif

extern "C" [[ gnu::visibility("default") ]]
int __dlapi_iterate_phdr(int (*callback)(struct dl_phdr_info *, size_t, void*), void *data) {
	int last_return = 0;
	for (auto object : initialRepository->loadedObjects) {
		struct dl_phdr_info info;
		info.dlpi_addr = object->baseAddress;
		info.dlpi_name = object->name.data();

		if(object->isMainObject) {
			info.dlpi_name = "";
		} else {
			info.dlpi_name = object->name.data();
		}
		info.dlpi_phdr = static_cast<ElfW(Phdr)*>(object->phdrPointer);
		info.dlpi_phnum = object->phdrCount;
		info.dlpi_adds = rtsCounter;
		info.dlpi_subs = 0; // TODO(geert): implement dlclose().
		if (object->tlsModel != TlsModel::null)
			info.dlpi_tls_modid = object->tlsIndex;
		else
			info.dlpi_tls_modid = 0;
		info.dlpi_tls_data = tryAccessDtv(object);

		last_return = callback(&info, sizeof(struct dl_phdr_info), data);
		if(last_return)
			return last_return;
	}

	return last_return;
}

extern "C" [[ gnu::visibility("default") ]]
void __dlapi_enter(uintptr_t *entry_stack) {
#ifdef MLIBC_STATIC_BUILD
	interpreterMain(entry_stack);
#else
	(void)entry_stack;
#endif
}

