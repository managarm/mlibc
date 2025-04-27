
#include <elf.h>
#include <link.h>

#include <frg/manual_box.hpp>
#include <frg/small_vector.hpp>

#include <abi-bits/auxv.h>
#include <mlibc/debug.hpp>
#include <mlibc/rtld-sysdeps.hpp>
#include <mlibc/rtld-config.hpp>
#include <mlibc/rtld-abi.hpp>
#include <mlibc/stack_protector.hpp>
#include <internal-config.h>
#include <abi-bits/auxv.h>

#include "elf.hpp"
#include "linker.hpp"

#if __MLIBC_POSIX_OPTION
#include <dlfcn.h>
#endif

#define HIDDEN  __attribute__((__visibility__("hidden")))
#define EXPORT  __attribute__((__visibility__("default")))

static constexpr bool logEntryExit = false;
static constexpr bool logStartup = false;
static constexpr bool logDlCalls = false;

#ifndef MLIBC_STATIC_BUILD
extern HIDDEN void *_GLOBAL_OFFSET_TABLE_[];
extern HIDDEN elf_dyn _DYNAMIC[];
#endif

namespace mlibc {
	// Declared in options/internal/mlibc/tcb.hpp.
	bool tcb_available_flag = false;
}

mlibc::RtldConfig rtldConfig;

bool ldShowAuxv = false;

uintptr_t *entryStack;
static constinit Tcb earlyTcb{};
frg::manual_box<ObjectRepository> initialRepository;
frg::manual_box<Scope> globalScope;

frg::manual_box<RuntimeTlsMap> runtimeTlsMap;

// We use a small vector to avoid memory allocation for the default library paths
frg::manual_box<frg::small_vector<frg::string_view, MLIBC_NUM_DEFAULT_LIBRARY_PATHS, MemoryAllocator>> libraryPaths;

frg::manual_box<frg::vector<frg::string_view, MemoryAllocator>> preloads;

static SharedObject *executableSO;
extern HIDDEN char __ehdr_start[];

// Global debug interface variable
DebugInterface globalDebugInterface;

#ifndef MLIBC_STATIC_BUILD

// Use a PC-relative instruction sequence to find our runtime load address.
uintptr_t getLdsoBase() {
#if defined(__x86_64__) || defined(__i386__) || defined(__aarch64__) || defined(__m68k__) || defined(__loongarch64)
	// On x86_64, the first GOT entry holds the link-time address of _DYNAMIC.
	// TODO: This isn't guaranteed on AArch64, so this might fail with some linkers.
	auto linktime_dynamic = reinterpret_cast<uintptr_t>(_GLOBAL_OFFSET_TABLE_[0]);
	auto runtime_dynamic = reinterpret_cast<uintptr_t>(_DYNAMIC);
	return runtime_dynamic - linktime_dynamic;
#elif defined(__riscv)
	return reinterpret_cast<uintptr_t>(&__ehdr_start);
#else
	#error Unknown architecture!
#endif
}

#if !defined(__m68k__)
// Relocates the dynamic linker (i.e. this DSO) itself.
// Assumptions:
// - There are no references to external symbols.
// Note that this code is fragile in the sense that it must not contain relocations itself.
// TODO: Use tooling to verify this at compile time.
extern "C" void relocateSelf() {
	size_t rela_offset = 0;
	size_t rela_size = 0;
	size_t rel_offset = 0;
	size_t rel_size = 0;
	size_t relr_offset = 0;
	size_t relr_size = 0;
	for(size_t i = 0; _DYNAMIC[i].d_tag != DT_NULL; i++) {
		auto ent = &_DYNAMIC[i];
		switch(ent->d_tag) {
		case DT_REL: rel_offset = ent->d_un.d_ptr; break;
		case DT_RELSZ: rel_size = ent->d_un.d_val; break;
		case DT_RELA: rela_offset = ent->d_un.d_ptr; break;
		case DT_RELASZ: rela_size = ent->d_un.d_val; break;
		case DT_RELR: relr_offset = ent->d_un.d_ptr; break;
		case DT_RELRSZ: relr_size = ent->d_un.d_val; break;
		}
	}

	auto ldso_base = getLdsoBase();

	for(size_t disp = 0; disp < rela_size; disp += sizeof(elf_rela)) {
		auto reloc = reinterpret_cast<elf_rela *>(ldso_base + rela_offset + disp);

		auto type = ELF_R_TYPE(reloc->r_info);
		if(ELF_R_SYM(reloc->r_info))
			__builtin_trap();

		auto p = reinterpret_cast<uint64_t *>(ldso_base + reloc->r_offset);
		switch(type) {
		case R_RELATIVE:
			*p = ldso_base + reloc->r_addend;
			break;
		default:
			__builtin_trap();
		}
	}

	for(size_t disp = 0; disp < rel_size; disp += sizeof(elf_rel)) {
		auto reloc = reinterpret_cast<elf_rel *>(ldso_base + rel_offset + disp);

		auto type = ELF_R_TYPE(reloc->r_info);
		if(ELF_R_SYM(reloc->r_info))
			__builtin_trap();

		auto p = reinterpret_cast<uint64_t *>(ldso_base + reloc->r_offset);
		switch(type) {
		case R_RELATIVE:
			*p += ldso_base;
			break;
		default:
			__builtin_trap();
		}
	}

	elf_addr *addr = nullptr;
	for(size_t disp = 0; disp < relr_size; disp += sizeof(elf_relr)) {
		auto entry = *(elf_relr *)(ldso_base + relr_offset + disp);

		// Even entry indicates the beginning address.
		if(!(entry & 1)) {
			addr = (elf_addr *)(ldso_base + entry);
			__ensure(addr);
			*addr++ += ldso_base;
		}else {
			// Odd entry indicates entry is a bitmap of the subsequent locations to be relocated.

			// The first bit of an entry is always a marker about whether the entry is an address or a bitmap,
			// discard it.
			entry >>= 1;

			for(int i = 0; entry; ++i) {
				if(entry & 1) {
					addr[i] += ldso_base;
				}
				entry >>= 1;
			}

			// Each entry describes at max 63 (on 64bit) or 31 (on 32bit) subsequent locations.
			addr += CHAR_BIT * sizeof(elf_relr) - 1;
		}
	}
}
#else
// m68k needs a tighter relocation function to avoid itself relying on the GOT.
extern "C" void relocateSelf68k(elf_dyn *dynamic, uintptr_t ldso_base) {
	size_t rela_offset = 0;
	size_t rela_size = 0;
	for(size_t i = 0; dynamic[i].d_tag != DT_NULL; i++) {
		auto ent = &dynamic[i];
		switch(ent->d_tag) {
		case DT_RELA: rela_offset = ent->d_un.d_ptr; break;
		case DT_RELASZ: rela_size = ent->d_un.d_val; break;
		}
	}

	for(size_t disp = 0; disp < rela_size; disp += sizeof(elf_rela)) {
		auto reloc = reinterpret_cast<elf_rela *>(ldso_base + rela_offset + disp);
		auto type = ELF_R_TYPE(reloc->r_info);

		auto p = reinterpret_cast<uintptr_t *>(ldso_base + reloc->r_offset);
		switch(type) {
		case R_NONE:
			break;

		case R_RELATIVE:
			*p = ldso_base + reloc->r_addend;
			break;
		default: {
			__builtin_trap();
		}
		}
	}
}
#endif // !defined(__m68k__)
#endif

extern "C" void *lazyRelocate(SharedObject *object, unsigned int rel_index) {
	__ensure(object->lazyExplicitAddend);
	auto reloc = (elf_rela *)(object->baseAddress + object->lazyRelocTableOffset
			+ rel_index * sizeof(elf_rela));
	auto type = ELF_R_TYPE(reloc->r_info);
	auto symbol_index = ELF_R_SYM(reloc->r_info);

	__ensure(type == R_X86_64_JUMP_SLOT);
	__ensure(ELF_CLASS == ELFCLASS64);

	auto [sym, ver] = object->getSymbolByIndex(symbol_index);
	auto p = Scope::resolveGlobalOrLocal(*globalScope, object->localScope, sym.getString(), object->objectRts, 0, ver);
	if(!p)
		mlibc::panicLogger() << "Unresolved JUMP_SLOT symbol" << frg::endlog;

	//mlibc::infoLogger() << "Lazy relocation to " << symbol_str
	//		<< " resolved to " << pointer << frg::endlog;

	*(uint64_t *)(object->baseAddress + reloc->r_offset) = p->virtualAddress();
	return (void *)p->virtualAddress();
}

extern "C" [[ gnu::visibility("default") ]] void *__rtld_allocateTcb() {
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

static frg::vector<frg::string_view, MemoryAllocator> parseList(frg::string_view paths, frg::string_view separators) {
	frg::vector<frg::string_view, MemoryAllocator> list{getAllocator()};

	size_t p = 0;
	while(p < paths.size()) {
		size_t s; // Offset of next colon or end of string.
		if(size_t cs = paths.find_first_of(separators, p); cs != size_t(-1)) {
			s = cs;
		}else{
			s = paths.size();
		}

		auto path = paths.sub_string(p, s - p);
		p = s + 1;

		if(path.size() == 0)
			continue;

		if(path.ends_with("/")) {
			size_t i = path.size() - 1;
			while(i > 0 && path[i] == '/')
				i--;
			path = path.sub_string(0, i + 1);
		}

		if(path == "/")
			path = "";

		list.push_back(path);
	}

	return list;
}

#ifndef MLIBC_STATIC_BUILD
static constexpr uint64_t supportedDtFlags = DF_BIND_NOW;
static constexpr uint64_t supportedDtFlags1 = DF_1_NOW;
#endif

extern "C" void *interpreterMain(uintptr_t *entry_stack) {
	if(logEntryExit)
		mlibc::infoLogger() << "Entering ld.so" << frg::endlog;
	entryStack = entry_stack;

	// Set up an early TCB such that we can cache our own TID.
	// The TID is needed to use futexes, so this caching saves a lot of syscalls.
	earlyTcb.selfPointer = &earlyTcb;
	earlyTcb.tid = mlibc::this_tid();
	if(mlibc::sys_tcb_set(&earlyTcb))
		__ensure(!"sys_tcb_set() failed");
	mlibc::tcb_available_flag = true;

	runtimeTlsMap.initialize();
	libraryPaths.initialize(getAllocator());
	preloads.initialize(getAllocator());

	void *phdr_pointer = 0;
	size_t phdr_entry_size = 0;
	size_t phdr_count = 0;
	void *entry_pointer = 0;
	void *stack_entropy = nullptr;

	const char *execfn = "(executable)";

#ifndef MLIBC_STATIC_BUILD
	using ctor_fn = void(*)(void);

	ctor_fn *ldso_ctors = nullptr;
	size_t num_ldso_ctors = 0;

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
		case DT_INIT_ARRAY: ldso_ctors = reinterpret_cast<ctor_fn *>(ent->d_un.d_ptr + ldso_base); break;
		case DT_INIT_ARRAYSZ: num_ldso_ctors = ent->d_un.d_val / sizeof(ctor_fn); break;
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
		case DT_REL:
		case DT_RELSZ:
		case DT_RELENT:
		case DT_RELCOUNT:
		case DT_RELR:
		case DT_RELRSZ:
		case DT_RELRENT:
		case DT_PLTGOT:
		case DT_BIND_NOW:
			continue;
		case DT_FLAGS: {
			if((ent->d_un.d_val & ~supportedDtFlags) == 0) {
				continue;
			}
			mlibc::panicLogger() << "rtld: unexpected DT_FLAGS value of " << frg::hex_fmt(ent->d_un.d_val) << " in program interpreter" << frg::endlog;
			break;
		}
		case DT_FLAGS_1: {
			if((ent->d_un.d_val & ~supportedDtFlags1) == 0) {
				continue;
			}
			mlibc::panicLogger() << "rtld: unexpected DT_FLAGS_1 value of " << frg::hex_fmt(ent->d_un.d_val) << " in program interpreter" << frg::endlog;
			break;
		}
		default:
			mlibc::panicLogger() << "rtld: unexpected dynamic entry " << ent->d_tag << " in program interpreter" << frg::endlog;
		}
	}
	__ensure(strtab_offset);
	__ensure(soname_str);

	// Find the auxiliary vector by skipping args and environment.
	auto aux = entryStack;
	aux += *aux + 1; // First, we skip argc and all args.
	__ensure(!*aux);
	aux++;
	while(*aux) { // Loop through the environment.
		auto env = reinterpret_cast<char *>(*aux);
		frg::string_view view{env};
		size_t s = view.find_first('=');

		if(s == size_t(-1))
			mlibc::panicLogger() << "rtld: environment '" << env << "' is missing a '='" << frg::endlog;

		auto name = view.sub_string(0, s);
		auto value = const_cast<char *>(view.data() + s + 1);

		if(name == "LD_SHOW_AUXV" && *value && *value != '0') {
			ldShowAuxv = true;
		}else if(name == "LD_LIBRARY_PATH" && *value) {
			for(auto path : parseList(value, ":;"))
				libraryPaths->push_back(path);
		}else if(name == "LD_PRELOAD" && *value) {
			*preloads = parseList(value, " :");
		}

		aux++;
	}
	aux++;

	for (const frg::string_view path : parseList(MLIBC_DEFAULT_LIBRARY_PATHS, "\n")) {
		libraryPaths->push_back(path);
	}

	// Parse the actual vector.
	while(true) {
		auto value = aux + 1;
		if(!(*aux))
			break;

		if(ldShowAuxv) {
			switch(*aux) {
				case AT_PHDR: mlibc::infoLogger() << "AT_PHDR: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
				case AT_PHENT: mlibc::infoLogger() << "AT_PHENT: " << *value << frg::endlog; break;
				case AT_PHNUM: mlibc::infoLogger() << "AT_PHNUM: " << *value << frg::endlog; break;
				case AT_ENTRY: mlibc::infoLogger() << "AT_ENTRY: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
				case AT_PAGESZ: mlibc::infoLogger() << "AT_PAGESZ: " << *value << frg::endlog; break;
				case AT_BASE: mlibc::infoLogger() << "AT_BASE: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
				case AT_FLAGS: mlibc::infoLogger() << "AT_FLAGS: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
				case AT_NOTELF: mlibc::infoLogger() << "AT_NOTELF: " << frg::hex_fmt{*value} << frg::endlog; break;
				case AT_UID: mlibc::infoLogger() << "AT_UID: " << *value << frg::endlog; break;
				case AT_EUID: mlibc::infoLogger() << "AT_EUID: " << *value << frg::endlog; break;
				case AT_GID: mlibc::infoLogger() << "AT_GID: " << *value << frg::endlog; break;
				case AT_EGID: mlibc::infoLogger() << "AT_EGID: " << *value << frg::endlog; break;
#ifdef AT_PLATFORM
 				case AT_PLATFORM: mlibc::infoLogger() << "AT_PLATFORM: " << reinterpret_cast<const char *>(*value) << frg::endlog; break;
#endif
#ifdef AT_HWCAP
				case AT_HWCAP: mlibc::infoLogger() << "AT_HWCAP: " << frg::hex_fmt{*value} << frg::endlog; break;
#endif
#ifdef AT_CLKTCK
				case AT_CLKTCK: mlibc::infoLogger() << "AT_CLKTCK: " << *value << frg::endlog; break;
#endif
#ifdef AT_FPUCW
				case AT_FPUCW: mlibc::infoLogger() << "AT_FPUCW: " << frg::hex_fmt{*value} << frg::endlog; break;
#endif
#ifdef AT_SECURE
 				case AT_SECURE: mlibc::infoLogger() << "AT_SECURE: " << *value << frg::endlog; break;
#endif
#ifdef AT_RANDOM
 				case AT_RANDOM: mlibc::infoLogger() << "AT_RANDOM: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
#endif
#ifdef AT_EXECFN
				case AT_EXECFN: mlibc::infoLogger() << "AT_EXECFN: " << reinterpret_cast<const char *>(*value) << frg::endlog; break;
#endif
#ifdef AT_SYSINFO_EHDR
				case AT_SYSINFO_EHDR: mlibc::infoLogger() << "AT_SYSINFO_EHDR: 0x" << frg::hex_fmt{*value} << frg::endlog; break;
#endif
			}
		}

		// TODO: Whitelist auxiliary vector entries here?
		switch(*aux) {
			case AT_PHDR: phdr_pointer = reinterpret_cast<void *>(*value); break;
			case AT_PHENT: phdr_entry_size = *value; break;
			case AT_PHNUM: phdr_count = *value; break;
			case AT_ENTRY: entry_pointer = reinterpret_cast<void *>(*value); break;
			case AT_EXECFN: execfn = reinterpret_cast<const char *>(*value); break;
			case AT_RANDOM: stack_entropy = reinterpret_cast<void*>(*value); break;
			case AT_SECURE: rtldConfig.secureRequired = reinterpret_cast<uintptr_t>(*value); break;
		}

		aux += 2;
	}
	globalDebugInterface.base = reinterpret_cast<void*>(ldso_base);

// This is here because libgcc will add a global constructor on glibc Linux
// (which is what it believes we are due to the aarch64-linux-gnu toolchain)
// in order to check if LSE atomics are supported.
//
// This is not necessary on a custom Linux toolchain and is purely an artifact of
// using the host toolchain.
#if defined(__aarch64__) && defined(__gnu_linux__)
	for (size_t i = 0; i < num_ldso_ctors; i++) {
		if(logStartup)
			mlibc::infoLogger() << "ldso: Running own constructor at "
					<< reinterpret_cast<void *>(ldso_ctors[i])
					<< frg::endlog;
		ldso_ctors[i]();
	}
#else
	if (num_ldso_ctors > 0) {
		mlibc::panicLogger() << "ldso: Found unexpected own global constructor(s), init_array starts at: "
				<< ldso_ctors
				<< frg::endlog;
	}
#endif

#else
	auto ehdr = reinterpret_cast<elf_ehdr*>(__ehdr_start);
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

	auto ldso_ehdr = reinterpret_cast<elf_ehdr *>(__ehdr_start);
	auto ldso_phdr = reinterpret_cast<elf_phdr *>(ldso_base + ldso_ehdr->e_phoff);

	ldso->phdrPointer = ldso_phdr;
	ldso->phdrCount = ldso_ehdr->e_phnum;
	ldso->phdrEntrySize = ldso_ehdr->e_phentsize;

	// TODO: support non-zero base addresses?
	executableSO = initialRepository->injectObjectFromPhdrs(execfn,
		frg::string<MemoryAllocator> { execfn, getAllocator() },
		phdr_pointer, phdr_entry_size, phdr_count, entry_pointer, 1);

	// We can't initialise the ldso object after the executable SO,
	// so we have to set the ldso path after loading both.
	ldso->path = executableSO->interpreterPath;

	// Discover dependencies in a breadth-first search.
	for (size_t i = 0; i < initialRepository->dependencyQueue.size(); i++) {
		auto current = initialRepository->dependencyQueue[i];
		initialRepository->discoverDependenciesFromLoadedObject(current);
		current->dependenciesDiscovered = true;
	}
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
	tcb->tid = earlyTcb.tid;
	if(mlibc::sys_tcb_set(tcb))
		__ensure(!"sys_tcb_set() failed");

	globalDebugInterface.ver = 1;
	globalDebugInterface.brk = &dl_debug_state;
	globalDebugInterface.state = 0;
	dl_debug_state();

	linker.initObjects(initialRepository.get());

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
const mlibc::RtldConfig &__dlapi_get_config() {
	return rtldConfig;
}

extern "C" [[ gnu::visibility("default") ]] void __dlapi_exit() {
	initialRepository->destructObjects();
}

#if __MLIBC_POSIX_OPTION

extern "C" [[ gnu::visibility("default") ]]
void *__dlapi_open(const char *file, int flags, void *returnAddress) {
	if (logDlCalls)
		mlibc::infoLogger() << "rtld: __dlapi_open(" << (file ? file : "nullptr") << ")" << frg::endlog;

	if (flags & RTLD_DEEPBIND)
		mlibc::infoLogger() << "rtld: dlopen(RTLD_DEEPBIND) is unsupported" << frg::endlog;

	if(!file)
		return executableSO;

	// TODO: Thread-safety!
	auto rts = rtsCounter++;

	SharedObject *object;
	if (flags & RTLD_NOLOAD) {
		object = initialRepository->findLoadedObject(file);
		if (object && object->globalRts == 0 && (flags & RTLD_GLOBAL)) {
			// The object was opened with RTLD_LOCAL, but we are called with RTLD_NOLOAD | RTLD_GLOBAL.
			// According to the man page, we should promote to the global scope here.
			object->globalRts = rts;
			globalScope->appendObject(object);
		}
	} else {
		bool isGlobal = flags & RTLD_GLOBAL;
		Scope *newScope = isGlobal ? globalScope.get() : nullptr;

		frg::expected<LinkerError, SharedObject *> objectResult;
		if (frg::string_view{file}.find_first('/') == size_t(-1)) {
			// In order to know which RUNPATH / RPATH to process, we must find the calling object.
			SharedObject *origin = initialRepository->findCaller(returnAddress);
			if (!origin) {
				mlibc::panicLogger() << "rtld: unable to determine calling object of dlopen "
					<< "(ra = " << returnAddress << ")" << frg::endlog;
			}

			objectResult = initialRepository->requestObjectWithName(file, origin, newScope, !isGlobal, rts);
		} else {
			objectResult = initialRepository->requestObjectAtPath(file, newScope, !isGlobal, rts);
		}

		if(!objectResult) {
			switch (objectResult.error()) {
			case LinkerError::success:
				__builtin_unreachable();
			case LinkerError::notFound:
				lastError = "Cannot locate requested DSO";
				break;
			case LinkerError::fileTooShort:
				lastError = "File too short";
				break;
			case LinkerError::notElf:
				lastError = "File is not an ELF file";
				break;
			case LinkerError::wrongElfType:
				lastError = "File has wrong ELF type";
				break;
			case LinkerError::outOfMemory:
				lastError = "Out of memory";
				break;
			case LinkerError::invalidProgramHeader:
				lastError = "File has invalid program header";
				break;
			}
			return nullptr;
		}

		object = objectResult.value();
		initialRepository->discoverDependenciesFromLoadedObject(object);
		for (size_t i = 0; i < initialRepository->dependencyQueue.size(); i++) {
			auto current = initialRepository->dependencyQueue[i];
			if(!current->dependenciesDiscovered) {
				initialRepository->discoverDependenciesFromLoadedObject(current);
				current->dependenciesDiscovered = true;
			}
		}

		Loader linker{object->localScope, nullptr, false, rts};
		linker.linkObjects(object);
		linker.initObjects(initialRepository.get());
	}

	dl_debug_state();

	return object;
}

extern "C" [[ gnu::visibility("default") ]]
void *__dlapi_resolve(void *handle, const char *string, void *returnAddress, const char *version) {
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

		mlibc::infoLogger() << "rtld: __dlapi_resolve(" << (quote ? "\"" : "") << name
			<< (quote ? "\"" : "") << ", \"" << string << "\")" << frg::endlog;
	}

	frg::optional<ObjectSymbol> target;
	frg::optional<SymbolVersion> targetVersion = frg::null_opt;

	if(version)
		targetVersion = SymbolVersion{version};

	if (handle == RTLD_DEFAULT) {
		target = globalScope->resolveSymbol(string, 0, 0, targetVersion);
	} else if (handle == RTLD_NEXT) {
		SharedObject *origin = initialRepository->findCaller(returnAddress);
		if (!origin) {
			mlibc::panicLogger() << "rtld: unable to determine calling object of dlsym "
				<< "(ra = " << returnAddress << ")" << frg::endlog;
		}

		target = Scope::resolveGlobalOrLocalNext(*globalScope, origin->localScope, string, origin, targetVersion);
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

			target = resolveInObject(current, string, targetVersion);
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
			mlibc::infoLogger() << "rtld: could not resolve \"" << string << "\"" << frg::endlog;

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
	const void *elf_symbol;
	void *link_map;
};

extern "C" [[ gnu::visibility("default") ]]
int __dlapi_reverse(const void *ptr, __dlapi_symbol *info) {
	if (logDlCalls)
		mlibc::infoLogger() << "rtld: __dlapi_reverse(" << ptr << ")" << frg::endlog;

	for(size_t i = 0; i < initialRepository->loadedObjects.size(); i++) {
		auto object = initialRepository->loadedObjects[i];

		auto eligible = [&] (ObjectSymbol cand) {
			if(cand.symbol()->st_shndx == SHN_UNDEF)
				return false;

			auto bind = ELF_ST_BIND(cand.symbol()->st_info);
			if(bind != STB_GLOBAL && bind != STB_WEAK)
				return false;

			return true;
		};

		auto hash_table = (Elf64_Word *)(object->baseAddress + object->hashTableOffset);
		auto num_symbols = hash_table[1];
		for(size_t i = 0; i < num_symbols; i++) {
			ObjectSymbol cand{object, (elf_sym *)(object->baseAddress
					+ object->symbolTableOffset + i * sizeof(elf_sym))};
			if(eligible(cand) && cand.virtualAddress() == reinterpret_cast<uintptr_t>(ptr)) {
				if (logDlCalls)
					mlibc::infoLogger() << "rtld: Found symbol " << cand.getString() << " in object "
							<< object->path << frg::endlog;

				info->file = object->path.data();
				info->base = reinterpret_cast<void *>(object->baseAddress);
				info->symbol = cand.getString();
				info->address = reinterpret_cast<void *>(cand.virtualAddress());
				info->elf_symbol = cand.symbol();
				info->link_map = &object->linkMap;
				return 0;
			}
		}
	}

	// Not found, find the DSO it should be in.
	for(size_t i = 0; i < initialRepository->loadedObjects.size(); i++) {
		auto object = initialRepository->loadedObjects[i];

		for(size_t j = 0; j < object->phdrCount; j++) {
			auto phdr = (elf_phdr *)((uintptr_t)object->phdrPointer + j * object->phdrEntrySize);
			if(phdr->p_type != PT_LOAD) {
				continue;
			}
			uintptr_t start = object->baseAddress + phdr->p_vaddr;
			uintptr_t end = start + phdr->p_memsz;
			if(reinterpret_cast<uintptr_t>(ptr) >= start && reinterpret_cast<uintptr_t>(ptr) < end) {
				mlibc::infoLogger() << "rtld: Found DSO " << object->path << frg::endlog;
				info->file = object->path.data();
				info->base = reinterpret_cast<void *>(object->baseAddress);
				info->symbol = nullptr;
				info->address = 0;
				info->elf_symbol = nullptr;
				info->link_map = &object->linkMap;
				return 0;
			}
		}
	}

	if (logDlCalls)
		mlibc::infoLogger() << "rtld: Could not find symbol in __dlapi_reverse()" << frg::endlog;

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
#if MLIBC_STATIC_BUILD
	interpreterMain(entry_stack);
#else
	(void)entry_stack;
#endif
}

#if __MLIBC_GLIBC_OPTION

extern "C" [[gnu::visibility("default")]] int _dl_find_object(void *address, dl_find_object *result) {
	for(const SharedObject *object : initialRepository->loadedObjects) {
		if(object->baseAddress > reinterpret_cast<uintptr_t>(address))
			continue;

		if(object->inLinkMap)
			result->dlfo_link_map = (link_map *)&object->linkMap;
		else
			result->dlfo_link_map = nullptr;

		uintptr_t end_addr = 0;
		for(size_t j = 0; j < object->phdrCount; j++) {
			auto phdr = (elf_phdr *)((uintptr_t)object->phdrPointer + j * object->phdrEntrySize);
			if(phdr->p_type == DLFO_EH_SEGMENT_TYPE) {
				result->dlfo_eh_frame = (void *)(object->baseAddress + phdr->p_vaddr);
				continue;
			}
			if(phdr->p_type != PT_LOAD) {
				continue;
			}
			end_addr = frg::max(end_addr, phdr->p_vaddr + phdr->p_memsz);
		}

		if(reinterpret_cast<uintptr_t>(address) > object->baseAddress + end_addr)
			continue;

		result->dlfo_flags = 0;
		result->dlfo_map_start = (void *)object->baseAddress;
		result->dlfo_map_end = (void *)(object->baseAddress + end_addr);

// TODO: fill these fields with proper values
#if DLFO_STRUCT_HAS_EH_DBASE
		mlibc::infoLogger() << "mlibc: _dl_find_object dlfo_eh_dbase is not implemented and always returns NULL" << frg::endlog;
		result->dlfo_eh_dbase = nullptr;
#endif // DLFO_STRUCT_HAS_EH_DBASE
#if DLFO_STRUCT_HAS_EH_COUNT
	mlibc::infoLogger() << "mlibc: _dl_find_object dlfo_eh_count is not implemented and always returns 0" << frg::endlog;
		result->dlfo_eh_count = 0;
#endif // DLFO_STRUCT_HAS_EH_COUNT

		return 0;
	}

	return -1;
}

#endif // __MLIBC_GLIBC_OPTION

// XXX(qookie):
// This is here because libgcc will call into __getauxval on glibc Linux
// (which is what it believes we are due to the aarch64-linux-gnu toolchain)
// in order to find AT_HWCAP to discover if LSE atomics are supported.
//
// This is not necessary on a custom Linux toolchain and is purely an artifact of
// using the host toolchain.

// __gnu_linux__ is the define checked by libgcc
#if defined(__aarch64__) && defined(__gnu_linux__) && !defined(MLIBC_STATIC_BUILD)

extern "C" unsigned long __getauxval(unsigned long type) {
	// Find the auxiliary vector by skipping args and environment.
	auto aux = entryStack;
	aux += *aux + 1; // Skip argc and all arguments
	__ensure(!*aux);
	aux++;
	while(*aux) // Now, we skip the environment.
		aux++;
	aux++;

	// Parse the auxiliary vector.
	while(true) {
		auto value = aux + 1;
		if(*aux == AT_NULL) {
			return 0;
		}else if(*aux == type) {
			return *value;
		}
		aux += 2;
	}
}

#endif
