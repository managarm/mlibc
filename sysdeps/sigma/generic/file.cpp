#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/sigma-posix.hpp>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>

#include <libsigma/sys.h>

#define IOTA_FRIGG_ALLOCATOR MemoryAllocator
#define IOTA_FRIGG_GET_ALLOCATOR getSysdepsAllocator
#include <protocols/zeta-frigg.hpp>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {
    // File functions
    static uint64_t tell(int fd){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Tell);
        builder.add_fd(fd);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Tell message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Tell message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_offset()){
            mlibc::infoLogger() << "Tell message response has no offset" << frg::endlog;
            return -1;
        }

        return parser.get_offset();
    }

    int sys_open(const char *path, int flags, int *fd){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Open);
        builder.add_path(iota::string{getSysdepsAllocator(), path});
        builder.add_flags(flags);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Open message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Open message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_fd()){
            mlibc::infoLogger() << "Open message response has no fd" << frg::endlog;
            return -1;
        }
        *fd = parser.get_fd();
        return 0;
    }

    int sys_dup2(int fd, int flags, int newfd){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Dup2);
        builder.add_fd(fd);
        builder.add_newfd(newfd);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Dup2 message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Dup2 message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_status()){
            mlibc::infoLogger() << "Dup2 message response has no status" << frg::endlog;
            return -1;
        }

        return parser.get_status();
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Write);
        builder.add_fd(fd);
        builder.add_count(count);

        frg::vector<uint8_t, MemoryAllocator> buffer{getSysdepsAllocator()};
        buffer.resize(count);
        memcpy(buffer.data(), buf, count);

        builder.add_buffer(buffer);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Write message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Write message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_status()){
            mlibc::infoLogger() << "Write message response has no status" << frg::endlog;
            return -1;
        }

        if(parser.get_status() == 0){
            *bytes_written = count;
            return 0;
        } else {
            return parser.get_status();
        }
    }

    int sys_close(int fd){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Close);
        builder.add_fd(fd);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Close message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Close message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_status()){
            mlibc::infoLogger() << "Close message response has no Status" << frg::endlog;
            return -1;
        }
        return parser.get_status();
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Read);
        builder.add_fd(fd);
        builder.add_count(count);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Read message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Read message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_status()){
            mlibc::infoLogger() << "Read message response has no Status" << frg::endlog;
            return -1;
        }

        if(!parser.has_buffer()){
            mlibc::infoLogger() << "Read message response has no Buffer" << frg::endlog;
            return -1;
        }

        if(parser.get_status() != 0){
            return -1;
        } else {
            memcpy(buf, parser.get_buffer().data(), count);
            *bytes_read = count;
            return 0;
        }
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        SignalGuard sguard{};
        using namespace sigma::zeta;
        client_request_builder builder{};

        builder.add_command((uint64_t)client_request_type::Seek);
        builder.add_fd(fd);
        builder.add_whence(whence);
        builder.add_offset(offset);

        if(libsigma_ipc_send(getUmRing(), (libsigma_message_t*)builder.serialize(), builder.length())){
            mlibc::infoLogger() << "Failed to send Seek message" << frg::endlog;
            return -1;
        }

        libsigma_block_thread(SIGMA_BLOCK_WAITING_FOR_IPC, getUmRing());

        size_t res_size = libsigma_ipc_get_msg_size(getUmRing());
        frg::vector<uint8_t, MemoryAllocator> res{getSysdepsAllocator()};
        res.resize(res_size);

        if(libsigma_ipc_receive(getUmRing(), (libsigma_message_t*)res.data())){
            mlibc::infoLogger() << "Failed to receive Seek message response" << frg::endlog;
            return -1;
        }

        server_response_parser parser{res.data(), res.size()};

        if(!parser.has_status()){
            mlibc::infoLogger() << "Seek message response has no Status" << frg::endlog;
            return -1;
        }

        if(parser.get_status() != 0){
            return -1;
        } else {
            *new_offset = tell(fd);
            return 0;
        }
    }

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
    int sys_symlink(const char *target_path, const char *link_path) STUB_ONLY
    int sys_fcntl(int fd, int request, va_list args, int *result) STUB_ONLY
    #endif*/
}
