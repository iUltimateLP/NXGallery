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

// Include borealis
#include <borealis.hpp>

// Port the server should run on
#define SERVER_PORT 1234

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

// Test borealis main activity
class MainActivity : public brls::Activity
{
public:
    CONTENT_FROM_XML_RES("activity/main.xml");
};

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

    // Some default styles
    brls::getStyle().addMetric("about/padding_top_bottom", 50);
    brls::getStyle().addMetric("about/padding_sides", 75);
    brls::getStyle().addMetric("about/description_margin", 50);

    // Push the main activity to the stack
    brls::Application::pushActivity(new MainActivity());

    return true;
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Initialize all modules NXGallery needs
    initSwitchModules();

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

    // Get all paths where Nintendo stores the album content and add these
    // as mount points for the web server
    for (const char* albumPath : nxgallery::AlbumWrapper::Get()->GetAlbumContentPaths())
    {
        webServer->AddMountPoint(albumPath);
    }

    // Initialize Borealis
    if (!initBorealis())
    {
        // If it didn't work, exit all previously opened Switch modules and return this app with an error
        exitSwitchModules();
        return EXIT_FAILURE;
    }

    // Run Borealis 
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
