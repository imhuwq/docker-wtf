#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/capability.h>
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

int pipefd[2];

void set_map(char *file, int inside_id, int outside_id, int len) {
  FILE *mapfd = fopen(file, "w");
  if (NULL == mapfd) {
    perror("open file error");
    return;
  }
  fprintf(mapfd, "%d %d %d", inside_id, outside_id, len);
  fclose(mapfd);
}

void set_uid_map(pid_t pid, int inside_id, int outside_id, int len) {
  char file[256];
  sprintf(file, "/proc/%d/uid_map", pid);
  set_map(file, inside_id, outside_id, len);
}

void set_gid_map(pid_t pid, int inside_id, int outside_id, int len) {
  char file[256];
  sprintf(file, "/proc/%d/gid_map", pid);
  set_map(file, inside_id, 0, len);
}

int container_main(void *arg) {
  printf("Container [%5d] - inside the container!\n", getpid());
  printf("Container: eUID = %ld;  eGID = %ld, UID=%ld, GID=%ld\n",
         (long) geteuid(), (long) getegid(), (long) getuid(), (long) getgid());

  char ch;
  close(pipefd[1]);
  read(pipefd[0], &ch, 1);

  printf("Container [%5d] - setup hostname!\n", getpid());
  sethostname("container", 10);

  mount("proc", "/rootfs/proc", "proc", 0, NULL);
  chdir("/rootfs");
  chroot("/rootfs");

  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main() {
  const int gid = getgid(), uid = getuid();
  printf("Parent: eUID = %ld;  eGID = %ld, UID=%ld, GID=%ld\n",
         (long) geteuid(), (long) getegid(), (long) getuid(), (long) getgid());

  pipe(pipefd);

  int container_pid = clone(container_main,
                            container_stack + STACK_SIZE,
                            CLONE_NEWNET | CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWUTS | SIGCHLD,
                            NULL);
  printf("Parent [%5d] - start a container %5d!\n", getpid(), container_pid);

  set_uid_map(container_pid, 0, uid, 1);
  set_gid_map(container_pid, 0, gid, 1); // won't work
  printf("Parent [%5d] - user/group mapping done!\n", getpid());
  close(pipefd[1]);

  // create peer network interface and put one of them to the container network namespace
//  system("ip link add name veth0 type veth peer name veth1 netns " + container_pid);

  waitpid(container_pid, NULL, 0);
  printf("Parent - container stopped!\n");
  return 0;
}