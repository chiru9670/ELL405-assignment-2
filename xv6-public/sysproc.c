#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#include "sys_send_recv_structs.h"
// The bounded buffer used by sys_send and sys_recv
struct buffer buf;

// Initializes the bounded buffer's lock
// Called in main.c
void sys_send_recv_init() {
  initlock(&buf.lock, "ipc_bounded_buffer_lock");
  for(int i = 0; i < BUFFER_SIZE; ++i) {
    buf.msgs[i].sender_pid = -1;
    buf.msgs[i].rec_pid = -1;
  }
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Add the 2 given int arguments and return the sum
int 
sys_add(void) 
{
  int num1, num2;
  argint(0, &num1);
  argint(1, &num2);
  return num1 + num2;
}

// Globals
#ifndef SYS_fork  // If the macros in syscall.h are not yet defined
#include "syscall.h"
#endif
/*
* Stores names of all system calls
*/
const char *sysCallName[] = {
[SYS_fork]          "sys_fork",
[SYS_exit]          "sys_exit",
[SYS_wait]          "sys_wait",
[SYS_pipe]          "sys_pipe",
[SYS_read]          "sys_read",
[SYS_kill]          "sys_kill",
[SYS_exec]          "sys_exec",
[SYS_fstat]         "sys_fstat",
[SYS_chdir]         "sys_chdir",
[SYS_dup]           "sys_dup",
[SYS_getpid]        "sys_getpid",
[SYS_sbrk]          "sys_sbrk",
[SYS_sleep]         "sys_sleep",
[SYS_uptime]        "sys_uptime",
[SYS_open]          "sys_open",
[SYS_write]         "sys_write",
[SYS_mknod]         "sys_mknod",
[SYS_unlink]        "sys_unlink",
[SYS_link]          "sys_link",
[SYS_mkdir]         "sys_mkdir",
[SYS_close]         "sys_close",
[SYS_add]           "sys_add",
[SYS_toggle]        "sys_toggle",
[SYS_print_count]   "sys_print_count",
[SYS_ps]            "sys_ps",
[SYS_send]          "sys_send",
[SYS_recv]          "sys_recv",
};
int numSysCalls[NELEM(sysCallName)] = {0};
int toggle_state = 0;

int 
sys_toggle(void) 
{
  for(int i=0;i<NELEM(sysCallName);i++){
    numSysCalls[i] = 0;
  }
  if(toggle_state==0){
    toggle_state=1;
  }else{
    toggle_state=0;
  }
  return 0;
}

// For sorting output of print_output
int partition(const char *arr[], int indexArr[], const char x[], int left, int right)
{
  int i = left - 1, j = right;
  // INV: arr[indexArr[a]] <= x for all left <= a <= i and arr[indexArr[b]] > x for all
  // j+1 <= b <= right; left-1 <= i <= j; j <= right
  while (i < j)
  {
    const char * pivotstr = arr[indexArr[i + 1]];
    int pivotlen = strlen(pivotstr), xlen = strlen(x);
    if (strncmp(pivotstr, x, ( pivotlen > xlen ? pivotlen : xlen ) ) <= 0)
    {
      i = i + 1;
    }
    else
    {
      int tmp = indexArr[j];
      indexArr[j] = indexArr[i+1];
      indexArr[i+1] = tmp;
      j = j - 1;
    }
  }
  int p = j;
  // assert: p is such that arr[indexArr[a]] <= x for all left <= a <= p and
  // arr[indexArr[b]] > x for all p+1 <= b <= right
  return p;
}

// Modifies indexArr such that arr[indexArr[i]] <= arr[indexArr[j]]
// for all i <= j
void qsort(const char *arr[], int indexArr[], int left, int right)
{
  if (left < right)
  {
    int p = partition(arr, indexArr, arr[indexArr[left]], left + 1, right);
    int tmp = indexArr[p];
    indexArr[p] = indexArr[left];
    indexArr[left] = tmp;
    qsort(arr, indexArr, left, p - 1);
    qsort(arr, indexArr, p + 1, right);
  }
}

int sys_print_count(void) 
{
  int indexArr[NELEM(sysCallName)];
  for(int i=0; i<NELEM(sysCallName);i++){
    indexArr[i] = i;
  }
  qsort(sysCallName, indexArr, 1, NELEM(sysCallName)-1);
  for(int i=1; i<NELEM(sysCallName);i++){
    if(numSysCalls[indexArr[i]] > 0) {
      cprintf("%s %d\n",sysCallName[indexArr[i]],numSysCalls[indexArr[i]]);
    }
  }
  return 0;
}

int 
sys_ps(void) 
{
  process_analyzer();
  return 0;
}

// Send a message to another process with a given pid
// non-blocking, returns -1 if buffer was seen full 
// by this function at the time when it is called
int sys_send() {
  int sender_pid, rec_pid;
  char *msg;
  argint(0, &sender_pid);
  argint(1, &rec_pid);
  argptr(2, &msg, MSG_LEN);

  acquire(&buf.lock);
  for(int i = 0; i < BUFFER_SIZE; ++i) {
    if(buf.msgs[i].sender_pid <= 0) {
      memmove(buf.msgs[i].msg, msg, MSG_LEN);
      buf.msgs[i].sender_pid = sender_pid;
      buf.msgs[i].rec_pid = rec_pid;
      release(&buf.lock);
      return 0;
    }
  }
  release(&buf.lock);
  return -1;
}

// Receive a message sent by sys_send to the current
// process's pid
int sys_recv() {
  char *msg;
  argptr(0, &msg, MSG_LEN);
  
  int my_pid = myproc()->pid;
  while(1) {
    acquire(&buf.lock);
    for(int i = 0; i < BUFFER_SIZE; ++i) {
      if(buf.msgs[i].rec_pid == my_pid) {
        memmove(msg, buf.msgs[i].msg, MSG_LEN);
        buf.msgs[i].rec_pid = -1;
        buf.msgs[i].sender_pid = -1;
        release(&buf.lock);
        return 0;
      }
    }
    release(&buf.lock);
  }
}