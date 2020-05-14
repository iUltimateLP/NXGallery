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

    // capsa is the libnx service to data from the Switch album
    u64 albumFileCount;
    CapsAlbumEntry albumFiles[10];
    Result r = capsaGetAlbumFileList(CapsAlbumStorage_Sd, &albumFileCount, albumFiles, 10);

    if (R_FAILED(r))
    {
        printf("Failed to get album file list: %d-%d\n", R_MODULE(r), R_DESCRIPTION(r));
        return outJSON;
    }

    // New json array which will hold all album contents
    json jsonArray = json::array();

    for (CapsAlbumEntry albumEntry : albumFiles)
    {
        // Screenshot files are always named after the following scheme:
        // yyyy/mm/dd/yyyymmddHHMMSSii-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.jpg
        // ii is an index to make sure that multiple screenshots can be taken in the same second
        // After the timestamp, a 32-digit long hash follows. My detective work showed
        // that it's NOT unique per console, and it's changing with the game the screenshot
        // was taken in. Strangely it's not the title ID, but some encryption of it?

        // The JSON object for this entry of the album
        json jsonObj;
        jsonObj["type"] = "screenshot";
        jsonObj["storedAt"] = "sd";

        // Parse out the app title ID to a 16-digit hex string
        char titleID[17];
        sprintf(titleID, "%016" PRIX64, albumEntry.file_id.application_id);
        jsonObj["game"] = titleID;
        
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
        char imgFolder[64];
        sprintf(imgFolder, "sdmc:/emuMMC/RAW1/Nintendo/Album/%04d/%02d/%02d/",
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

                jsonObj["path"] = imgPath;
            }
        }

        // Close the directory again
        closedir(imgDir);

        // Push this json object to the array
        jsonArray.push_back(jsonObj);
    }

    // Stringify the JSON array
    outJSON = jsonArray.dump();

    printf(outJSON.c_str());

    return outJSON;
}
