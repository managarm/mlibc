#! /bin/sh

set -ex

LINUX_MAJOR=v6.x
LINUX_VERSION=6.13.8
: "${ARCH:=$(uname -m)}"

curl -Lo linux-$LINUX_VERSION.tar.xz https://cdn.kernel.org/pub/linux/kernel/$LINUX_MAJOR/linux-$LINUX_VERSION.tar.xz
tar -xf linux-$LINUX_VERSION.tar.xz
rm linux-$LINUX_VERSION.tar.xz
cd linux-$LINUX_VERSION
make mrproper
make ARCH=$ARCH headers_install
find usr/include -type f ! -name '*.h' -delete
mv usr/include ../linux-headers
cd ..
rm -rf linux-$LINUX_VERSION
