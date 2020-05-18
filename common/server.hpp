/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
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

namespace nxgallery
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
