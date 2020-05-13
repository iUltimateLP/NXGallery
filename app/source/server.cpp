/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "server.hpp"
#include <switch.h>
using namespace nxgallery;

// Taken from libstratosphere for memory management
#define MEMORY_PAGE_SIZE 0x1000
#define THREAD_STACK_ALIGNMENT 4 * 1024 // 4kb

// The web server will mount assets under this path
#define WEB_STATIC_PATH "romfs:/www"

WebServer::WebServer(int port)
{
    // Store the port
    this->port = port;

    // We won't initialize the web server here just now, 
    // the caller can do that by calling WebServer::Start
}

void WebServer::Start()
{
    // We might actually register an appletHook here

    // Construct a socket address where we want to listen for requests
    static struct sockaddr_in serv_addr;
    serv_addr.sin_addr.s_addr = gethostid(); // The Switch'es IP address
    serv_addr.sin_port = htons(port); //
    serv_addr.sin_family = AF_INET; // The Switch only supports AF_INET and AF_ROUTE: https://switchbrew.org/wiki/Sockets_services#Socket

    // Create a new STREAM IPv4 socket
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        printf("Failed to create a web server socket: %d\n", errno);
        return;
    }

    // Bind the just-created socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Failed to bind web server socket: %d\n", errno);
        return;
    }

    // Start listening to the socket with 5 maximum parallel connections
    if (listen(serverSocket, 5) < 0)
    {
        printf("Failed to listen to the web server socket: %d\n", errno);
        return;
    }

    printf("Listening to %s:%u\n", inet_ntoa(serv_addr.sin_addr), serv_addr.sin_port);

    /*
    // To poll the web server and listen for requests, we need to run in a thread so we won't block the system
    // The threads are libnx' system
    static Thread workerThread;

    // Size of the memory stack for the thread. Partially taken off libstratosphere, it's important to align
    // the stack by 4KB (THREAD_STACK_ALIGNMENT) otherwise we will get a "Bad Input" error when creating the
    // thread.
    constexpr std::size_t workerThreadStackSize = 2 * MEMORY_PAGE_SIZE;
    alignas(THREAD_STACK_ALIGNMENT) static std::uint8_t workerThreadStack[workerThreadStackSize];

    // Create the thread using the thread handle, stack and function above
    // We cast our worker instance to a void* so we can pass it to the thread
    // 0x3f is a special thread priority for background tasks
    // -2 indicates to run the thread on the default CPU core
    // typedef void (*ThreadFunc)(void *); ///< Thread entrypoint function.
    ThreadFunc threadFunc = (ThreadFunc)&WebServer::ServeLoop;
    Result r = threadCreate(&workerThread, threadFunc, NULL, workerThreadStack, workerThreadStackSize, 0x3f, -2);
    if (R_FAILED(r))
    {
        printf("Failed to create web server thread\n");
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }

    r = threadStart(&workerThread);
    if (R_FAILED(r))
    {
        printf("Failed to start web server thread\n");
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }

    r = threadWaitForExit(&workerThread);
    if (R_FAILED(r))
    {
        printf("Failed to wait for web server thread exit\n");
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }

    r = threadClose(&workerThread);
    if (R_FAILED(r))
    {
        printf("Failed to close web server thread\n");
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }
    */
}

void WebServer::ServeLoop(void* args)
{
    // Endless serving loop
    while (true)
    {
        // Accept a new client connection of the web server socket
        struct sockaddr_in clientAddress;
        socklen_t addrLen = sizeof(clientAddress);
        int acceptedConnection = accept(serverSocket, (struct sockaddr*)&clientAddress, &addrLen);
        if (acceptedConnection < 0)
            printf("Accept failed\n");
        else
            printf("Accepted connection\n");
        
        // Serve ("answer") the request which is waiting at the file descriptor accept() returned
        ServeRequest(acceptedConnection, acceptedConnection);

        // After we served the request, close the connection
        close(acceptedConnection);
    }
}

void WebServer::ServeRequest(int in, int out)
{
    // A lot of the code here is taken from the german page here: https://www.kompf.de/cplus/artikel/httpserv.html
    
    // Will hold the raw data to send out
    char buffer[8192];

    // Various pointers for the header reading
    char *b = buffer;
    char *l = buffer;
    char *le;

    // Counts the bytes received from a request
    int count = 0;
    int totalCount = 0;

    // URL which was requested
    char url[256];
    *url = 0;

    // Will hold the requested URL mapped to the server filesystem
    char path[256];

    // Will hold the file descriptor of the file which was requested (e.g. a .html file)
    int fileToServe;

    // While there is data to receive, receive it
    bool breakReceive = false;
    while ((count = recv(in, b, sizeof(buffer) - totalCount, 0)) > 0) 
    {
        // Count the bytes received
        totalCount += count;
        b += count;

        // Go through all bytes we received until now
        while (l < b) 
        {
            // Check if we reached the end of the request by looking at terminators
            le = l;
            while (le < b && *le != '\n' && *le != '\r')
            { 
                ++le;
            }

            if ('\n' == *le || '\r' == *le) 
            {
                // Right now we reached the end and therefore we have a full request header
                *le = 0;
                //printf ("Header line = \"%s\"\n", l);

                // Scan the request header to see if this was the last request header
                // The last request header will always contain the operation requested, such as GET /index.html HTTP/1.0
                // The requested URL will be stored in url
                sscanf(l, "GET %255s HTTP/", url);

                // Was the last header we received an operation? If so, we reached the end
                // of all request headers and we can break this while loop
                if (strlen(l))
                    breakReceive = true;

                l = le + 1;
            }
        }

        // Should we break? Then break
        if (breakReceive) 
            break;
    }

    // Did the request contain a URL for us to serve?
    if (strlen(url)) 
    {
        printf("Received request: GET %s\n", url);

        // Map the requested URL to the path where we serve web assets
        sprintf(path, "%s%s", WEB_STATIC_PATH, url);
        printf("Serving %s\n", path);

        // Open the requested file
        fileToServe = open(path, O_RDONLY);

        // Check if we file we tried to open existed
        if (fileToServe > 0) {
            // The file exists, so lets send an 200 OK to notify the client
            // that we will continue to send HTML data now
            sprintf(buffer, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
            send(out, buffer, strlen(buffer), 0);

            // Read the data from the file requested until there is no data left to read
            do {
                count = read(fileToServe, buffer, sizeof(buffer));

                // Send it out to the client
                send(out, buffer, count, 0);
            } while (count > 0);

            // We successfully read the file to serve, so close it
            close(fileToServe);
            printf("Finished request: GET %s\n", url);
        }
        else
        {
            // The requested file did not exist, send out a 404
            sprintf(buffer, "HTTP/1.0 404 Not Found\n\n");
            send(out, buffer, strlen(buffer), 0);
        }
    }
    else 
    {
        // There was no URL given, likely that another request was issued.
        // Return a 501
        sprintf(buffer, "HTTP/1.0 501 Method Not Implemented\n\n");
        send(out, buffer, strlen(buffer), 0);
    }
}

void WebServer::Stop()
{
    // Stop the httplib::Server
    //server.stop();

    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);

    printf("Stopped WebServer\n");
}
