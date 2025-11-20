# Adding a new OS port

Adding a new OS port to mlibc is a straightforward process, as it has been designed with portability in mind.

First, make sure your kernel has the [prerequisites](kernel_prerequisites.md) needed. Then, [choose the compiler](choosing_a_compiler.md) which will be used to compile mlibc and your user applications. After that, [implement the sysdeps](implementing_sysdeps.md) for your OS. With that done, you will have mlibc working under your OS! You can then take a look at [what to do next](next_steps.md).
