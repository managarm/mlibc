#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{
    int sys_open(const char * /* pathname */, int /* flags */, mode_t /* mode */, int * /* fd */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
    int sys_read(int /* fd */, void * /* buf */, size_t /* count */, ssize_t * /* bytes_read */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
    int sys_write(int /* fd */, const void * /* data */, size_t /* size */, ssize_t * /* bytes_written */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
    int sys_seek(int /* fd */, off_t /* offset */, int /* whence */, off_t * /* new_offset */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
    int sys_close(int /* fd */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    [[gnu::weak]] int sys_stat(fsfd_target /* fsfdt */, int /* fd */, const char * /* path */, int /* flags */,
                               struct stat * /* statbuf */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_vm_map(void * /* hint */, size_t /* size */, int /* prot */, int /* flags */, int /* fd */, off_t /* offset */, void ** /* window */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
} // namespace mlibc