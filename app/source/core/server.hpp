/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)

    MIT License

    Copyright (c) 2020-2021 Jonathan Verbeek

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/time.h>
#include <vector>
#include <switch.h>

namespace nxgallery::core
{
    // This class will handle the web server running.
    // Therefore it's a very important part for this app
    // Huge shoutout to https://www.kompf.de/cplus/artikel/httpserv.html
    // and the man7.org Linux manual
    class WebServer
    {
    public:
        // Constructor taking in the port for this server to run on
        WebServer(int port);
        
        // Starts the web server
        void Start();

        // Stops the web server
        void Stop();

        // Adds a new mount point for where the server will look for files to serve
        void AddMountPoint(const char* path);

        // The loop for the server to accept and handle requests
        void ServeLoop();

        // Returns the address and port string this server is running on
        void GetAddress(char* buffer);

    private:
        // Serves a incoming request to the speicified out socket
        static void ServeRequest(int in, int out, std::vector<const char*> mountPoints);

    public:
        // The port the server is running on
        int port;

        // Holds whether the server is fully initialized or not
        bool isRunning;

        // The socket which was opened for the HTTP server
        int serverSocket;

        // The mounted folders the web server can serve from
        std::vector<const char*> mountPoints;
    };
}
