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
#include "../../common/server.hpp"
#include "../../common/albumwrapper.hpp"

#define R_ASSERT(res_expr)            \
    ({                                \
        const Result rc = (res_expr); \
        if (R_FAILED(rc))             \
        {                             \
            fatalThrow(rc);           \
        }                             \
    })

// Taken from libstratosphere for memory management
#define MEMORY_PAGE_SIZE 0x1000
#define HEAP_SIZE 0xA7000

#define THREAD_STACK_ALIGNMENT 4 * 1024 // 4kb

extern "C" 
{
    // Sysmodules should not use applet*
    u32 __nx_applet_type = AppletType_None;

    // Adjust heap size as needed
    size_t nx_inner_heap_size = HEAP_SIZE;
    char   nx_inner_heap[HEAP_SIZE];
}

// Called by libnx to initialize the heap memory we get for this sysmodule
extern "C" void __libnx_initheap(void)
{
    // Original NX Heap memory
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	// Fake Heap which we could use for our own data
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

// Initializes the sysmodule, used to init services
extern "C" void __attribute__((weak)) __appInit(void)
{
    R_ASSERT(smInitialize());
    R_ASSERT(fsInitialize());
    R_ASSERT(fsdevMountSdmc());
    R_ASSERT(capsaInitialize());
    R_ASSERT(setsysInitialize());

    static const SocketInitConfig socketInitConfig = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = 0x800,
        .tcp_rx_buf_size = 0x800,
        .tcp_tx_buf_max_size = 0x25000,
        .tcp_rx_buf_max_size = 0x25000,

        //We don't use UDP, set all UDP buffers to 0
        .udp_tx_buf_size = 0,
        .udp_rx_buf_size = 0,

        .sb_efficiency = 1,
    };
    R_ASSERT(socketInitialize(&socketInitConfig));
}

// Called (and not needed because this is a sysmodule) when the user tries to exit this app
extern "C" void __attribute__((weak)) userAppExit(void);

// Called when the Switch requests this sysmodule to exit
extern "C" void __attribute__((weak)) __appExit(void)
{
    // Exit the loaded modules in reversed order we loaded them
    socketExit();
    setsysExit();
    capsaExit();
    fsExit();
    smExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Initialize the album wrapper
    //nxgallery::AlbumWrapper::Get()->Init();

    // Create the web server for hosting the web interface, add romfs:/www as a mount point for
    // static web assets and start it
    nxgallery::WebServer* webServer = new nxgallery::WebServer(1234);
    //webServer->AddMountPoint("romfs:/www");
    webServer->Start();

    // To run the web server, we need to run in a thread so we won't block the system
    // The threads are libnx' system
    static Thread serverThread;

    // Size of the memory stack for the thread. Partially taken off libstratosphere, it's important to align
    // the stack by 4KB (THREAD_STACK_ALIGNMENT) otherwise we will get a "Bad Input" error when creating the
    // thread.
    constexpr std::size_t serverThreadStackSize = 2 * HEAP_SIZE;
    alignas(THREAD_STACK_ALIGNMENT) static std::uint8_t serverThreadStack[serverThreadStackSize];

    // This is the function which runs in our thread
    static auto serverThreadFunc = +[](void* args) {
        // Get back the web server from the args
        nxgallery::WebServer* webServer = static_cast<nxgallery::WebServer*>(args);
    
        while (true)
        {
            // Run the serve loop
            webServer->ServeLoop();
        }
    };

    // Create the thread using the thread handle, stack and function above
    // We cast our worker instance to a void* so we can pass it to the thread
    // 0x3f is a special thread priority for background tasks
    // -2 indicates to run the thread on the default CPU core
    Result r = threadCreate(&serverThread, serverThreadFunc, static_cast<void*>(webServer), serverThreadStack, serverThreadStackSize, 0x3f, -2);
    if (R_FAILED(r))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_BadInput));

        // Stop the web server
        webServer->Stop();

        // Stop the album wrapper
        //nxgallery::AlbumWrapper::Get()->Shutdown();
        return 1;
    }

    // Start the thread after we created it
    r = threadStart(&serverThread);
    if (R_FAILED(r))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_BadInput));

        // Stop the web server
        webServer->Stop();

        // Stop the album wrapper
        //nxgallery::AlbumWrapper::Get()->Shutdown();
        return 1;
    }

    // Now, this will completely block the execution of this sysmodule until the thread exits
    r = threadWaitForExit(&serverThread);
    if (R_FAILED(r))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_BadInput));

        // Stop the web server
        webServer->Stop();

        // Stop the album wrapper
        //nxgallery::AlbumWrapper::Get()->Shutdown();
        return 1;
    }

    // Close the thread because this part of the code is only reached when the thread exited already
    r = threadClose(&serverThread);
    if (R_FAILED(r))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_BadInput));

        // Stop the web server
        webServer->Stop();

        // Stop the album wrapper
        //nxgallery::AlbumWrapper::Get()->Shutdown();
        return 1;
    }

    return 0;
}
