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
#include "../../common/server.hpp"
#include "../../common/albumwrapper.hpp"

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

    // Initialize the setsys service (needed to get the Switch's color theme)
    r = setsysInitialize();
    if (R_FAILED(r))
        printf("ERROR initializing setsys: %d\n", R_DESCRIPTION(r));
}

// Closes and unloads all libnx modules we need
void exitSwitchModules()
{
    // Exit the loaded modules in reversed order we loaded them
    setsysExit();
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
    nxlinkStdio();
#endif

    // Other initialization goes here. As a demonstration, we print hello world.
    printCentered("\n\n");
    printCentered(" _   _  __   __   _____           _   _                       \n");
    printCentered("| \\ | | \\ \\ / /  / ____|         | | | |                      \n");
    printCentered("|  \\| |  \\ V /  | |  __    __ _  | | | |   ___   _ __   _   _ \n");
    printCentered("| . ` |   > <   | | |_ |  / _` | | | | |  / _ \\ | '__| | | | |\n");
    printCentered("| |\\  |  / . \\  | |__| | | (_| | | | | | |  __/ | |    | |_| |\n");
    printCentered("|_| \\_| /_/ \\_\\  \\_____|  \\__,_| |_| |_|  \\___| |_|     \\__, |\n");
    printCentered("                                                         __/ |\n");
    printCentered("                                                         |___/ \n\n");
    printCentered(VERSION_STRING);
    printf("\n\n");
    printf(CONSOLE_RED);
    printCentered("Press + to exit\n");
    printf(CONSOLE_RESET);

    // Initialize the album wrapper
    nxgallery::AlbumWrapper::Get()->Init();

    // Create the web server for hosting the web interface, and start it
    nxgallery::WebServer* webServer = new nxgallery::WebServer(1234);
    webServer->Start();

    printf(CONSOLE_GREEN);
    char serverAddress[32];
    webServer->GetAddress(serverAddress);
    printCentered("Open %s in your web browser\n", serverAddress);
    printf(CONSOLE_RESET);

    // Get all paths where Nintendo stores the album content and add these
    // as mount points for the web server
    for (const char* albumPath : nxgallery::AlbumWrapper::Get()->GetAlbumContentPaths())
    {
        webServer->AddMountPoint(albumPath);
    }

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
    nxgallery::AlbumWrapper::Get()->Shutdown();

    // Deinitialize all modules NXGallery needed
    exitSwitchModules();

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
