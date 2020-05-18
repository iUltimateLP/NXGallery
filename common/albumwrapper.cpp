/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "albumwrapper.hpp"
#include "json.hpp"
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>
#include <dirent.h>
using namespace nxgallery;
using json = nlohmann::json;

// Needed for compiler
AlbumWrapper* AlbumWrapper::singleton = NULL;

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

AlbumWrapper* AlbumWrapper::Get()
{
    // If no singleton is existing, create a new instance
    if (!singleton)
    {
        singleton = new AlbumWrapper();
    }

    // Return the instance
    return singleton;
}

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

    // Check to see if this system runs on emuMMC just by checking if the emuMMC folder exists
    struct stat emuMMCstat;
    if (stat(PATH_SD_EMUMMC, &emuMMCstat) < 0)
    {
#ifdef __DEBUG__
        printf("System does NOT run on emuMMC\n");
#endif
        isEmuMMC = false;
    }
    else
    {
#ifdef __DEBUG__
        printf("System runs on emuMMC\n");
#endif
        isEmuMMC = true;
    }
    
    // Cache the gallery content
    CacheGalleryContent();
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
    paths.push_back(PATH_NAND);

    // SD card paths
    paths.push_back(PATH_SD);
    paths.push_back(PATH_SD_EMUMMC);

    return paths;
}

std::string AlbumWrapper::GetGalleryContent(int page)
{
    // Will hold the stringified JSON data
    std::string outJSON;

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

    // Combine both NAND and SD caches into one vector for iterating
    std::vector<CapsAlbumEntry> allAlbumContent;
    allAlbumContent.insert(allAlbumContent.end(), nandAlbumContent.begin(), nandAlbumContent.end());
    allAlbumContent.insert(allAlbumContent.end(), sdAlbumContent.begin(), sdAlbumContent.end());

    // Reverse the content so we have newest items first
    std::reverse(allAlbumContent.begin(), allAlbumContent.end());

    // Fill in some data for the frontend
    // Calculate the max amount of pages we will have
    finalObject["pages"] = (int)ceil((double)allAlbumContent.size() / (double)CONTENT_PER_PAGE);

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
    if (pageMax >= allAlbumContent.size())
        pageMax = allAlbumContent.size();

    // Iterate over the current range for the page
    for (int i = pageMin; i < pageMax; i++)
    {
        // Get the entry off the album content cache
        CapsAlbumEntry albumEntry = allAlbumContent[i];

        // Screenshot files are always named after the following scheme:
        // yyyy/mm/dd/yyyymmddHHMMSSii-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.jpg
        // ii is an index to make sure that multiple screenshots can be taken in the same second
        // After the timestamp, a 32-digit long hash follows. My detective work showed
        // that it's NOT unique per console, and it's changing with the game the screenshot
        // was taken in. Strangely it's not the title ID, but some encryption of it?

        // Figure out if this album entry is stored on the NAND of SD by looking at the
        // vector this element is in
        bool isStoredInNand = std::find(nandAlbumContent.begin(), nandAlbumContent.end(), albumEntry) != nandAlbumContent.end();

        // The JSON object for this entry of the album
        json jsonObj;
        jsonObj["storedAt"] = isStoredInNand ? "nand" : "sd";

        // Retrieve the control.nacp data for the game where the current album entry was taken
        NsApplicationControlData nacpData;
        u64 nacpDataSize;
        Result getNACPResult = nsGetApplicationControlData(NsApplicationControlSource_Storage, albumEntry.file_id.application_id, &nacpData, sizeof(nacpData), &nacpDataSize);
        if (R_SUCCEEDED(getNACPResult))
        {
            // Retrieve the language string for the Switch'es desired language
            NacpLanguageEntry* nacpLangEntry;
            Result getLangEntryResult = nsGetApplicationDesiredLanguage(&nacpData.nacp, &nacpLangEntry);

            // If it worked, we can grab the nacpLangEntry->name which will hold the name of the title
            if (R_SUCCEEDED(getLangEntryResult))
            {
                jsonObj["game"] = nacpLangEntry->name;
            }
        }

        // If the above didn't work, it's probably not a game where this album entry was created
        if (!jsonObj.contains("game")) 
        {
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
            systemTitles[0x0100000000001009] = "miiEdit"; // miiEdit
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
            if (systemTitles.count(albumEntry.file_id.application_id))
            {
                jsonObj["game"] = systemTitles[albumEntry.file_id.application_id];
            }
            else
            {
                jsonObj["game"] = "Unknown";
            }
            
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

        // The folder where to look for the screenshot
        // If the entry we're currently looking at is stored in NAND, take the NAND path, otherwise
        // pick either the SD path or the SD emuMMC path based on where the system is running on
        char imgFolder[64];
        sprintf(imgFolder, "%s/%04d/%02d/%02d/",
            isStoredInNand ? PATH_NAND : (isEmuMMC ? PATH_SD_EMUMMC : PATH_SD),
            albumEntry.file_id.datetime.year,
            albumEntry.file_id.datetime.month,
            albumEntry.file_id.datetime.day);

        // Format a 16-digit long string which looks like the timestamp Nintendo
        // uses to format the filename for screenshots
        char imgFileNameTimestamp[24];
        sprintf(imgFileNameTimestamp, "%04d%02d%02d%02d%02d%02d%02d",
            albumEntry.file_id.datetime.year,
            albumEntry.file_id.datetime.month,
            albumEntry.file_id.datetime.day,
            albumEntry.file_id.datetime.hour,
            albumEntry.file_id.datetime.minute,
            albumEntry.file_id.datetime.second,
            albumEntry.file_id.datetime.id);

        // Since I didn't find out yet how the 32-digit long game ID 
        // in the screenshot filename is constructed, we have to search for
        // a files with starts with the exact timestamp we constructed above
        
        // Open the folder where the image should be located
        DIR* imgDir = opendir(imgFolder);
        
        // Read through the folder until there is no more file to be read
        struct dirent* dirp;
        std::string foundFileName;
        while ((dirp = readdir(imgDir)) != NULL)
        {
            // Get the name of the current file
            std::string fileName = dirp->d_name;

            // Check if the current file name starts with the timestamp we are searching for
            if (fileName.find(imgFileNameTimestamp) == 0)
            {
                // Format the path this image is located (not the real path on the filesystem,
                // but the path where the web server can access it)
                char imgPath[66];
                sprintf(imgPath, "/%04d/%02d/%02d/%s",
                    albumEntry.file_id.datetime.year,
                    albumEntry.file_id.datetime.month,
                    albumEntry.file_id.datetime.day,
                    fileName.c_str());

                foundFileName = fileName;
                jsonObj["path"] = imgPath;
            }
        }

        // Close the directory again
        closedir(imgDir);

        // Determine the type by looking at the file extension
        bool isVideo = foundFileName.compare(foundFileName.length() - 3, 3, "mp4") == 0;
        jsonObj["type"] = isVideo ? "video" : "screenshot";

        // Push this json object to the array
        jsonArray.push_back(jsonObj);
    }

    finalObject["gallery"] = jsonArray;

    // Stringify the JSON array
    outJSON = finalObject.dump();

    return outJSON;
}

void AlbumWrapper::CacheGalleryContent()
{  
    // Cache NAND album
    CacheAlbum(CapsAlbumStorage_Nand, nandAlbumContent);

    // Cache SD album
    CacheAlbum(CapsAlbumStorage_Sd, sdAlbumContent);
}

void AlbumWrapper::CacheAlbum(CapsAlbumStorage location, std::vector<CapsAlbumEntry>& outCache)
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
        outCache.push_back(entry);
    }

#ifdef __DEBUG__
    printf("Cached %ld album files for %s storage\n", totalAlbumFileCount, location == CapsAlbumStorage_Sd ? "SD" : "NAND");
#endif
}
