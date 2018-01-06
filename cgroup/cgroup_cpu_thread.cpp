#define _GNU_SOURCE         /* See feature_test_macros(7) */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

const int NUM_THREADS = 4;

void *thread_main(void *threadid) {
  /* 把自己加入cgroup中（syscall(SYS_gettid)为得到线程的系统tid） */
  char cmd[128];
  sprintf(cmd, "echo %ld >> /sys/fs/cgroup/cpu/john/tasks", syscall(SYS_gettid));
  system(cmd);
  sprintf(cmd, "echo %ld >> /sys/fs/cgroup/cpuset/john/tasks", syscall(SYS_gettid));
  system(cmd);

  long tid;
  tid = (long) threadid;
  printf("thread #%ld, pid #%ld\n", tid, syscall(SYS_gettid));

  int a = 0;
  while (1) {
    a++;
  }
  pthread_exit(NULL);
}

int main(const int argc, const char **argv) {
  int num_threads;
  if (argc > 1) {
    num_threads = atoi(argv[1]);
  }
  if (num_threads <= 0 || num_threads >= 100) {
    num_threads = NUM_THREADS;
  }

  mkdir("/sys/fs/cgroup/cpu/john", 755);
  system("echo 50000 > /sys/fs/cgroup/cpu/john/cpu.cfs_quota_us");

  mkdir("/sys/fs/cgroup/cpuset/john", 755);
  /* 限制CPU只能使用#2核和#3核 */
  system("echo \"2,3\" > /sys/fs/cgroup/cpuset/john/cpuset.cpus");
  system("echo 0 > /sys/fs/cgroup/cpuset/john/cpuset.mems");


  pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
  for (long thread_id = 0; thread_id < num_threads; thread_id++) {
    printf("Main: creating thread %ld\n", thread_id);
    int err_code = pthread_create(&threads[thread_id], NULL, thread_main, (void *) thread_id);
    if (err_code) {
      printf("ERROR: return code from pthread_create() is %d\n", err_code);
      exit(-1);
    }
  }

  pthread_exit(NULL);
  free(threads);
}