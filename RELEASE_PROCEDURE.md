# Versioning

mlibc uses semantic versioning to denote releases.
A bump in the major version signals an ABI break.
A bump in the minor version signals a newly released set of features, while still maintaining ABI.
A bump in the patch signals a bug-fix, while not adding new features.

# Release schedule

A minor or major version will be released every two months (every even month). A release is turned into a major one
when there is a pending ABI break, given that the ABI hasn't been broken in the last 6 months.
A patch version will be released on-demand, as bugs are fixed.

When a new minor or major version is to be released, first a release candiate ("rc") will
be released. This release candiate will become the definite release after a week. During
this time, end-users shall test the rc and determine if there are breaking changes. Several
rc's can be released (``rc-1``, ``rc-2``, etc.). Once the rc is confirmed to be in a stable
state, the final version is to be released.
