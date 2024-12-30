#pragma once

#define BIT(n) (1UL<<(n))

typedef enum vma_flags
{
    VMA_FLAGS_HUGE_PAGE = BIT(0),
    VMA_FLAGS_GUARD_PAGE = BIT(2),
    VMA_FLAGS_32BIT = BIT(3),
    VMA_FLAGS_HINT = BIT(4),
    VMA_FLAGS_NON_PAGED = BIT(5),
	VMA_FLAGS_PRIVATE = BIT(6), // only applies when mapping a file.
	VMA_FLAGS_PREFAULT = BIT(7), // only applies when mapping a file.
	VMA_FLAGS_RESERVE = BIT(8), // Registers the pages, but does not back them by anything. If this is set, the VMA ignores the 'file' parameter.
//	VMA_FLAGS_32BITPHYS = BIT(9), // 32-bit physical addresses should be allocated. Best to use with VMA_FLAGS_NON_PAGED. Ignored if file != nullptr.
	VMA_FLAGS_KERNEL_STACK = VMA_FLAGS_NON_PAGED|VMA_FLAGS_GUARD_PAGE,
} vma_flags;
typedef enum prot_flags
{
	/// <summary>
	/// Allocates the pages as read-only.
	/// </summary>
	OBOS_PROTECTION_READ_ONLY = 0x1,
	/// <summary>
	/// Allows execution on the pages. Might not be supported on some architectures.
	/// </summary>
	OBOS_PROTECTION_EXECUTABLE = 0x2,
	/// <summary>
	/// Allows user-mode threads to read the allocated pages. Note: On some architectures, in some configurations, this might page fault in kernel-mode.
	/// </summary>
	OBOS_PROTECTION_USER_PAGE = 0x4,
	/// <summary>
	/// Disables cache on the pages. Should not be allowed for most user programs.
	/// </summary>
	OBOS_PROTECTION_CACHE_DISABLE = 0x8,
	/// <summary>
	/// For Mm_VirtualMemoryProtect. Sets the protection to the same thing it was before.</br>
	/// If other protection bits are set, said protection bit is overrided in the page.
	/// </summary>
	OBOS_PROTECTION_SAME_AS_BEFORE = 0x10,
	/// <summary>
	/// Enables cache on the pages. This is the default.</br>
	/// Overrided by OBOS_PROTECTION_CACHE_DISABLE.
	/// </summary>
	OBOS_PROTECTION_CACHE_ENABLE = 0x20,
	/// <summary>
	/// Bits from here to OBOS_PROTECTION_PLATFORM_END are reserved for the architecture.
	/// </summary>
	OBOS_PROTECTION_PLATFORM_START = 0x01000000,
	OBOS_PROTECTION_PLATFORM_END = 0x80000000,
} prot_flags;

#undef BIT
