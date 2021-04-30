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

    // Initialize the setsys service (needed to get the Switch's color theme)
    r = setsysInitialize();
    if (R_FAILED(r))
        printf("ERROR initializing setsys: %d\n", R_DESCRIPTION(r));

    // Initialize the ns service (needed to translate title IDs to title names)
    r = nsInitialize();
    if (R_FAILED(r))
        printf("ERROR initializing ns: %d\n", R_DESCRIPTION(r));
}

// Closes and unloads all libnx modules we need
void exitSwitchModules()
{
    // Exit the loaded modules in reversed order we loaded them
    nsExit();
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

    // Configure the gamepad input
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    // Initialize nxlink stdout and stderr redirect
#ifdef __DEBUG__
    nxlinkStdio();
#endif

    // Print a welcome message
    // Let's be nice!
    std::vector<std::string> NiceLines = {
        "You look nice today!",
        "I miss Brewster...",
        "Did you know you can calculate the times the DVD logo hits the corner?",
        "Much love to the whole Switch Homebrew team!",
        "I wish I was better at Kart",
        "You should try Minecraft!",
        "Nintendo... seriously?",
        "Did you know? The moon has \"moonquakes!\"",
        "You now breathe in manual mode!",
        "Rabbits are incapable of spewing.",
        "Bees sometimes sting other bees",
        "I should add more features instead of thinking of lines to put here."
    };

    srand(time(NULL));

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
    printf(CONSOLE_YELLOW);
    printCentered((" +++ " + NiceLines[rand() % NiceLines.size()] + " +++ ").c_str());
    printf("\n\n");
    printf(CONSOLE_RED);
    printCentered("Press + to exit");
    printf("\n\n");
    printf(CONSOLE_RESET);

    // Initialize the album wrapper
    nxgallery::AlbumWrapper::Get()->Init();

    // Create the web server for hosting the web interface, add romfs:/www as a mount point for
    // static web assets and start it
    nxgallery::WebServer* webServer = new nxgallery::WebServer(1234);
    webServer->AddMountPoint("romfs:/www");
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
        // Scan all gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been
        // newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
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
