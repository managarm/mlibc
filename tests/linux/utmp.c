#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>

int main() {
	char path[] = "/tmp/mlibc-utmp-test-XXXXXX";
	int fd = mkstemp(path);
	assert(fd != -1);
	close(fd);

	int ret = utmpname(path);
	assert(!ret);

	// Test writing to the utmp file.
	struct utmp entry1;
	memset(&entry1, 0, sizeof(struct utmp));
	entry1.ut_type = USER_PROCESS;
	entry1.ut_pid = 1234;
	strcpy(entry1.ut_line, "pts/1");
	strcpy(entry1.ut_id, "id1");
	strcpy(entry1.ut_user, "user1");
	strcpy(entry1.ut_host, "host1");
	entry1.ut_tv.tv_sec = 1;
	entry1.ut_tv.tv_usec = 1;

	setutent();
	struct utmp *res = pututline(&entry1);
	assert(res);
	endutent();

	// Test reading from the utmp file.
	setutent();
	struct utmp *read_entry = getutent();
	assert(read_entry);
	assert(read_entry->ut_type == USER_PROCESS);
	assert(read_entry->ut_pid == 1234);
	assert(strcmp(read_entry->ut_line, "pts/1") == 0);
	assert(strcmp(read_entry->ut_id, "id1") == 0);
	assert(strcmp(read_entry->ut_user, "user1") == 0);
	assert(strcmp(read_entry->ut_host, "host1") == 0);
	assert(read_entry->ut_tv.tv_sec == 1);
	assert(read_entry->ut_tv.tv_usec == 1);

	read_entry = getutent();
	assert(!read_entry);
	assert(errno == ESRCH);
	endutent();

	// Test getutid
	struct utmp id_entry;
	memset(&id_entry, 0, sizeof(struct utmp));
	id_entry.ut_type = USER_PROCESS;
	strcpy(id_entry.ut_id, "id1");

	setutent();
	struct utmp *id_read_entry = getutid(&id_entry);
	assert(id_read_entry);
	assert(id_read_entry->ut_pid == 1234);

	read_entry = getutent();
	assert(!read_entry);
	assert(errno == ESRCH);
	endutent();

	// Test getutline
	struct utmp line_entry;
	memset(&line_entry, 0, sizeof(struct utmp));
	line_entry.ut_type = USER_PROCESS;
	strcpy(line_entry.ut_line, "pts/1");

	setutent();
	struct utmp *line_read_entry = getutline(&line_entry);
	assert(line_read_entry);
	assert(line_read_entry->ut_pid == 1234);
	endutent();

	// Test getutent_r
	struct utmp buf;
	struct utmp *res_r;
	setutent();
	int ret_r = getutent_r(&buf, &res_r);
	assert(!ret_r);
	assert(res_r == &buf);
	assert(buf.ut_pid == 1234);
	endutent();

	// Test updwtmp.
	char wtmp_path[] = "/tmp/mlibc-wtmp-test-XXXXXX";
	int wtmp_fd = mkstemp(wtmp_path);
	assert(wtmp_fd != -1);
	close(wtmp_fd);

	struct utmp entry2;
	memset(&entry2, 0, sizeof(struct utmp));
	entry2.ut_type = USER_PROCESS;
	entry2.ut_pid = 5678;
	strcpy(entry2.ut_line, "pts/2");
	strcpy(entry2.ut_id, "id2");
	strcpy(entry2.ut_user, "user2");
	strcpy(entry2.ut_host, "host2");
	entry2.ut_tv.tv_sec = 2;
	entry2.ut_tv.tv_usec = 2;

	updwtmp(wtmp_path, &entry2);

	// Verify the wtmp file.
	FILE *f = fopen(wtmp_path, "r");
	assert(f);
	struct utmp wtmp_entry;
	ret = fread(&wtmp_entry, sizeof(struct utmp), 1, f);
	assert(ret == 1);
	fclose(f);

	assert(wtmp_entry.ut_type == USER_PROCESS);
	assert(wtmp_entry.ut_pid == 5678);
	assert(strcmp(wtmp_entry.ut_line, "pts/2") == 0);
	assert(strcmp(wtmp_entry.ut_id, "id2") == 0);
	assert(strcmp(wtmp_entry.ut_user, "user2") == 0);
	assert(strcmp(wtmp_entry.ut_host, "host2") == 0);
	assert(wtmp_entry.ut_tv.tv_sec == 2);
	assert(wtmp_entry.ut_tv.tv_usec == 2);

	unlink(wtmp_path);
	unlink(path);

	return EXIT_SUCCESS;
}
