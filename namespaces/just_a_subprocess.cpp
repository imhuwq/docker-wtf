/*
 *    Namespace   Constant          Isolates
 *    Cgroup      CLONE_NEWCGROUP   Cgroup root directory
 *    IPC         CLONE_NEWIPC      System V IPC, POSIX message queues
 *    Network     CLONE_NEWNET      Network devices, stacks, ports, etc.
 *    Mount       CLONE_NEWNS       Mount points
 *    PID         CLONE_NEWPID      Process IDs
 *    User        CLONE_NEWUSER     User and group IDs
 *    UTS         CLONE_NEWUTS      Hostname and NIS domain name
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

char *const container_args[] = {
    "/bin/bash",
    NULL
};

int container_main(void *arg) {
  printf("Container [%5d] - inside the container\n", getpid());
  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main() {
  printf("Parent [%5d] - start a container!\n", getpid());
  // https://linux.die.net/man/2/clone
  int container_pid = clone(container_main,
                            container_stack + STACK_SIZE,
                            SIGCHLD,
                            NULL);
  waitpid(container_pid, NULL, 0);
  printf("Parent - container stopped!\n");
  return 0;
}