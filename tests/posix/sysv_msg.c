#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

struct test_msgbuf {
	long mtype;
	char mtext[64];
};

int main(void) {
	srand(time(NULL));

	int msqid = -1;
	for (int i = 0; i < 10; i++) {
		key_t key = (key_t)rand();
		msqid = msgget(key, 0644 | IPC_CREAT | IPC_EXCL);
		if (msqid != -1) {
			break;
		}
	}
	assert(msqid != -1);

	struct msqid_ds buf = {0};
	int res_stat1 = msgctl(msqid, IPC_STAT, &buf);
	assert(res_stat1 == 0);

	msgqnum_t qnum1 = buf.msg_qnum;
	assert(qnum1 == 0);

	unsigned long cbytes1 = buf.msg_cbytes;
	assert(cbytes1 == 0);

	pid_t lspid1 = buf.msg_lspid;
	assert(lspid1 == 0);

	pid_t lrpid1 = buf.msg_lrpid;
	assert(lrpid1 == 0);

	unsigned int mode1 = buf.msg_perm.mode & 0777;
	assert(mode1 == 0644);

	struct test_msgbuf send_msg;
	send_msg.mtype = 1;
	strcpy(send_msg.mtext, "Hello World from Message Queue!");

	int res_send = msgsnd(msqid, &send_msg, sizeof(send_msg.mtext), 0);
	assert(res_send == 0);

	int res_stat2 = msgctl(msqid, IPC_STAT, &buf);
	assert(res_stat2 == 0);

	msgqnum_t qnum2 = buf.msg_qnum;
	assert(qnum2 == 1);

	unsigned long cbytes2 = buf.msg_cbytes;
	unsigned long expected_size = sizeof(send_msg.mtext);
	assert(cbytes2 == expected_size);

	pid_t lspid2 = buf.msg_lspid;
	pid_t my_pid = getpid();
	assert(lspid2 == my_pid);

	pid_t lrpid2 = buf.msg_lrpid;
	assert(lrpid2 == 0);

	time_t stime2 = buf.msg_stime;
	assert(stime2 > 0);

	struct test_msgbuf recv_msg;
	memset(&recv_msg, 0, sizeof(recv_msg));
	ssize_t res_rcv = msgrcv(msqid, &recv_msg, sizeof(recv_msg.mtext), 1, 0);
	assert(res_rcv == (ssize_t)expected_size);

	long recv_type = recv_msg.mtype;
	assert(recv_type == 1);

	int cmp = strcmp(recv_msg.mtext, "Hello World from Message Queue!");
	assert(cmp == 0);

	int res_stat3 = msgctl(msqid, IPC_STAT, &buf);
	assert(res_stat3 == 0);

	msgqnum_t qnum3 = buf.msg_qnum;
	assert(qnum3 == 0);

	unsigned long cbytes3 = buf.msg_cbytes;
	assert(cbytes3 == 0);

	pid_t lspid3 = buf.msg_lspid;
	assert(lspid3 == my_pid);

	// TODO: enable this check when qemu-user's issue is fixed
	// https://lore.kernel.org/qemu-devel/20260724160105.3787888-1-leo@managarm.org/T/#u
#if 0
	pid_t lrpid3 = buf.msg_lrpid;
	assert(lrpid3 == my_pid);
#endif

	time_t rtime3 = buf.msg_rtime;
	assert(rtime3 > 0);

	int res_rm = msgctl(msqid, IPC_RMID, NULL);
	assert(res_rm == 0);

	return 0;
}
