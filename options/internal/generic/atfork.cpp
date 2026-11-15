#include <errno.h>

#include <mlibc/allocator.hpp>
#include <mlibc/atfork.hpp>
#include <mlibc/lock.hpp>

namespace mlibc {

namespace {

struct atfork_handler {
	void (*prepare)(void);
	void (*parent)(void);
	void (*child)(void);

	atfork_handler *next;
	atfork_handler *prev;
};

atfork_handler *handler_begin = nullptr;
atfork_handler *handler_end = nullptr;

// Held from the first prepare handler until the last parent or child one, so
// that a concurrent pthread_atfork() cannot change the list halfway through.
constinit FutexLock handler_lock{};

} // namespace

int atfork_register(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
	auto hand = frg::construct<atfork_handler>(getAllocator());
	if (!hand)
		return ENOMEM;

	hand->prepare = prepare;
	hand->parent = parent;
	hand->child = child;
	hand->next = nullptr;

	handler_lock.lock();

	hand->prev = handler_end;
	if (handler_end)
		handler_end->next = hand;
	handler_end = hand;

	if (!handler_begin)
		handler_begin = hand;

	handler_lock.unlock();

	return 0;
}

void atfork_prepare(void) {
	handler_lock.lock();

	// POSIX runs the prepare handlers in the reverse order of registration.
	for (auto hand = handler_end; hand; hand = hand->prev)
		if (hand->prepare)
			hand->prepare();
}

void atfork_parent(void) {
	for (auto hand = handler_begin; hand; hand = hand->next)
		if (hand->parent)
			hand->parent();

	handler_lock.unlock();
}

void atfork_child(void) {
	for (auto hand = handler_begin; hand; hand = hand->next)
		if (hand->child)
			hand->child();

	handler_lock.reset_after_fork();
}

} // namespace mlibc
