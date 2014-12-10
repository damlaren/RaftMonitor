#include "client.h"
#include "env.h"
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* copyStr(const char* src)
{
  int len = strlen(src);
  char *buf = new char[len + 1];
  std::copy(src, src + len, buf);
  buf[len] = '\0';
  return buf;
}

PacketDropConfig::PacketDropConfig(const std::string& s, float f)
{
  dropping = false;
  srcAddr = s;
  frac = f;
}

PacketDropConfig::~PacketDropConfig()
{
  stopDropping();
}

void PacketDropConfig::startDropping()
{
  if (!dropping)
  {
    std::string cmd =
      std::string("./scripts/startPacketDrop.sh ") +
      srcAddr +
      std::string(" ") +
      std::to_string(frac);
    system(cmd.c_str());
    dropping = true;
  }
}

void PacketDropConfig::stopDropping()
{
  if (dropping)
  {
    std::string cmd =
      std::string("./scripts/stopPacketDrop.sh ") +
      srcAddr +
      std::string(" ") +
      std::to_string(frac);
    system(cmd.c_str());
    dropping = false;
  }
}

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
  //TODO: this should be done in a separate thread
  //for timing reasons.
  if (kill(pid, SIGKILL) == -1)
  {
    perror("stopProcess failed");
    return false;
  }
  return true;
}
