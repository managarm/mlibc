#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <frg/vector.hpp>
#include <frg/string.hpp>
#include <frg/eternal.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>

#include <libsigma/sys.h>

namespace {
    class id_allocator{
        public:
        constexpr id_allocator()
        : id(0) {}

        uint64_t get_id(){
            return id++;
        }

        private:
        uint64_t id;
    };

    auto file_descriptor_allocator = id_allocator();

    struct file_descriptor {
        file_descriptor()
        : fd(0), offset(0), name(frg::string<MemoryAllocator>(getAllocator())) {}
        file_descriptor(const char* name)
        : fd(file_descriptor_allocator.get_id()), offset(0), \
          name(frg::string<MemoryAllocator>(getAllocator(), name) + '\0') {}

        uint64_t fd, offset;
        frg::string<MemoryAllocator> name;
    };

    static frg::vector<file_descriptor, MemoryAllocator>& get_file_descriptor_list(){
        static frg::eternal<frg::vector<file_descriptor, MemoryAllocator>> file_descriptor_list(getAllocator());
        return file_descriptor_list.get();
    }

}


namespace mlibc {

int sys_tcb_set(void *pointer) {
	libsigma_set_fsbase(reinterpret_cast<uint64_t>(pointer));
	return 0;
}



int sys_open(const char *path, int flags, int *fd){
    char buf[2] = {};
    if(libsigma_read_initrd_file(path, reinterpret_cast<uint8_t*>(buf), 0, 1)) return ENOENT; // File doesn't exist
    auto& file = get_file_descriptor_list().push_back(file_descriptor(path));
    (void)(flags);
    *fd = file.fd;
    return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
    __ensure(whence == SEEK_SET);

    for(auto& file : get_file_descriptor_list()){
        if(file.fd == fd){
            file.offset = offset;
            *new_offset = offset;
            return 0;
        }
    }

    return EBADF;
}
int sys_read(int fd, void *data, size_t length, ssize_t *bytes_read){
    for(auto& file : get_file_descriptor_list()){
        if(file.fd == fd){
            if(libsigma_read_initrd_file(file.name.data(), (uint8_t*)data, file.offset, length)){
                *bytes_read = 0;
                return EBADF;
            } else {
                *bytes_read = length;
                return 0;
            }
        }
    }

    return EBADF;
}
int sys_close(int fd){
    return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
    if(!(flags & MAP_ANONYMOUS))
            __ensure(!"MAP_ANONYMOUS must be set, fd based mmaps are unimplemented");

    void* ret = libsigma_vm_map(size, hint, NULL, prot, flags);
    if(!ret)
        return ENOMEM;
    *window = ret;
    return 0;
} // namespace mlibc

}
