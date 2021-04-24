// Defines the structs used by sys_send and sys_recv

#include "spinlock.h"
#include "param.h"	// For NPROC

#define MSG_LEN 8
#define BUFFER_SIZE 1024

struct message
{
	char msg[MSG_LEN];
	int sender_pid, rec_pid;
};

struct buffer
{
	struct spinlock lock;
	struct message msgs[BUFFER_SIZE];
};

extern void sys_send_recv_init();
