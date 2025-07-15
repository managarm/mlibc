#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>

int main() {
	char path[] = "/tmp/mlibc-utmpx-test-XXXXXX";
	int fd = mkstemp(path);
	assert(fd != -1);
	close(fd);

	int ret = utmpxname(path);
	assert(!ret);

	struct utmpx entry;
	memset(&entry, 0, sizeof(struct utmpx));

	// Test an empty file.
	setutxent();
	struct utmpx *read_entry = getutxent();
	assert(!read_entry);
	endutxent();

	// Test writing a new entry.
	entry.ut_type = USER_PROCESS;
	entry.ut_pid = getpid();
	strncpy(entry.ut_line, "tty1", sizeof(entry.ut_line));
	strncpy(entry.ut_id, "t1", sizeof(entry.ut_id));
	strncpy(entry.ut_user, "root", sizeof(entry.ut_user));
	strncpy(entry.ut_host, "localhost", sizeof(entry.ut_host));
	entry.ut_tv.tv_sec = 1234567890;
	entry.ut_tv.tv_usec = 0;
	setutxent();
	assert(pututxline(&entry));
	endutxent();

	// Test reading the entry back.
	setutxent();
	read_entry = getutxent();
	assert(read_entry);
	assert(read_entry->ut_type == USER_PROCESS);
	assert(read_entry->ut_pid == getpid());
	assert(!strcmp(read_entry->ut_line, "tty1"));
	assert(!strcmp(read_entry->ut_id, "t1"));
	assert(!strcmp(read_entry->ut_user, "root"));
	assert(!strcmp(read_entry->ut_host, "localhost"));
	assert(read_entry->ut_tv.tv_sec == 1234567890);
	read_entry = getutxent();
	assert(!read_entry);
	endutxent();

	// Test getutxid().
	struct utmpx id_entry;
	memset(&id_entry, 0, sizeof(struct utmpx));
	id_entry.ut_type = USER_PROCESS;
	strncpy(id_entry.ut_id, "t1", sizeof(id_entry.ut_id));
	setutxent();
	struct utmpx *id_read_entry = getutxid(&id_entry);
	assert(id_read_entry);
	assert(id_read_entry->ut_pid == getpid());
	endutxent();

	// Test getutxline().
	struct utmpx line_entry;
	memset(&line_entry, 0, sizeof(struct utmpx));
	line_entry.ut_type = USER_PROCESS;
	strncpy(line_entry.ut_line, "tty1", sizeof(line_entry.ut_line));
	setutxent();
	struct utmpx *line_read_entry = getutxline(&line_entry);
	assert(line_read_entry);
	assert(line_read_entry->ut_pid == getpid());
	endutxent();

	// Test updwtmpx.
	char wtmpx_path[] = "/tmp/mlibc-wtmpx-test-XXXXXX";
	int wtmpx_fd = mkstemp(wtmpx_path);
	assert(wtmpx_fd != -1);
	close(wtmpx_fd);

	struct utmpx entry2;
	memset(&entry2, 0, sizeof(struct utmpx));
	entry2.ut_type = USER_PROCESS;
	entry2.ut_pid = 5678;
	strcpy(entry2.ut_line, "pts/2");
	strcpy(entry2.ut_id, "id2");
	strcpy(entry2.ut_user, "user2");
	strcpy(entry2.ut_host, "host2");
	entry2.ut_tv.tv_sec = 2;
	entry2.ut_tv.tv_usec = 2;

	updwtmpx(wtmpx_path, &entry2);

	// Verify the wtmpx file.
	FILE *f = fopen(wtmpx_path, "r");
	assert(f);
	struct utmpx wtmpx_entry;
	ret = fread(&wtmpx_entry, sizeof(struct utmpx), 1, f);
	assert(ret == 1);
	fclose(f);

	assert(wtmpx_entry.ut_type == USER_PROCESS);
	assert(wtmpx_entry.ut_pid == 5678);
	assert(strcmp(wtmpx_entry.ut_line, "pts/2") == 0);
	assert(strcmp(wtmpx_entry.ut_id, "id2") == 0);
	assert(strcmp(wtmpx_entry.ut_user, "user2") == 0);
	assert(strcmp(wtmpx_entry.ut_host, "host2") == 0);
	assert(wtmpx_entry.ut_tv.tv_sec == 2);
	assert(wtmpx_entry.ut_tv.tv_usec == 2);

	unlink(wtmpx_path);
	unlink(path);

	return EXIT_SUCCESS;
}
