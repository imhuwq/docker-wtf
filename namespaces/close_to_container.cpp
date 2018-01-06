#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>

#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

char *const container_args[] = {
    "/bin/bash",
    NULL
};

int container_main(void *arg) {
  printf("Container [%5d] - inside the container\n", getpid());
  sethostname("container", 10);

  // https://linux.die.net/man/2/mount
  if (mount("proc", "/rootfs/proc", "proc", 0, NULL) != 0) {
    perror("proc");
  }
  if (mount("sysfs", "/rootfs/sys", "sysfs", 0, NULL) != 0) {
    perror("sys");
  }
  if (mount("none", "/rootfs/tmp", "tmpfs", 0, NULL) != 0) {
    perror("tmp");
  }
  if (mount("udev", "/rootfs/dev", "devtmpfs", 0, NULL) != 0) {
    perror("dev");
  }
  if (mount("devpts", "/rootfs/dev/pts", "devpts", 0, NULL) != 0) {
    perror("dev/pts");
  }
  if (mount("shm", "/rootfs/dev/shm", "tmpfs", 0, NULL) != 0) {
    perror("dev/shm");
  }
  if (mount("tmpfs", "/rootfs/run", "tmpfs", 0, NULL) != 0) {
    perror("run");
  }

  if (mount("/conf/hosts", "/rootfs/etc/hosts", "none", MS_BIND, NULL) != 0 ||
      mount("/conf/hostname", "/rootfs/etc/hostname", "none", MS_BIND, NULL) != 0 ||
      mount("/conf/resolv.conf", "/rootfs/etc/resolv.conf", "none", MS_BIND, NULL) != 0) {
    perror("conf");
  }

  if (mount("./", "/rootfs/vol", "ext4", MS_BIND, NULL) != 0) {
    perror("mnt");
  }

  if (chdir("/rootfs") != 0 || chroot("/rootfs") != 0) {
    perror("chdir/chroot");
  }

  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main() {
  printf("Parent [%5d] - start a container!\n", getpid());
  // https://linux.die.net/man/2/clone
  int container_pid = clone(container_main,
                            container_stack + STACK_SIZE,
                            CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWUTS | SIGCHLD,
                            NULL);
  waitpid(container_pid, NULL, 0);
  printf("Parent - container stopped!\n");
  return 0;
}
