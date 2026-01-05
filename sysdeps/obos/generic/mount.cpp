extern "C" {

int mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data) {}
int umount(const char *target) {}
int umount2(const char *target, int flags) {}

}
