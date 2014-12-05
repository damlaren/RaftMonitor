#include "env.h"

// For now, assumed running in top level of repository.
// TODO: we'll want some way of setting this to an
// absolute path.
std::string RaftEnv::rootDir = ".";
