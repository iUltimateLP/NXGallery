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

// Taken from libstratosphere for memory management
#define MEMORY_PAGE_SIZE 0x1000
#define THREAD_STACK_ALIGNMENT 4 * 1024 // 4kb

extern "C" 
{
    // Sysmodules should not use applet*
    u32 __nx_applet_type = AppletType_None;

    // Adjust heap size as needed
    #define INNER_HEAP_SIZE (0x1e * MEMORY_PAGE_SIZE)
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
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
    // Will hold the result of several service inits
    Result rc;

    // Initialize default Switch services and make sure it was successful
    rc = smInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    rc = socketInitializeDefault();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_NotInitialized));

    // Initialize the romfs service (needed to serve the static web page from the file system)
    rc = romfsInit();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    // Initialize the filesystem service (needed to access the filesystem - obviously)
    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    // Initialize the capsa service (needed to access the Switch's album)
    rc = capsaInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_NotInitialized));

    // Initialize the setsys service (needed to get the Switch's color theme)
    rc = setsysInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_NotInitialized));
}

// Called (and not needed because this is a sysmodule) when the user tries to exit this app
extern "C" void __attribute__((weak)) userAppExit(void);

// Called when the Switch requests this sysmodule to exit
extern "C" void __attribute__((weak)) __appExit(void)
{
    // Exit the loaded modules in reversed order we loaded them
    setsysExit();
    capsaExit();
    fsExit();
    romfsExit();
    socketExit();
    smExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Initialization code can go here.

    // Your code / main loop goes here.
    // If you need threads, you can use threadCreate etc.

    // Deinitialization and resources clean up code can go here.
    return 0;
}
