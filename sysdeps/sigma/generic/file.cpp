#include <mlibc/sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/sigma-posix.hpp>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>

#include <libsigma/memory.h>
#include <libsigma/thread.h>
#include <libsigma/ipc.h>
#include <libsigma/klog.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {
    // File functions
    int sys_open(const char *path, int flags, int *fd){
        SignalGuard sguard{};
        size_t path_size = strlen(path);

        auto& allocator = getSysdepsAllocator();

        size_t req_size = sizeof(libsigma_open_message) + path_size;
        libsigma_open_message* req = static_cast<libsigma_open_message*>(allocator.allocate(req_size));

        req->command = OPEN;
        req->msg_id = 0;
        req->flags = flags;
        memcpy(req->path, path, path_size);
        req->path_len = path_size;

        libsigma_ipc_set_message_checksum(req->msg(), sizeof(libsigma_open_message) + path_size);

        if(libsigma_ipc_send(libsigma_get_um_tid(), sizeof(libsigma_open_message) + path_size, req->data()) == 1){
            libsigma_klog("Failed to send open");
            allocator.free(static_cast<void*>(req));
            return -1;
        }

        allocator.free(static_cast<void*>(req));

        while(libsigma_ipc_get_msg_size() == 0); // No response yet

        size_t sz = libsigma_ipc_get_msg_size();
        libsigma_ret_message* ret = static_cast<libsigma_ret_message*>(allocator.allocate(sz));
        uint64_t origin, useless;
        if(libsigma_ipc_receive(&origin, &useless, ret->data()) == 1){
            libsigma_klog("Failed to receive return message");
            allocator.free(static_cast<void*>(ret));
            return -1;
        }
            

        if(!libsigma_ipc_check_message(ret->msg(), sz)){
            libsigma_klog("Invalid return message checksum");
            allocator.free(static_cast<void*>(ret));
            return -1;
        } 

        if(origin != libsigma_get_um_tid()){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Didn't receive return message from correct thread");
            return -1;
        } 

        if(ret->msg_id != 0){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Return message didn't have correct message id");
            return -1;
        } 

        *fd = ret->ret;
        allocator.free(static_cast<void*>(ret));
        return 0;
    }
    
    int sys_dup2(int fd, int flags, int newfd){
        SignalGuard sguard{};
        auto& allocator = getSysdepsAllocator();
        auto* req = reinterpret_cast<libsigma_dup2_message*>(allocator.allocate(sizeof(libsigma_dup2_message)));

        req->command = DUP2;
        req->msg_id = 0;
        req->oldfd = fd;
        req->newfd = newfd;

        libsigma_ipc_set_message_checksum(req->msg(), sizeof(libsigma_dup2_message));

        if(libsigma_ipc_send(libsigma_get_um_tid(), sizeof(libsigma_dup2_message), req->data()) == 1){
            libsigma_klog("Failed to send dup2 message");
            allocator.free(static_cast<void*>(req));
            return -1;
        }

        while(libsigma_ipc_get_msg_size() == 0); // No response yet

        size_t sz = libsigma_ipc_get_msg_size();
        auto* ret = reinterpret_cast<libsigma_ret_message*>(allocator.allocate(sz));
        uint64_t origin, useless;
        if(libsigma_ipc_receive(&origin, &useless, ret->data()) == 1){
            libsigma_klog("Failed to receive return msg");
            allocator.free(static_cast<void*>(ret));
            return -1;
        }

        if(!libsigma_ipc_check_message(ret->msg(), sz)){
            libsigma_klog("Invalid message checksum");
            allocator.free(static_cast<void*>(ret));
            return -1;
        } 

        if(origin != libsigma_get_um_tid()){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Didn't receive return message from correct thread");
            return -1;
        } 

        if(ret->msg_id != 0){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Return message didn't have correct message id");
            return -1;
        } 

        int real_ret = ret->ret;
        allocator.free(static_cast<void*>(ret));
        return real_ret;
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        SignalGuard sguard{};
        auto& allocator = getSysdepsAllocator();
        
        auto* req = reinterpret_cast<libsigma_write_message*>(allocator.allocate(count + sizeof(libsigma_write_message)));

        req->command = WRITE;
        req->msg_id = 0;
        req->count = count;
        req->fd = fd;
        memcpy(req->buf, buf, count);

        libsigma_ipc_set_message_checksum(req->msg(), sizeof(libsigma_write_message) + count);

        if(libsigma_ipc_send(libsigma_get_um_tid(), sizeof(libsigma_write_message) + count, req->data()) == 1){
            libsigma_klog("Failed to send write message");
            allocator.free(static_cast<void*>(req));
            return -1;
        }

        while(libsigma_ipc_get_msg_size() == 0); // No response yet

        size_t sz = libsigma_ipc_get_msg_size();
        libsigma_ret_message* ret = reinterpret_cast<libsigma_ret_message*>(allocator.allocate(sz));
        uint64_t origin, useless;
        if(libsigma_ipc_receive(&origin, &useless, ret->data()) == 1){
            libsigma_klog("Failed to receive return message");
            allocator.free(static_cast<void*>(ret));
            return -1;
        }
            

        if(!libsigma_ipc_check_message(ret->msg(), sz)){
            libsigma_klog("Invalid return message checksum");
            allocator.free(static_cast<void*>(ret));
            return -1;
        } 

        if(origin != libsigma_get_um_tid()){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Didn't receive return message from correct thread");
            return -1;
        } 

        if(ret->msg_id != 0){
            allocator.free(static_cast<void*>(ret));
            libsigma_klog("Return message didn't have correct message id");
            return -1;
        } 

        int real_ret = ret->ret;
        allocator.free(static_cast<void*>(ret));
        *bytes_written = count;
        return 0;
    }

    int sys_close(int fd){
        return 0;
    }
    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        return ESPIPE;
    }
    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) STUB_ONLY

    int sys_isatty(int fd){
        return 0;
    }

    /*
    int sys_ioctl(int fd, unsigned long request, void *arg, int *result) STUB_ONLY
    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) STUB_ONLY
    int sys_rename(const char *path, const char *new_path) STUB_ONLY
    int sys_open_dir(const char *path, int *handle) STUB_ONLY
    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) STUB_ONLY
    int sys_access(const char *path, int mode) STUB_ONLY
    int sys_dup(int fd, int flags, int *newfd) STUB_ONLY
    
    
    int sys_ttyname(int fd, char *buf, size_t size) STUB_ONLY
    int sys_chroot(const char *path) STUB_ONLY
    int sys_mkdir(const char *path) STUB_ONLY
    int sys_tcgetattr(int fd, struct termios *attr) STUB_ONLY
    int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) STUB_ONLY
    int sys_tcflow(int fd, int action) STUB_ONLY
    int sys_pipe(int *fds, int flags) STUB_ONLY
    int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) STUB_ONLY
    int sys_ftruncate(int fd, size_t size) STUB_ONLY
    int sys_fallocate(int fd, off_t offset, size_t size) STUB_ONLY
    int sys_unlink(const char *path) STUB_ONLY
    int sys_symlink(const char *target_path, const char *link_path) STUB_ONLY
    int sys_fcntl(int fd, int request, va_list args, int *result) STUB_ONLY
    #endif*/
}