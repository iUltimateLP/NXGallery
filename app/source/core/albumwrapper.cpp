/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)

    MIT License

    Copyright (c) 2020-2022 Jonathan Verbeek

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

#include "albumwrapper.hpp"
#include "json.hpp"
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>
#include <dirent.h>
#include <filesystem>
#include <regex>
using namespace nxgallery::core;
using json = nlohmann::json;

// Needed for compiler
CAlbumWrapper* CAlbumWrapper::singleton = NULL;

// Overwritten == operator to compare CapsAlbumEntry's
inline bool operator==(CapsAlbumEntry lhs, const CapsAlbumEntry rhs)
{
    // Just compare by the datetime
    return lhs.file_id.datetime.day == rhs.file_id.datetime.day
        && lhs.file_id.datetime.month == rhs.file_id.datetime.month
        && lhs.file_id.datetime.year == rhs.file_id.datetime.year
        && lhs.file_id.datetime.hour == rhs.file_id.datetime.hour
        && lhs.file_id.datetime.minute == rhs.file_id.datetime.minute
        && lhs.file_id.datetime.second == rhs.file_id.datetime.second
        && lhs.file_id.datetime.id == rhs.file_id.datetime.id;
}

CVideoStreamReader::CVideoStreamReader(const CapsAlbumEntry& inAlbumEntry)
    : albumEntry(inAlbumEntry)
{
    // Open video stream
    if (R_FAILED(capsaOpenAlbumMovieStream(&streamHandle, &albumEntry.file_id)))
    {
        printf("Failed to open video stream!\n");
        return;
    }

    // Initialize the buffer
    workBuffer = (unsigned char*)calloc(workBufferSize, sizeof(unsigned char));

    // Get the size the stream will be
    capsaGetAlbumMovieStreamSize(streamHandle, &streamSize);
}

CVideoStreamReader::~CVideoStreamReader()
{
    // Free the read buffer
    free(workBuffer);

    // Close the stream
    capsaCloseAlbumMovieStream(streamHandle);
}

u64 CVideoStreamReader::GetStreamSize()
{
    // If the stream size is bigger than INT_MAX, it's probably invalid and we'll return 0
    if (streamSize > 0x80000000) return 0;

    return streamSize;
}

u64 CVideoStreamReader::Read(char* outBuffer, u64 numBytes)
{
    // Calculate a few statistics
    u64 remaining = streamSize - bytesRead;
    u64 bufferIndex = bytesRead / workBufferSize;
    u64 currentOffset = bytesRead % workBufferSize;
    u64 readSize = std::min(std::min(numBytes, workBufferSize - currentOffset), remaining);
    
    // Could debug the video stream read progress here
    //float percentage = ((float)bytesRead / (float)streamSize) * 100;
    //printf("Progress: %f\n", percentage);

    // If no data is remaining, exit
    if (remaining <= 0) return 0;

    // Read the next buffer
    Result readResult = 0;
    if (bufferIndex != lastBufferIndex)
    {
        u64 actualSize = 0;
        readResult = capsaReadMovieDataFromAlbumMovieReadStream(streamHandle, bufferIndex * workBufferSize, workBuffer, workBufferSize, &actualSize);
        lastBufferIndex = bufferIndex;
    }

    // Copy from the work buffer to the the output buffer
    unsigned char* startOutBuffer = workBuffer + currentOffset;
    memcpy(outBuffer, startOutBuffer, readSize);

    // Keep track of progress
    bytesRead += readSize;

    // If it worked, return the number of bytes we just read, otherwise return 0
    if (R_SUCCEEDED(readResult))
        return readSize;
    else 
    {
        printf("Error reading movie stream!");
        return 0;
    }
}

CAlbumWrapper* CAlbumWrapper::Get()
{
    // If no singleton is existing, create a new instance
    if (!singleton)
    {
        singleton = new CAlbumWrapper();
    }

    // Return the instance
    return singleton;
}

void CAlbumWrapper::Init()
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
    
    // Cache the gallery content
    CacheGalleryContent();
}

void CAlbumWrapper::Shutdown()
{
    // Unmount the NAND storage
    int r = fsdevUnmountDevice("nand");
    if (r < 0)
    {
        printf("Error unmounting NAND storage!\n");
    }
}

std::string CAlbumWrapper::GetGalleryContent(int page)
{
    // Will hold the stringified JSON data
    std::string outJSON;

    // Holds the stat object we pass along
    json statObject;

    // Take the time we need so we can display a cool stat
    auto startTime = std::chrono::steady_clock::now();

    /*
        A "gallery content" object should look like this:
        {
            "path": "/2020/05/04/2020050410190700-02CB906EA538A35643C1E1484C4B947D.jpg",
            "takenAt": "1589444763",
            "game": "0100000000010000" OR "Super Mario Odyssey",
            "storedAt": "nand" / "sd",
            "type": "screenshot" / "movie"
        }
    */

    // New json object which will hold the total response
    json finalObject;

    // Fill in some data for the frontend
    // Calculate the max amount of pages we will have
    finalObject["pages"] = (int)ceil((double)cachedAlbumContent.size() / (double)CONTENT_PER_PAGE);

    // Get the console's color theme so the frontend can fit
    ColorSetId colorTheme;
    Result r = setsysGetColorSetId(&colorTheme);
    if (R_SUCCEEDED(r))
    {
        finalObject["theme"] = colorTheme == ColorSetId_Dark ? "dark" : "light";
    }
    
    // Will hold the album contents
    json jsonArray = json::array();

    // Calculate the page range
    int pageMin = (page - 1) * CONTENT_PER_PAGE;
    int pageMax = page * CONTENT_PER_PAGE;

    // Make sure to stay in bounds
    if (pageMax >= cachedAlbumContent.size())
        pageMax = cachedAlbumContent.size();

    // Iterate over the current range for the page
    for (int i = pageMin; i < pageMax; i++)
    {
        // Get the entry off the album content cache
        CapsAlbumEntry albumEntry = cachedAlbumContent[i];

        // Figure out if this album entry is stored on the NAND of SD by looking at the
        // vector this element is in
        bool isStoredInNand = albumEntry.file_id.storage != CapsAlbumStorage_Sd;

        // The JSON object for this entry of the album
        json jsonObj;
        jsonObj["id"] = i;
        jsonObj["storedAt"] = isStoredInNand ? "nand" : "sd";

        // Retrieve the title's name
        jsonObj["game"] = GetTitleName(albumEntry.file_id.application_id);

        // Retrieve the file size of the file
        u64 fileSize;
        Result getFileSizeResult = capsaGetAlbumFileSize(&albumEntry.file_id, &fileSize);
        if (R_SUCCEEDED(getFileSizeResult))
        {
            jsonObj["fileSize"] = fileSize;
        }

        // Create a UNIX-timestamp from the datetime we get from capsa
        struct tm createdAt;
        createdAt.tm_year = albumEntry.file_id.datetime.year - 1900;
        createdAt.tm_mon = albumEntry.file_id.datetime.month - 1;
        createdAt.tm_mday = albumEntry.file_id.datetime.day;
        createdAt.tm_hour = albumEntry.file_id.datetime.hour;
        createdAt.tm_min = albumEntry.file_id.datetime.minute;
        createdAt.tm_sec = albumEntry.file_id.datetime.second;
        time_t timestamp = mktime(&createdAt);
        jsonObj["takenAt"] = timestamp;

        // Determine the type by looking at the file extension
        if (albumEntry.file_id.content == CapsAlbumFileContents_Movie || albumEntry.file_id.content == CapsAlbumFileContents_ExtraMovie)
        {
            jsonObj["type"] = "video";
        }
        else
        {
            jsonObj["type"] = "screenshot";
        }

        // Add the filename for downloading
        jsonObj["fileName"] = nxgallery::core::CAlbumWrapper::Get()->GetAlbumEntryFilename(i);
        
        // Push this json object to the array
        jsonArray.push_back(jsonObj);
    }

    finalObject["gallery"] = jsonArray;

    // Stop the time!
    auto endTime = std::chrono::steady_clock::now();
    double indexTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();

    // Attach the stats
    statObject["indexTime"] = indexTime;
    statObject["numScreenshots"] = screenshotCount;
    statObject["numVideos"] = videoCount;
    finalObject["stats"] = statObject;

    // Stringify the JSON array
    outJSON = finalObject.dump();

    return outJSON;
}

std::string CAlbumWrapper::GetTitleName(u64 titleId)
{
    // Retrieve the control.nacp data for the game where the current album entry was taken
    NsApplicationControlData nacpData;
    u64 nacpDataSize;
    Result getNACPResult = nsGetApplicationControlData(NsApplicationControlSource_Storage, titleId, &nacpData, sizeof(nacpData), &nacpDataSize);
    if (R_SUCCEEDED(getNACPResult))
    {
        // Retrieve the language string for the Switch'es desired language
        NacpLanguageEntry* nacpLangEntry;
        Result getLangEntryResult = nsGetApplicationDesiredLanguage(&nacpData.nacp, &nacpLangEntry);

        // If it worked, we can grab the nacpLangEntry->name which will hold the name of the title
        if (R_SUCCEEDED(getLangEntryResult))
        {
            return nacpLangEntry->name;
        }
    }

    // If the above didn't work, it's probably not a game where this album entry was created
    // A list of all system applets mapped to their title ID
    // https://switchbrew.org/wiki/Title_list#System_Applets
    std::map<u64, const char*> systemTitles;
    systemTitles[0x0100000000001000] = "Home Menu"; // qlaunch
    systemTitles[0x0100000000001001] = "Auth"; // auth
    systemTitles[0x0100000000001002] = "Cabinet"; // cabinet
    systemTitles[0x0100000000001003] = "Controller"; // controller
    systemTitles[0x0100000000001004] = "DataErase"; // dataErase
    systemTitles[0x0100000000001005] = "Error"; // error
    systemTitles[0x0100000000001006] = "Net Connect"; // netConnect
    systemTitles[0x0100000000001007] = "Player Select"; // playerSelect
    systemTitles[0x0100000000001008] = "Keyboard"; // swkbd
    systemTitles[0x0100000000001009] = "Mii Editor"; // miiEdit
    systemTitles[0x010000000000100A] = "Web Browser"; // web
    systemTitles[0x010000000000100B] = "eShop"; // shop
    systemTitles[0x010000000000100C] = "Overlay"; // overlayDisp
    systemTitles[0x010000000000100D] = "Album"; // photoViewer
    systemTitles[0x010000000000100F] = "Offline Web Browser"; // offlineWeb
    systemTitles[0x0100000000001010] = "Share"; // loginShare
    systemTitles[0x0100000000001011] = "WiFi Web Auth"; // wifiWebAuth
    systemTitles[0x0100000000001012] = "Starter"; // starter
    systemTitles[0x0100000000001013] = "My Page"; // myPage

    // If it's one of the above, lookup the name from the map, otherwise it's Unknown
    if (systemTitles.count(titleId))
    {
        return systemTitles[titleId];
    }
    else
    {
        return "Unknown";
    }
}

CapsAlbumFileContents CAlbumWrapper::GetAlbumEntryType(int id)
{
    return (CapsAlbumFileContents)cachedAlbumContent[id].file_id.content;
}

std::string CAlbumWrapper::GetAlbumEntryFilename(int id)
{
    // Get the entry from cache
    CapsAlbumEntry albumEntry = cachedAlbumContent[id];

    // Get the file and check if it's a video
    CapsAlbumFileContents fileType = GetAlbumEntryType(id);
    bool isVideo = (fileType == CapsAlbumFileContents_Movie || fileType == CapsAlbumFileContents_ExtraMovie);
    std::string extension = isVideo ? ".mp4" : ".jpg";

    // Get the title name
    std::string titleName = GetTitleName(albumEntry.file_id.application_id);

    // Do some regex to replace whitespaces
    titleName = std::regex_replace(titleName, std::regex("\\s+"), "_");

    // Get the date and form a string
    char dateStr[32];
    sprintf(dateStr, "%04u%02u%02u_%02u%02u%02u_%02u",
        albumEntry.file_id.datetime.year,
        albumEntry.file_id.datetime.month,
        albumEntry.file_id.datetime.day,
        albumEntry.file_id.datetime.hour,
        albumEntry.file_id.datetime.minute,
        albumEntry.file_id.datetime.second,
        albumEntry.file_id.datetime.id);

    std::string finalName = titleName + "_" + dateStr + extension;

    return finalName;
}

bool CAlbumWrapper::GetFileThumbnail(int id, void* outBuffer, u64 bufferSize, u64* outActualImageSize)
{
    // Make sure that ID exists
    if (id < 0 || id > cachedAlbumContent.size())
        return false;

    // Get the content with that ID
    CapsAlbumEntry entry = cachedAlbumContent[id];

    // Load the thumbnail
    Result result = capsaLoadAlbumFileThumbnail(&entry.file_id, outActualImageSize, outBuffer, bufferSize);
    if (R_SUCCEEDED(result))
    {
        return true;
    }
    else
    {
        printf("Failed to get thumbnail for file %d: %d-%d\n", id, R_MODULE(result), R_DESCRIPTION(result));
        return false;
    }
}

bool CAlbumWrapper::GetFileContent(int id, void* outBuffer, u64 bufferSize, u64* outActualFileSize)
{
    // Make sure that ID exists
    if (id < 0 || id > cachedAlbumContent.size())
        return false;

    // Get the content with that ID
    CapsAlbumEntry entry = cachedAlbumContent[id];

    // If it's a screenshot, we can use capsaLoadAlbumFile to retrieve it
    if (entry.file_id.content == CapsAlbumFileContents_ScreenShot || entry.file_id.content == CapsAlbumFileContents_ExtraScreenShot)
    {
        // Load the file content
        Result result = capsaLoadAlbumFile(&entry.file_id, outActualFileSize, outBuffer, bufferSize);
        if (R_SUCCEEDED(result))
        {
            return true;
        }
        else
        {
            printf("Failed to get file content for file %d: %d-%d\n", id, R_MODULE(result), R_DESCRIPTION(result));
            return false; 
        }
    }
    else
    {
        // It's a video, use our movie stream reader
        CVideoStreamReader* videoStreamReader = new CVideoStreamReader(entry);

        // Get the stream size. Using that we also know how much bytes we need to read in total
        *outActualFileSize = videoStreamReader->GetStreamSize();
        u64 readLeft = videoStreamReader->GetStreamSize();

        // Read until no bytes are left to read
        size_t bytesRead = 0;
        while (readLeft > 0)
        {
            // Offset the buffer's pointer accordingly
            char* ptr = ((char*)outBuffer) + bytesRead;
            u64 readResult = videoStreamReader->Read(ptr, videoStreamReader->GetStreamSize());

            bytesRead += readResult;
            readLeft -= readResult;
        }

        // Finished, delete the stream reader
        delete videoStreamReader;

        // If no bytes are left to read, everything worked fine
        return readLeft <= 0;
    }
}

void CAlbumWrapper::CacheGalleryContent()
{  
    // Cache NAND album
    CacheAlbum(CapsAlbumStorage_Nand, cachedAlbumContent);

    // Cache SD album
    CacheAlbum(CapsAlbumStorage_Sd, cachedAlbumContent);

    // Reverse the album entries so newest are first
    std::reverse(cachedAlbumContent.begin(), cachedAlbumContent.end());
}

void CAlbumWrapper::CacheAlbum(CapsAlbumStorage location, std::vector<CapsAlbumEntry>& outCache)
{    
    // This uses capsa (Capture Service), which is the libnx service to data from the Switch album

    // Get the total amount of files in the album
    u64 totalAlbumFileCount;
    capsaGetAlbumFileCount(location, &totalAlbumFileCount);

    // Get all album files from the album
    u64 albumFileCount;
    CapsAlbumEntry albumFiles[totalAlbumFileCount];
    Result r = capsaGetAlbumFileList(location, &albumFileCount, albumFiles, totalAlbumFileCount);
    
    if (R_FAILED(r))
    {
        printf("Failed to get album file list for storage %s: %d-%d\n", location == CapsAlbumStorage_Sd ? "SD" : "NAND", R_MODULE(r), R_DESCRIPTION(r));
        return;
    }

    // Add the files from the raw array to the std::vector
    for (CapsAlbumEntry entry : albumFiles)
    {
        // Count
        if (entry.file_id.content == CapsAlbumFileContents_Movie || entry.file_id.content == CapsAlbumFileContents_ExtraMovie)
            videoCount++;
        else
            screenshotCount++;

        // Add to the cache
        outCache.push_back(entry);
    }

#ifdef __DEBUG__
    printf("Cached %ld album files for %s storage\n", totalAlbumFileCount, location == CapsAlbumStorage_Sd ? "SD" : "NAND");
#endif
}
