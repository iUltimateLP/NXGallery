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

// Include UI code
#include "ui/qrcode.hpp"
#include "ui/mainactivity.hpp"

// Port the server should run on
#define SERVER_PORT 1234

// The main activity Borealis runs
nxgallery::ui::MainActivity* mainActivity = nullptr;

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

// Initializes the borealis UI library
bool initBorealis()
{
    // Set log level
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);

    // Init the app and internalization
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to initialize UI! Exiting");
        return false;
    }

    // Create main window
    brls::Application::createWindow("NXGallery");

    // Have the application register an action on every activity that will quit
    brls::Application::setGlobalQuit(true);

    // Register custom fonts
    brls::Application::loadFontFromFile("mono", "romfs:/robotomono/RobotoMono-Regular.ttf");

    // Register custom components
    brls::Application::registerXMLView("QrCode", nxgallery::ui::QrCode::create);

    // Create a new main activity and push it to the stack
    mainActivity = new nxgallery::ui::MainActivity();
    brls::Application::pushActivity(mainActivity);

    // Set the label's font to be mono
    mainActivity->address->setFont(brls::Application::getFont("mono"));

    return true;
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Initialize all modules NXGallery needs
    initSwitchModules();

    // Initialize Borealis
    if (!initBorealis())
    {
        // If it didn't work, exit all previously opened Switch modules and return this app with an error
        exitSwitchModules();
        return EXIT_FAILURE;
    }

    // Initialize the album wrapper
    nxgallery::AlbumWrapper::Get()->Init();

    // Create the web server for hosting the web interface, add romfs:/www as a mount point for
    // static web assets and start it
    nxgallery::WebServer* webServer = new nxgallery::WebServer(SERVER_PORT);
    webServer->AddMountPoint("romfs:/www");
    webServer->Start();

    char serverAddress[32];
    webServer->GetAddress(serverAddress);
    printf("Open %s in your web browser\n", serverAddress);

    // Encode the server address URL into the QR code the UI displays
    mainActivity->qrCode->setText(std::string(serverAddress));
    mainActivity->address->setText(std::string(serverAddress));

    // Get all paths where Nintendo stores the album content and add these
    // as mount points for the web server
    for (const char* albumPath : nxgallery::AlbumWrapper::Get()->GetAlbumContentPaths())
    {
        webServer->AddMountPoint(albumPath);
    }

    // Run the Borealis loop
    while (brls::Application::mainLoop())
    {
        // Run the web server loop
        webServer->ServeLoop();
    }

    // Stop the web server
    webServer->Stop();

    // Stop the album wrapper
    nxgallery::AlbumWrapper::Get()->Shutdown();

    // Deinitialize all modules NXGallery needed
    exitSwitchModules();

    // Return gracefully
    return EXIT_SUCCESS;
}
