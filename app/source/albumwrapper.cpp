/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "albumwrapper.hpp"
#include <sys/stat.h>
#include <errno.h>
using namespace nxgallery;

void AlbumWrapper::Init()
{
    // SD card storage will already be mounted thanks to romfsInitialize()

    // Mount the USER partition of the NAND storage by first opening the BIS USER parition
    Result r = fsOpenBisFileSystem(&nandFileSystem, FsBisPartitionId_User, "");
    if (R_FAILED(r))
    {
        printf("Error opening BIS USER partition: %d\n", R_DESCRIPTION(r));
        return;
    }

    // Now mount the opened partition to "nand:/"
    int rc = fsdevMountDevice("nand", nandFileSystem);
    if (rc < 0)
    {
        printf("Error mounting NAND storage!\n");
    }
}

void AlbumWrapper::Shutdown()
{
    // Unmount the NAND storage
    int r = fsdevUnmountDevice("nand");
    if (r < 0)
    {
        printf("Error unmounting NAND storage!\n");
    }
}

std::vector<const char*> AlbumWrapper::GetAlbumContentPaths()
{
    std::vector<const char*> paths;

    // NAND storage paths
    paths.push_back("nand:/Album");

    // SD card paths
    paths.push_back("sdmc:/Nintendo/Album");
    paths.push_back("sdmc:/emuMMC/RAW1/Nintendo/Album");

    return paths;
}
