/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <switch.h>

// Defines how many items should be returned per page
#define CONTENT_PER_PAGE 10

// Screenshot locations
#define PATH_NAND "nand:/Album"
#define PATH_SD "sdmc:/Nintendo/Album"
#define PATH_SD_EMUMMC "sdmc:/emuMMC/RAW1/Nintendo/Album"

namespace nxgallery
{
    // This class will help NXGallery with the Switch'es album.
    // It will implement logic from libnx and provide an interface
    // for the server/backend to provide for the frontend.
    class AlbumWrapper
    {
    public:
        // Returns the singleton instance of the album wrapper
        // We use a singleton so the web server can access it
        static AlbumWrapper* Get();

        // Initializes the album wrapper
        void Init();

        // Shuts down the album wrapper
        void Shutdown();

        // Returns all paths where the Switch stores album content
        std::vector<const char*> GetAlbumContentPaths();

        // Basically, the logic behind the /gallery endpoint as a backend API
        // Contains a JSON-stringified array of gallery content
        std::string GetGalleryContent(int page);

    private:
        // Cache all gallery images upon startup and store them so we won't have
        // to look up the gallery content everytime a request happens
        void CacheGalleryContent();

        // Caches a specified album in a specified cache
        void CacheAlbum(CapsAlbumStorage location, std::vector<CapsAlbumEntry>& outCache);

    private:
        // Holds the filesystem of the internal NAND storage
        FsFileSystem nandFileSystem;

        // Stores whether this system runs on SD emuMMC
        bool isEmuMMC = false;

        // Holds all album content in cache
        std::vector<CapsAlbumEntry> nandAlbumContent;
        std::vector<CapsAlbumEntry> sdAlbumContent;

        // Singleton instance of the AlbumWrapper
        static AlbumWrapper* singleton;
    };
}
