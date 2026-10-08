#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

#include <KittyMemoryEx.hpp>

#include "Dump.hpp"

int main(int argc, char **argv)
{
    setbuf(stdout, nullptr);

    std::string package, outputDir;
    pid_t pid = 0;
    for (int opt; (opt = getopt(argc, argv, "p:o:i:")) != -1;)
    {
        if (opt == 'p') package = optarg;
        else if (opt == 'o') outputDir = optarg;
        else if (opt == 'i') pid = atoi(optarg);
    }
    if (package.empty() || outputDir.empty())
    {
        fprintf(stderr, "Usage: %s -p <package> -o <output directory> [-i <pid>]\n", argv[0]);
        return 1;
    }
    if (!pid) pid = KittyMemoryEx::getProcessID(package);
    if (pid <= 0)
    {
        fprintf(stderr, "%s is not running.\n", package.c_str());
        return 1;
    }
    return DumpGame(pid, package, outputDir) ? 0 : 1;
}
