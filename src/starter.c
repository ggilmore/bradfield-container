#include <linux/prctl.h>
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE 65536
#define STRING_SIZE 8000

void
updateCGroup(char* hostname, char* group, const char* content);
void
setupCGroups(char* hostname);

struct child_config
{
  int argc;
  char** argv;
};

/* Entry point for child after `clone` */
int
child(void* arg)
{
  struct child_config* config = arg;
  char* hostname = "the-king";
  sethostname(hostname, strlen(hostname));
  unshare(CLONE_NEWNS);

  char cwd[STRING_SIZE];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("Current working dir: %s\n", cwd);
  }

  setupCGroups(hostname);
  chroot("/home/geoffrey/dev/bradfield-container/container-root");
  chdir("/");

  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("Current working dir: %s\n", cwd);
  }

  prctl(PR_CAPBSET_DROP, CAP_DAC_OVERRIDE);
  prctl(PR_CAPBSET_DROP, CAP_CHOWN);

  if (execvpe(config->argv[0], config->argv, NULL)) {
    fprintf(stderr, "execvpe failed %m.\n");
    return -1;
  }
  return 0;
}

void
setupCGroups(char* hostname)
{

  char pid[STRING_SIZE];
  sprintf(pid, "%d", getpid());
  updateCGroup(hostname, "cgroup.procs", pid);
  updateCGroup(hostname, "pids.max", "10");
  updateCGroup(hostname, "notify_on_release", "1");
}

void
updateCGroup(char* hostname, char* group, const char* content)
{

  char filename[STRING_SIZE];
  sprintf(filename, "/sys/fs/cgroup/pids/%s/%s", hostname, group);
  FILE* fp = fopen(filename, "w+");
  fprintf(fp, content);
  fclose(fp);
}

int
main(int argc, char** argv)
{
  struct child_config config = { 0 };
  int flags = 0;
  pid_t child_pid = 0;

  // Prepare child configuration
  config.argc = argc - 1;
  config.argv = &argv[1];

  // Allocate stack for child
  char* stack = 0;
  if (!(stack = malloc(STACK_SIZE))) {
    fprintf(stderr, "Malloc failed");
    exit(1);
  }

  flags = flags | CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET;

  // Clone parent, enter child code
  if ((child_pid =
         clone(child, stack + STACK_SIZE, flags | SIGCHLD, &config)) == -1) {
    fprintf(stderr, "Clone failed");
    exit(2);
  }

  wait(NULL);

  return 0;
}
