#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int main()
{
    int fd = openat(3, "test.txt", 0, 0, NULL);

    char buf[100];
    ssize_t nbytes = pread(fd, buf, sizeof(buf), 0);
    ssize_t total_written = 0;
    while (total_written < nbytes)
    {
        ssize_t bytes_written = pwrite(1, buf + total_written, nbytes - total_written, 0);
        if (bytes_written < 0)
        {
            exit(1);
        }
        total_written += bytes_written;
    }
    close(fd);

    char *mapping = mmap(NULL, 4096, 0, 1 << 1, -1, 0);
    mapping[0] = 'A';
    munmap(mapping, 4096);

    // I don't think file backed mappings are 100% functional right now, but let's at least check that I can map and unmap
    int fd2 = openat(3, "test2.txt", 0, 0, NULL);
    char *mapped_file = mmap(NULL, 4096, 0, 1 << 1, fd2, 0);
    munmap(mapped_file, 4096);
    close(fd2);

    exit(0);
}