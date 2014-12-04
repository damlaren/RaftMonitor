#include "client.h"
#include <cstdio>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

pid_t RaftClusterConfig::createProcess(const char* path, char* const args[])
{
  pid_t pid = fork();

  if (pid == 0)
  {
    // Child process executes command.
    int status = execv(path, args);
    if (status == -1)
    {
      //TODO signal failure to parent
      perror("createProcess failed");
      exit(1);
    }
  }

  return pid;
}

bool RaftClusterConfig::stopProcess(pid_t pid)
{
  if (kill(pid, SIGKILL) == -1)
  {
    perror("stopProcess failed");
    return false;
  }
  return true;
}
