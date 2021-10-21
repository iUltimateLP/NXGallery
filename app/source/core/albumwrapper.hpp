/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)

    MIT License

    Copyright (c) 2020-2021 Jonathan Verbeek

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <switch.h>

// Defines how many items should be returned per page
#define CONTENT_PER_PAGE 21

// Screenshot locations
#define PATH_NAND "nand:/Album"
#define PATH_SD "sdmc:/Nintendo/Album"
#define PATH_SD_EMUMMC "sdmc:/emuMMC/RAW1/Nintendo/Album"
#define PATH_SD_EMUMMC_FILEBASED "sdmc:/emuMMC/SD00/Nintendo/Album"

namespace nxgallery::core
{
    // Type of homebrew installation
    enum HomebrewInstallationType
    {
        Nand,
        SD_EmuMMC,
        SD_EmuMMC_Filebased
    };

    // This class is responsible for reading videos as we access them
    // in a stream-like format through capsa. 
    // Full credit goes to HookedBehemoth and his ShareNX project :)
    class VideoStreamReader
    {
    public:
        // Constructor and destructor
        VideoStreamReader(const CapsAlbumEntry& inAlbumEntry);
        ~VideoStreamReader();

        // Returns the size of the whole video stream (= size of the video file)
        u64 GetStreamSize();

        // Reads a number of bytes from the stream. Won't read more data than the internal
        // workbuffer allows
        u64 Read(char* outBuffer, u64 numBytes);

    private:
        // The album entry we're reading video data for
        CapsAlbumEntry albumEntry;

        // Size of the work buffer
        u64 workBufferSize = 0x40000;

        // The work buffer
        unsigned char* workBuffer;

        // Handle for the movie stream, returned by capsaOpenAlbumMovieStream
        u64 streamHandle = 0;

        // How many bytes have been read until now
        u64 bytesRead = 0;

        // Size of the stream, returned by capsaGetAlbumMovieStreamSize and cached at start
        u64 streamSize = 0;

        // Index of the last work buffer
        u32 lastBufferIndex = -1;
    };

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

        // Returns the album entry for a given ID (useful for querying type)
        CapsAlbumFileContents GetAlbumEntryType(int id);

        // Returns the raw file content of a file's thumbnail (JPEG)
        bool GetFileThumbnail(int id, void* outBuffer, u64 bufferSize, u64* outActualImageSize);

        // Returns the raw file content of a file's main content file (JPEG/mp4)
        bool GetFileContent(int id, void* outBuffer, u64 bufferSize, u64* outActualFileSize);

    private:
        // Cache all gallery images upon startup and store them so we won't have
        // to look up the gallery content everytime a request happens
        void CacheGalleryContent();

        // Caches a specified album in a specified cache
        void CacheAlbum(CapsAlbumStorage location, std::vector<CapsAlbumEntry>& outCache);

        // Returns the directory of the screenshot files based on the current installation type
        std::string GetScreenshotDir();

    private:
        // Holds the filesystem of the internal NAND storage
        FsFileSystem nandFileSystem;

        // Stores the type of installation NXGallery runs on
        HomebrewInstallationType installationType;

        // Holds all album content in cache
        std::vector<CapsAlbumEntry> cachedAlbumContent;

        // Singleton instance of the AlbumWrapper
        static AlbumWrapper* singleton;
    };
}
