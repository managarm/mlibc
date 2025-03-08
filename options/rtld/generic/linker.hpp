
#include <frg/hash_map.hpp>
#include <frg/optional.hpp>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <frg/stack.hpp>
#include <frg/expected.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/tcb.hpp>

#include "elf.hpp"

struct ObjectRepository;
struct Scope;
struct Loader;
struct SharedObject;
struct ObjectSymbol;
struct SymbolVersion;

extern uint64_t rtsCounter;

enum class TlsModel {
	null,
	initial,
	dynamic
};

enum class LinkerError {
	success,
	notFound,
	fileTooShort,
	notElf,
	wrongElfType,
	outOfMemory,
	invalidProgramHeader
};

uint32_t elf64Hash(frg::string_view string);

// --------------------------------------------------------
// ObjectRepository
// --------------------------------------------------------

struct ObjectRepository {
	ObjectRepository();

	ObjectRepository(const ObjectRepository &) = delete;

	ObjectRepository &operator= (const ObjectRepository &) = delete;

	// This is primarily used to create a SharedObject for the RTLD itself.
	SharedObject *injectObjectFromDts(frg::string_view name,
			frg::string<MemoryAllocator> path,
			uintptr_t base_address, elf_dyn *dynamic, uint64_t rts);

	// This is used to create a SharedObject for the executable that we want to link.
	SharedObject *injectObjectFromPhdrs(frg::string_view name,
			frg::string<MemoryAllocator> path, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
			uint64_t rts);

	SharedObject *injectStaticObject(frg::string_view name,
			frg::string<MemoryAllocator> path, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
			uint64_t rts);

	frg::expected<LinkerError, SharedObject *> requestObjectWithName(frg::string_view name,
			SharedObject *origin, Scope *localScope, bool createScope, uint64_t rts);

	frg::expected<LinkerError, SharedObject *> requestObjectAtPath(frg::string_view path,
			Scope *localScope, bool createScope, uint64_t rts);

	void discoverDependenciesFromLoadedObject(SharedObject *object);

	SharedObject *findCaller(void *address);

	SharedObject *findLoadedObject(frg::string_view name);

	void addObjectToDestructQueue(SharedObject *object);
	void destructObjects();

	// Used by dl_iterate_phdr: stores objects in the order they are loaded.
	frg::vector<SharedObject *, MemoryAllocator> loadedObjects;

	// Used for breadth-first searching dependencies.
	frg::vector<SharedObject *, MemoryAllocator> dependencyQueue;

private:
	void _fetchFromPhdrs(SharedObject *object, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer);

	frg::expected<LinkerError, void> _fetchFromFile(SharedObject *object, int fd);

	void _parseDynamic(SharedObject *object);

	void _parseVerdef(SharedObject *object);
	void _parseVerneed(SharedObject *object);

	void _discoverDependencies(SharedObject *object, Scope *localScope, uint64_t rts);

	void _addLoadedObject(SharedObject *object);

	frg::hash_map<frg::string_view, SharedObject *,
			frg::hash<frg::string_view>, MemoryAllocator> _nameMap;

	// Used for destructing the objects, stores all the objects in the order they are initialized.
	frg::stack<SharedObject *, MemoryAllocator> _destructQueue;
};

// --------------------------------------------------------
// SharedObject
// --------------------------------------------------------

enum class HashStyle {
	none,
	systemV,
	gnu
};

using InitFuncPtr = void (*)();

// The ABI of this struct is fixed by GDB
struct DebugInterface {
	int ver;
	void *head;
	void (*brk)(void);
	int state;
	void *base;
};

// The ABI of this struct is fixed by GDB
struct LinkMap {
	uintptr_t base = 0;
	const char *name = nullptr;
	elf_dyn *dynv = nullptr;
	LinkMap *next = nullptr, *prev = nullptr;
};

struct SharedObject {
	// path is copied
	SharedObject(const char *name, frg::string<MemoryAllocator> path,
		bool is_main_object, Scope *localScope, uint64_t object_rts);

	SharedObject(const char *name, const char *path, bool is_main_object,
		Scope *localScope, uint64_t object_rts);

	frg::string<MemoryAllocator> name;
	frg::string<MemoryAllocator> path;
	frg::string<MemoryAllocator> interpreterPath;
	const char *soName;
	bool isMainObject;
	uint64_t objectRts;

	// link map for debugging
	LinkMap linkMap;
	bool inLinkMap;

	// base address this shared object was loaded to
	uintptr_t baseAddress;

	Scope *localScope;

	// pointers to the dynamic table, GOT and entry point
	elf_dyn *dynamic = nullptr;
	void **globalOffsetTable;
	void *entry;

	// object initialization information
	InitFuncPtr initPtr = nullptr;
	InitFuncPtr finiPtr = nullptr;
	InitFuncPtr *initArray = nullptr;
	InitFuncPtr *finiArray = nullptr;
	InitFuncPtr *preInitArray = nullptr;
	size_t initArraySize = 0;
	size_t finiArraySize = 0;
	size_t preInitArraySize = 0;


	// TODO: read this from the PHDR
	size_t tlsSegmentSize, tlsAlignment, tlsImageSize;
	void *tlsImagePtr;
	bool tlsInitialized;

	// symbol and string table of this shared object
	HashStyle hashStyle = HashStyle::none;
	uintptr_t hashTableOffset;
	uintptr_t symbolTableOffset;
	uintptr_t stringTableOffset;

	// Version tables of this shared object
	uintptr_t versionTableOffset = 0;
	uintptr_t versionDefinitionTableOffset = 0;
	size_t versionDefinitionCount = 0;
	uintptr_t versionRequirementTableOffset = 0;
	size_t versionRequirementCount = 0;

	// Versions we know about for this object's VERSYM.
	frg::hash_map<
		elf_version,
		SymbolVersion,
		frg::hash<unsigned int>,
		MemoryAllocator
	> knownVersions;
	// Versions that this object defines.
	frg::vector<SymbolVersion, MemoryAllocator> definedVersions;

	const char *runPath = nullptr;

	// save the lazy JUMP_SLOT relocation table
	uintptr_t lazyRelocTableOffset;
	size_t lazyTableSize;
	frg::optional<bool> lazyExplicitAddend;

	bool symbolicResolution;
	bool eagerBinding;
	bool haveStaticTls;

	// vector of dependencies
	frg::vector<SharedObject *, MemoryAllocator> dependencies;

	TlsModel tlsModel;
	size_t tlsIndex;
	ssize_t tlsOffset;

	uint64_t globalRts;
	bool wasLinked;

	bool scheduledForInit;
	bool onInitStack;
	bool wasInitialized;

	bool wasDestroyed = false;

	bool wasVisited = false;

	bool dependenciesDiscovered = false;

	// PHDR related stuff, we only set these for the main executable
	void *phdrPointer = nullptr;
	size_t phdrEntrySize = 0;
	size_t phdrCount = 0;

	frg::tuple<ObjectSymbol, SymbolVersion> getSymbolByIndex(size_t index);
};

struct Relocation {
	Relocation(SharedObject *object, elf_rela *r)
	: object_{object}, type_{Addend::Explicit} {
		offset_ = r->r_offset;
		info_ = r->r_info;
		addend_ = r->r_addend;
	}

	Relocation(SharedObject *object, elf_rel *r)
	: object_{object}, type_{Addend::Implicit} {
		offset_ = r->r_offset;
		info_ = r->r_info;
	}

	SharedObject *object() {
		return object_;
	}

	elf_info type() const {
		return ELF_R_TYPE(info_);
	}

	elf_info symbol_index() const {
		return ELF_R_SYM(info_);
	}

	elf_addr addend_rel() {
		switch(type_) {
			case Addend::Explicit:
				return addend_;
			case Addend::Implicit: {
				auto ptr = reinterpret_cast<elf_addr *>(object_->baseAddress + offset_);
				return *ptr;
			}
		}
		__builtin_unreachable();
	}

	elf_addr addend_norel() {
		switch(type_) {
			case Addend::Explicit:
				return addend_;
			case Addend::Implicit:
				return 0;
		}
		__builtin_unreachable();
	}

	void *destination() {
		return reinterpret_cast<void *>(object_->baseAddress + offset_);
	}

	void relocate(elf_addr addr) {
		auto ptr = destination();
		memcpy(ptr, &addr, sizeof(addr));
	}

private:
	enum class Addend {
		Implicit,
		Explicit
	};

	SharedObject *object_;
	Addend type_;

	elf_addr offset_;
	elf_info info_;
	elf_addend addend_ = 0;
};

void processCopyRelocations(SharedObject *object);

// --------------------------------------------------------
// RuntimeTlsMap
// --------------------------------------------------------

struct RuntimeTlsMap {
	RuntimeTlsMap();

	// Amount of initialLimit that has already been allocated.
	size_t initialPtr;

	// Size of the inital TLS segment.
	size_t initialLimit;

	// TLS indices.
	frg::vector<SharedObject *, MemoryAllocator> indices;
};

extern frg::manual_box<RuntimeTlsMap> runtimeTlsMap;

Tcb *allocateTcb();
void initTlsObjects(Tcb *tcb, const frg::vector<SharedObject *, MemoryAllocator> &objects, bool checkInitialized);
void *accessDtv(SharedObject *object);
// Tries to access the DTV, if not allocated, or object doesn't have
// PT_TLS, return nullptr.
void *tryAccessDtv(SharedObject *object);

// --------------------------------------------------------
// ObjectSymbol
// --------------------------------------------------------

struct ObjectSymbol {
	ObjectSymbol(SharedObject *object, const elf_sym *symbol);

	SharedObject *object() {
		return _object;
	}

	const elf_sym *symbol() {
		return _symbol;
	}

	const char *getString();

	uintptr_t virtualAddress();

private:
	SharedObject *_object;
	const elf_sym *_symbol;
};

frg::optional<ObjectSymbol> resolveInObject(SharedObject *object, frg::string_view string,
		frg::optional<SymbolVersion> version);

// --------------------------------------------------------
// SymbolVersion
// --------------------------------------------------------

struct SymbolVersion {
	SymbolVersion(const char *name, uint32_t hash)
	: _local{false}, _global{false}, _default{false}
	, _name{name}, _hash{hash} { }

	SymbolVersion(int idx)
	: _local{idx == 0}, _global{idx == 1}, _default{false}
	, _name{""}, _hash{0} { }

	SymbolVersion(const char *name)
	: _local{false}, _global{false}, _default{false}
	, _name{name}, _hash{elf64Hash(name)} { }

	bool isLocal() const {
		return _local;
	}

	bool isGlobal() const {
		return _global;
	}

	bool isDefault() const {
		return _default;
	}

	frg::string_view name() const {
		if(_local) return "(*local*)";
		if(_global) return "(*global*)";
		return _name;
	}

	uint32_t hash() const {
		return _hash;
	}

	bool operator==(const SymbolVersion &other) const {
		if(_local || other._local) return _local && other._local;
		if(_global || other._global) return _global && other._global;
		if(_hash != other._hash) return false;
		return _name == other._name;
	}

	SymbolVersion makeDefault() const {
		auto copy = *this;
		copy._default = true;

		return copy;
	}

private:
	bool _local, _global;
	bool _default;

	frg::string_view _name;
	uint32_t _hash;
};


// --------------------------------------------------------
// Scope
// --------------------------------------------------------

struct Scope {
	using ResolveFlags = uint32_t;
	static inline constexpr ResolveFlags resolveCopy = 1;
	static inline constexpr ResolveFlags skipGlobalAfterRts = 1 << 1;

	static frg::optional<ObjectSymbol> resolveGlobalOrLocal(Scope &globalScope,
			Scope *localScope, frg::string_view string, uint64_t skipRts, ResolveFlags flags,
			frg::optional<SymbolVersion> version);
	static frg::optional<ObjectSymbol> resolveGlobalOrLocalNext(Scope &globalScope,
			Scope *localScope, frg::string_view string, SharedObject *origin,
			frg::optional<SymbolVersion> version);

	Scope(bool isGlobal = false);

	void appendObject(SharedObject *object);

	frg::optional<ObjectSymbol> resolveSymbol(frg::string_view string, uint64_t skipRts, ResolveFlags flags,
			frg::optional<SymbolVersion> version);

	bool isGlobal;

private:
	frg::optional<ObjectSymbol> _resolveNext(frg::string_view string, SharedObject *target,
			frg::optional<SymbolVersion> version);
public: // TODO: Make this private again. (Was made public for __dlapi_reverse()).
	frg::vector<SharedObject *, MemoryAllocator> _objects;
};

extern frg::manual_box<Scope> globalScope;

// --------------------------------------------------------
// Loader
// --------------------------------------------------------

struct Loader {
public:
	Loader(Scope *scope, SharedObject *mainExecutable, bool is_initial_link, uint64_t rts);

public:
	void linkObjects(SharedObject *root);

private:
	void _buildLinkBfs(SharedObject *root);
	void _buildTlsMaps();

	void _processStaticRelocations(SharedObject *object);
	void _processLazyRelocations(SharedObject *object);

	void _processRelocations(Relocation &rel);

public:
	void initObjects(ObjectRepository *repository);

private:
	void _scheduleInit(SharedObject *object);

private:
	SharedObject *_mainExecutable;
	Scope *_loadScope;
	bool _isInitialLink;
	uint64_t _linkRts;

	frg::vector<SharedObject *, MemoryAllocator> _linkBfs;

	frg::vector<SharedObject *, MemoryAllocator> _initQueue;
};

// --------------------------------------------------------
// Namespace scope functions
// --------------------------------------------------------

extern "C" void pltRelocateStub() __attribute__((__visibility__("hidden")));

// --------------------------------------------------------
// RTLD interface
// --------------------------------------------------------

void *rtld_auxvector();

