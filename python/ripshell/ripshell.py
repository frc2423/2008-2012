#!/usr/bin/env python3

# Copyright (c) 2006 Guido Wesdorp. All rights reserved.
#  --> ported to Python3, Dustin Spicuzza (10/28/2010)

# This software is distributed under the terms of the RIPShell
# License. See LICENSE.txt for license text.

"""simple socket server to remotely execute Python code

    the client can either send a full string of Python code and have it
    executed, or have the server present a Python prompt; in both cases
    stdin, stdout and stderr are redirected from the server to the client
"""

import socket
import code
import sys
import _thread
import traceback

class config:
    server_ip = '0.0.0.0'
    port = 8002

__version__ = '0.2 SVN'

class STDFilePointers:
    """proxy for file pointers

        can be used to redirect sys.stdin, sys.stdout and sys.stderr to a
        socket
    """
    def __init__(self, conn):
        self.conn = conn

    def write(self, s):
        self.conn.send(s.encode())

    def read(self, l):
        r = self.conn.recv(l)
        if r:
            return r.decode()
        #if not r:
        #    raise IOError('Connection closed')
        return ' '

    def readline(self):
        data = []
        while 1:
            c = self.read(1)
            if c == '\n':
                return ''.join(data) + '\n'
            data.append(c)

class RIPServer:
    """server that processes Python code from the client locally
    
        either executes file content with stdin, stdout and stderr redirected
        from and to the client, or runs an interactive interpreter loop
    """

    banner = ('Python %s\n'
                'Remote Interactive Python Shell v%s\n'
                'Type "help", "copyright", "credits" or "license" '
                    'for more information.' % (sys.version, __version__))

    def __init__(self, config):
        """open a socket and start waiting for connections"""
        self.config = config
        self.sock = s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.bind((config.server_ip, config.port))
            s.listen(1)
            while 1:
                conn, addr = s.accept()
                print('Connection from', addr)
                self.handle(conn, addr)
                conn.close()
                print('Connection closed')
        finally:
            print('Closing')
            s.close()

    def handle(self, conn, addr):
        """handle a new connection"""
        stdfps = STDFilePointers(conn)
        sys.stdin = stdfps
        sys.stdout = stdfps
        sys.stderr = stdfps
        try:
            try:
                command = conn.recv(1).decode()
                # dispatch depending on command (first char sent should be '-'
                # for the interactive interpreter loop, 'x' for executing code)
                if command == '-':
                    self.interpreterloop(conn, addr)
                elif command == 'x':
                    self.readandexec(conn, addr)
                else:
                    print('Unexpected input, exiting...')
            except SystemExit as e:
                # raise a SystemExit with message 'quit' to stop the server
                # from a client
                if str(e) == 'quit':
                    print('Stopping server')
                    raise # kill the server
                print('SystemExit')
            except:
                exc, e, tb = sys.exc_info()
                try:
                    print('%s - %s' % (exc, e))
                    print('\n'.join(traceback.format_tb(tb)))
                except:
                    pass
                del tb
                print('Exception:', exc, '-', e, file=sys.__stdout__)
        finally:
            sys.stdout = sys.__stdout__
            sys.stderr = sys.__stderr__

    def interpreterloop(self, conn, addr):
        """standard code interpreter loop"""
        code.interact(self.banner)

    def readandexec(self, conn, addr):
        """execute file data"""
        data = []
        while 1:
            c = conn.recv(1)
            if not c:
                raise IOError('Connection closed')
                return
            c = c.decode()
            if c == '\0':
                exec(''.join(data))
                return
            data.append(c)

class RIPClient:
    """sends code to the server to execute and prints returned data
    
        thin client, basically only presents stdout and stderr from,
        and sends stdin to, the server
    """
    
    def __init__(self, server_ip, config , filedata=None):
        """connect to the server and start the session

            if filedata resolves to False, the interactive interpreter loop
            is started; if not it should be a string containing Python code,
            which will be executed on the server
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((server_ip, config.port))
            if filedata:
                self.execfile(s, filedata)
            else:
                self.interpreterloop(s)
        finally:
            s.close()

    def interpreterloop(self, sock):
        """interpreter"""
        sock.send(b'-') # tell the server we want to get a prompt
        _thread.start_new_thread(self.readloop, (sock,))
        self.writeloop(sock)

    def execfile(self, sock, filedata):
        """execute file data"""
        sock.send(b'x') # tell the server file data is coming
        _thread.start_new_thread(self.readloop, (sock,))
        sock.send(filedata.encode())
        sock.send(b'\0') # tell the server we're done...
        self.writeloop(sock)

    def readloop(self, sock):
        while 1:
            try:
                sock.send(sys.stdin.read(1).encode())
            except socket.error:
                return

    def writeloop(self, sock):
        while 1:
            c = sock.recv(1)
            if not c:
                break
            sys.stdout.write(c.decode())
            sys.stdout.flush()

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('Starting server...')
        RIPServer(config) # blocks until done
        sys.exit(0)
    elif len(sys.argv) == 2 and sys.argv[1] not in ['-h', '?', '--help']:
        filedata = None
        #if sys.argv[1] != '-':
        #    fp = open(sys.argv[1])
        #    try:
        #        filedata = fp.read()
        #    finally:
        #        fp.close()
        RIPClient(sys.argv[1], config, filedata) # blocks until done
        sys.exit(0)
    print('Usage: %s [-|<filename>]' % (sys.argv[0],))
    print()
    print ('Without any command line arguments, the server is started, which\n'
            'allows executing Python code from connected clients. When the\n'
            'first argument is a dash (-), the client connects to the server\n'
            'and requests it to start an interactive loop. Any other\n'
            'argument will be considered a path to a Python script, which\n'
            'will be executed on the server.\n')

