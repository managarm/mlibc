# Release Procedure

mlibc uses [semantic versioning v2.0.0](https://semver.org/spec/v2.0.0.html) to denote releases.

In short:
- A bump in the major version signals an ABI or API break, or otherwise any other major change
involving compatibility breakage.
- A bump in the minor version signals a newly released set of features, while still maintaining
compatibility.
- A bump in the patch signals a bug-fix, while not adding new features.

The `master` branch is where active development occurs, and where PRs get merged into. Changes
that are deemed appropriate to be backported to a major version branch (including the current,
latest major version branch) are cherry-picked there.

Releases are tagged off of their respective major version branches once they are ready.

Major versions get a new, dedicated branch, which is created from `master` when deemed
necessary, usually shortly before the tagging of the first release in the new major version
branch. This branch is called `vN.x`, where `N` is the number of the major version
(and `x` is just a literal "x").

Submodules are fixed to specific commits when a major version branch is created.

The first tag on the major version branch is the `vN.0.0` release.

Minor versions and patch versions are tagged on their respective major version branches
after the necessary commits are backported onto said branch from `master`.

## ABI_BREAKS.md

The `ABI_BREAKS.md` file should be updated in `master` alongside any newly merged commit that
does break ABI, in order to keep track of these changes.
