#!/usr/bin/env python
# Copyright (c) 2012 Stanford University
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

"""
This runs some basic tests against a LogCabin cluster.

Usage:
  smoketest.py [options]
  smoketest.py (-h | --help)

Options:
  -h --help            Show this help message and exit
  --binary=<cmd>       Server binary to execute [default: build/LogCabin]
  --client=<cmd>       Client binary to execute
                       [default: build/Examples/SmokeTest]
  --reconf=<opts>      Additional options to pass through to the Reconfigure
                       binary. For example, you can pass the cluster name here.
                       [default: '']
  --servers=<num>      Number of servers [default: 5]
  --timeout=<seconds>  Number of seconds to wait for client to complete before
                       exiting with an error [default: 10]
"""

from __future__ import print_function, division
from common import sh, captureSh, Sandbox, smokehosts
from docopt import docopt
import os
import subprocess
import time

def main():
    arguments = docopt(__doc__)
    client_command = arguments['--client']
    server_command = arguments['--binary']
    num_servers = int(arguments['--servers'])
    reconf_opts = arguments['--reconf']
    if reconf_opts == "''":
        reconf_opts = ""
    timeout = int(arguments['--timeout'])

    server_ids = range(1, num_servers + 1)
    with Sandbox() as sandbox:
        sh('rm -rf smoketeststorage/')
        sh('rm -f debug/*')
        sh('mkdir -p debug')
        print('Initializing first server\'s log')
        sandbox.rsh(smokehosts[0][0],
                    '%s --bootstrap --id 1 --config smoketest.conf' %
                    server_command,
                   stderr=open('debug/bootstrap', 'w'))
        print()

        for server_id in server_ids:
            host = smokehosts[server_id - 1]
            command = ('%s --id %d --config smoketest.conf' %
                       (server_command, server_id))
            print('Starting %s on %s' % (command, smokehosts[server_id - 1][0]))
            sandbox.rsh(smokehosts[server_id - 1][0], command, bg=True,
                        stderr=open('debug/%d' % server_id, 'w'))
            sandbox.checkFailures()

        print('Growing cluster')
        sh('build/Examples/Reconfigure %s %s' %
           (reconf_opts, ' '.join([h[0] for h in smokehosts[:num_servers]])))

        print('Starting %s on localhost' % client_command)
        client = sandbox.rsh('localhost', client_command, bg=True,
                             stderr=open('debug/client', 'w'))

        start = time.time()
        while client.proc.returncode is None:
            sandbox.checkFailures()
            time.sleep(.1)
            if time.time() - start > timeout:
                raise Exception('timeout exceeded')

if __name__ == '__main__':
    main()
