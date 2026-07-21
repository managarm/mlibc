#pragma once

#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/thread-types.hpp>
#include <spawn.h>

struct __mlibc_spawnattr {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, posix_spawnattr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_spawnattr, __mlibc_spawnattr>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	int __flags;
	pid_t __pgrp;
	sigset_t __def;
	sigset_t __mask;
	int __prio;
	int __pol;
	struct sched_param __schedparam;
	int __schedpolicy;
};

struct __mlibc_spawn_file_actions {
	static __mlibc_spawn_file_actions *from(posix_spawn_file_actions_t *attr) {
		return std::launder(reinterpret_cast<__mlibc_spawn_file_actions *>(attr->__heap_ptr));
	}

	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, posix_spawn_file_actions_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_spawn_file_actions, __mlibc_spawn_file_actions>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	struct fdop {
		int cmd;
		int fd;
		int srcfd = -1;
		int oflag = 0;
		mode_t mode = 0;
		frg::string<MemoryAllocator> path{getAllocator()};
	};

	frg::vector<fdop, MemoryAllocator> ops{getAllocator()};
};
