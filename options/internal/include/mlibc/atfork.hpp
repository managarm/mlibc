#pragma once

namespace mlibc {

int atfork_register(void (*prepare)(void), void (*parent)(void), void (*child)(void));

// Run the prepare handlers and take the lock that keeps the list from changing
// for the duration of the fork. Every call must be followed by exactly one call
// to atfork_parent() or atfork_child(), including when the fork itself failed.
void atfork_prepare(void);
void atfork_parent(void);
void atfork_child(void);

} // namespace mlibc
