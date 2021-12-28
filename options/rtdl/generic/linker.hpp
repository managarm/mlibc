
#include <frg/hash_map.hpp>
#include <frg/optional.hpp>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/tcb.hpp>

struct ObjectRepository;
struct Scope;
struct Loader;
struct SharedObject;

extern uint64_t rtsCounter;

enum class TlsModel {
	null,
	initial,
	dynamic
};

// --------------------------------------------------------
// ObjectRepository
// --------------------------------------------------------

struct ObjectRepository {
	ObjectRepository();

	ObjectRepository(const ObjectRepository &) = delete;

	ObjectRepository &operator= (const ObjectRepository &) = delete;

	// This is primarily used to create a SharedObject for the RTDL itself.
	SharedObject *injectObjectFromDts(frg::string_view name,
			frg::string<MemoryAllocator> path,
			uintptr_t base_address, Elf64_Dyn *dynamic, uint64_t rts);

	// This is used to create a SharedObject for the executable that we want to link.
	SharedObject *injectObjectFromPhdrs(frg::string_view name,
			frg::string<MemoryAllocator> path, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
			uint64_t rts);

	SharedObject *injectStaticObject(frg::string_view name,
			frg::string<MemoryAllocator> path, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer,
			uint64_t rts);

	SharedObject *requestObjectWithName(frg::string_view name,
			SharedObject *origin, uint64_t rts);

	SharedObject *requestObjectAtPath(frg::string_view path, uint64_t rts);

private:
	void _fetchFromPhdrs(SharedObject *object, void *phdr_pointer,
			size_t phdr_entry_size, size_t num_phdrs, void *entry_pointer);

	void _fetchFromFile(SharedObject *object, int fd);

	void _parseDynamic(SharedObject *object);

	void _discoverDependencies(SharedObject *object, uint64_t rts);

	frg::hash_map<frg::string_view, SharedObject *,
			frg::hash<frg::string_view>, MemoryAllocator> _nameMap;
};

// FIXME: Do not depend on the initial universe everywhere.
extern frg::manual_box<ObjectRepository> initialRepository;

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
	Elf64_Dyn *dynv = nullptr;
	LinkMap *next = nullptr, *prev = nullptr;
};

struct SharedObject {
	// path is copied
	SharedObject(const char *name, frg::string<MemoryAllocator> path,
		bool is_main_object, uint64_t object_rts);

	SharedObject(const char *name, const char *path, bool is_main_object,
		uint64_t object_rts);

	const char *name;
	frg::string<MemoryAllocator> path;
	frg::string<MemoryAllocator> interpreterPath;
	bool isMainObject;
	uint64_t objectRts;

	// link map for debugging
	LinkMap linkMap;
	bool inLinkMap;

	// base address this shared object was loaded to
	uintptr_t baseAddress;

	Scope *loadScope;

	// pointers to the dynamic table, GOT and entry point
	Elf64_Dyn *dynamic = nullptr;
	void **globalOffsetTable;
	void *entry;

	// object initialization information
	InitFuncPtr initPtr = nullptr;
	InitFuncPtr *initArray = nullptr;
	size_t initArraySize = 0;

	// TODO: read this from the PHDR
	size_t tlsSegmentSize, tlsAlignment, tlsImageSize;
	void *tlsImagePtr;
	bool tlsInitialized;

	// symbol and string table of this shared object
	HashStyle hashStyle = HashStyle::none;
	uintptr_t hashTableOffset;
	uintptr_t symbolTableOffset;
	uintptr_t stringTableOffset;

	const char *runPath = nullptr;

	// save the lazy JUMP_SLOT relocation table
	uintptr_t lazyRelocTableOffset;
	size_t lazyTableSize;
	bool lazyExplicitAddend;

	bool symbolicResolution;
	bool eagerBinding;
	bool haveStaticTls;

	// vector of dependencies
	frg::vector<SharedObject *, MemoryAllocator> dependencies;

	TlsModel tlsModel;
	size_t tlsIndex;
	size_t tlsOffset;

	uint64_t globalRts;
	bool wasLinked;

	bool scheduledForInit;
	bool onInitStack;
	bool wasInitialized;

	Scope *objectScope;

	// PHDR related stuff, we only set these for the main executable
	void *phdrPointer = nullptr;
	size_t phdrEntrySize = 0;
	size_t phdrCount = 0;
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
void *accessDtv(SharedObject *object);
// Tries to access the DTV, if not allocated, or object doesn't have
// PT_TLS, return nullptr.
void *tryAccessDtv(SharedObject *object);

// --------------------------------------------------------
// ObjectSymbol
// --------------------------------------------------------

struct ObjectSymbol {
	ObjectSymbol(SharedObject *object, const Elf64_Sym *symbol);

	SharedObject *object() {
		return _object;
	}

	const Elf64_Sym *symbol() {
		return _symbol;
	}

	const char *getString();

	uintptr_t virtualAddress();

private:
	SharedObject *_object;
	const Elf64_Sym *_symbol;
};

frg::optional<ObjectSymbol> resolveInObject(SharedObject *object, frg::string_view string);

// --------------------------------------------------------
// Scope
// --------------------------------------------------------

struct Scope {
	using ResolveFlags = uint32_t;
	static inline constexpr ResolveFlags resolveCopy = 1;

	static frg::optional<ObjectSymbol> resolveWholeScope(Scope *scope,
			frg::string_view string, ResolveFlags flags);

	Scope();

	void appendObject(SharedObject *object);

	frg::optional<ObjectSymbol> resolveSymbol(ObjectSymbol r, ResolveFlags flags);
private:
public: // TODO: Make this private again. (Was made public for __dlapi_reverse()).
	frg::vector<SharedObject *, MemoryAllocator> _objects;
};

// --------------------------------------------------------
// Loader
// --------------------------------------------------------

class Loader {
public:
	Loader(Scope *scope, bool is_initial_link, uint64_t rts);

	void submitObject(SharedObject *object);

public:
	void linkObjects();

private:
	void _buildTlsMaps();

	void _processStaticRelocations(SharedObject *object);
	void _processLazyRelocations(SharedObject *object);
	void _processRela(SharedObject *object, Elf64_Rela *reloc);

public:
	void initObjects();

private:
	void _scheduleInit(SharedObject *object);

private:
	struct Token { };

	Scope *_globalScope;
	bool _isInitialLink;
	uint64_t _linkRts;

	frg::hash_map<SharedObject *, Token,
			frg::hash<SharedObject *>, MemoryAllocator> _linkSet;

	// Stores the same objects as _linkSet but in dependency-BFS order.
	frg::vector<SharedObject *, MemoryAllocator> _linkBfs;

	frg::vector<SharedObject *, MemoryAllocator> _initQueue;
};

// --------------------------------------------------------
// Namespace scope functions
// --------------------------------------------------------

extern "C" void pltRelocateStub() __attribute__ (( visibility("hidden") ));

// --------------------------------------------------------
// RTDL interface
// --------------------------------------------------------

void *rtdl_auxvector();

