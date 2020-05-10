/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "server.hpp"
#include <switch.h>
using namespace nxgallery;

WebServer::WebServer(int port)
{
    // Store the port
    this->port = port;

    // We won't initialize the web server here just now, 
    // the caller can do that by calling WebServer::Start
}

void WebServer::Start()
{
    // Hook a logger for the web server
    server.set_logger([](const auto& req, const auto& res) {
        printf("Server received request\n");
    });

    // We might actually register an appletHook here

    // See https://github.com/mtheall/ftpd/blob/master/source/ftp.c#2004
    /*static struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = gethostid();
    serv_addr.sin_port = port;*/

    // Figure out the host to bind the server to
    const char* host = "0.0.0.0";

    // Start the httplib::Server
    server.listen(host, port);
}

void WebServer::Stop()
{
    // Stop the httplib::Server
    server.stop();
}

void WebServer::RegisterRequests()
{
    // Register a test request
    server.Get("/test", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Works!", "text/plain");
    });

    // Register the static server for the web interface
    // Since we will bundle the static web files with the romfs, we require that
    // romfsInitialize() was called first, so we can access the files.
    bool r = server.set_mount_point("/", "romfs:/www");
    if (!r)
    {
        printf("Error mounting static files\n");
    }
}
