#pragma once

#include <string>
#include <sys/types.h>

// Probes the reflection offsets of the game running as pid, then writes the
// dump to <outputDir>/UEDump3r/<package>. Returns false when nothing was written.
bool DumpGame(pid_t pid, const std::string &package, const std::string &outputDir);
