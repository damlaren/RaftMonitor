/* Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <getopt.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "Core/Debug.h"
#include "Core/ThreadId.h"
#include "Server/Globals.h"
#include "Server/RaftConsensus.h"

namespace {

/**
 * Parses argv for the main function.
 */
class OptionParser {
  public:
    OptionParser(int& argc, char**& argv)
        : argc(argc)
        , argv(argv)
        , configFilename("logcabin.conf")
        , serverId(1)
        , bootstrap(false)
    {
        while (true) {
            static struct option longOptions[] = {
               {"bootstrap",  no_argument, NULL, 'b'},
               {"config",  required_argument, NULL, 'c'},
               {"help",  no_argument, NULL, 'h'},
               {"id",  required_argument, NULL, 'i'},
               {0, 0, 0, 0}
            };
            int c = getopt_long(argc, argv, "bc:hi:", longOptions, NULL);

            // Detect the end of the options.
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage();
                    exit(0);
                case 'b':
                    bootstrap = true;
                    break;
                case 'c':
                    configFilename = optarg;
                    break;
                case 'i':
                    serverId = uint64_t(atol(optarg));
                    break;
                case '?':
                default:
                    // getopt_long already printed an error message.
                    usage();
                    exit(1);
            }
        }

        // We don't expect any additional command line arguments (not options).
        if (optind != argc) {
            usage();
            exit(1);
        }
    }

    void usage() {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "  -h, --help          "
                  << "Print this usage information" << std::endl;
        std::cout << "  --bootstrap         "
                  << "Write a cluster configuration into the very first "
                  << "server's log and exit. This should only be run once "
                  << "ever in each cluster" << std::endl;
        std::cout << "  -c, --config <file> "
                  << "Specify the configuration file "
                  << "(default: logcabin.conf)" << std::endl;
        std::cout << "  -i, --id <id>       "
                  << "Set server id to <id> "
                  << "(default: index of first bindable address + 1)"
                  << std::endl;
    }

    int& argc;
    char**& argv;
    std::string configFilename;
    uint64_t serverId;
    bool bootstrap;
};

} // anonymous namespace

int
main(int argc, char** argv)
{
    LogCabin::Core::ThreadId::setName("evloop");
    //LogCabin::Core::Debug::setLogPolicy({{"Server", "VERBOSE"}});

    // Parse command line args.
    OptionParser options(argc, argv);
    NOTICE("Using config file %s", options.configFilename.c_str());

    // Initialize and run Globals.
    LogCabin::Server::Globals globals;
    globals.config.readFile(options.configFilename.c_str());
    globals.init(options.serverId);
    if (options.bootstrap) {
        globals.raft->bootstrapConfiguration();
        NOTICE("Done bootstrapping configuration. Exiting.");
    } else {
        globals.run();
    }
    return 0;
}
