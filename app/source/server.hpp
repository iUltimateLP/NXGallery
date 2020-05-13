/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
//#include "httplib.hpp"
#include <arpa/inet.h>
#include <cstring>
//#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <csignal>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <cassert>
#include <climits>
#include <condition_variable>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <thread>

namespace nxgallery
{
    // This class will handle the web server running.
    // Therefore it's a very important part for this app
    // Huge shoutout to https://www.kompf.de/cplus/artikel/httpserv.html
    class WebServer
    {
    public:
        // Constructor taking in the port for this server to run on
        WebServer(int port);
        
        // Starts the web server
        void Start();

        // Stops the web server
        void Stop();

        // The loop for the server to accept and handle requests
        void ServeLoop(void* args);

    private:
        // Serves a incoming request to the speicified out socket
        void ServeRequest(int in, int out);

    private:
        // The port the server is running on
        int port;

        // The socket which was opened for the HTTP server
        int serverSocket;
    };
}
