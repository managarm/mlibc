# Contributing to mlibc

We are happy to take contributions to mlibc! This page aims to summarize the process to help make the process smoother.

## Committing changes

When making your changes, ensure that they are separated out into sensible commits. Everyy commit should be self-contained, only cover one logical unit of change, and be buildable (commonly referred to as `atomic commits`). Make sure that commit messages appropriately describe the changes made. Feel free to use the commit body to explain the changes more in-depth.

### Special case: ABI breaks

In some unfortunate circumstances, it might be necessary to break the public ABI of mlibc. In such cases, split the ABI-breaking changes out into their own PR. Commits messages should be prefixed with `[ABI BREAK]`, and you should add a line to `ABI_BREAKS.md` about the breaking change.

## Test your changes

You should, at a minimum, run the testsuite to ensure that your changes do not obviously break existing code.

```sh
meson setup -Dbuild_tests=true -Dlinux_kernel_headers=path/to/kernel/headers/for/your/arch -Ddefault_library=shared --buildtype=debug build .
meson test -C build
```

Additionally, your changes should come with tests for new functionality or fixed regressions.

## Open a PR

Once you have completed your changes, the next step is to open a PR. For this, please use a feature branch (don't open PRs from your fork's `master` branch) against mlibc's `master` branch.

### Code review

Every PR needs to get reviewed before it gets merged. This starts with the submitter, who should have thought through their changes and verified their correctness. Once the PR is submitted, one of the maintainers also reviews the code. Don't be surprised if there are a few comments and a bit of back-and-forth though - this is just what a review process looks like.

### CI

Every PR needs to run the complete testsuite, which happens on GitHub Actions. The CI runs the testsuite in many configurations: different architectures (x86_64, aarch64, riscv64, ...), different compilers (gcc and clang), different mlibc configurations (ansi-only, static, shared, ...) and for different sysdeps.
