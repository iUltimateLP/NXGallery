/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// Include the headers from NXGallery
#include "util.hpp"
#include "server.hpp"
#include "albumwrapper.hpp"

// Loads up and initializes all libnx modules needed
void initSwitchModules()
{
    // Initialize the sockets service (needed for networking)
    Result r = socketInitializeDefault();
    if (R_FAILED(r))
        printf("ERROR initializing socket: %d\n", R_DESCRIPTION(r));

    // Initialize the romfs service (needed to serve the static web page from the file system)
    r = romfsInit();
    if (R_FAILED(r))
        printf("ERROR initializing romfs: %d\n", R_DESCRIPTION(r));

    // Initialize the filesystem service (needed to access the filesystem - obviously)
    r = fsInitialize();
    if (R_FAILED(r))
        printf("ERROR initializing fs: %d\n", R_DESCRIPTION(r));

    // Initialize the capsa service (needed to access the Switch's album)
    r = capsaInitialize();
    if (R_FAILED(r))
        printf("ERROR initializing capsa: %d\n", R_DESCRIPTION(r));
}

// Closes and unloads all libnx modules we need
void exitSwitchModules()
{
    // Exit the loaded modules in reversed order we loaded them
    capsaExit();
    fsExit();
    romfsExit();
    socketExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Initialize all modules NXGallery needs
    initSwitchModules();

    // Initialize nxlink stdout and stderr redirect
#ifdef __DEBUG__
    printf("nxlink initialized: %s\n", inet_ntoa(__nxlink_host));
    nxlinkStdio();
#endif

    // Other initialization goes here. As a demonstration, we print hello world.
    printf(YELLOW "NXGallery starting up\n");
    printf(RED "Press + to exit\n" RESET);

    // Create the AlbumWrapper tool and initialize is
    nxgallery::AlbumWrapper* albumWrapper = new nxgallery::AlbumWrapper();
    albumWrapper->Init();

    // Create the web server for hosting the web interface, and start it
    nxgallery::WebServer* webServer = new nxgallery::WebServer(1234);
    webServer->Start();

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        // Run the web server loop
        webServer->ServeLoop();

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Stop the web server
    webServer->Stop();

    // Stop the album wrapper
    albumWrapper->Shutdown();

    // Deinitialize all modules NXGallery needed
    exitSwitchModules();

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
