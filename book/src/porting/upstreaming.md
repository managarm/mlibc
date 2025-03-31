# Upstreaming your port

Once your port is reasonably stable, feel free to submit it to us upstream. This ensures that your sysdeps won't be broken by any internal refactorings.

Though we won't be able to test your kernel on our CI, we require you add your port to the 'Compile sysdeps' GitHub action which checks that compilation succeeds.

It's a good idea to include a `.clang-format` file so that any changes we make to your code will be formatted to your liking.

See the [pull request adding Astral sysdeps](https://github.com/managarm/mlibc/pull/1136) for an example to follow.
