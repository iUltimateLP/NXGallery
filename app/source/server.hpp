/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
#include "httplib.hpp"

namespace nxgallery
{
    // This class will handle the web server running.
    // Therefore it's a very important part for this app
    class WebServer
    {
    public:
        // Constructor taking in the port for this server to run on
        WebServer(int port);
        
        // Starts the web server
        void Start();

        // Stops the web server
        void Stop();

    private:
        // Registers all HTTP requests we will provide
        void RegisterRequests();

    private:
        // The port the server is running on
        int port;

        // Holds the http server instance
        httplib::Server server;
    };
}
