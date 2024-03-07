#include "Utility.h"
#include "../Include/Globals.h"
#include <filesystem>

std::vector<std::string> getAvailableDrives() 
{
    std::vector<std::string> drives;
    DWORD drivesMask = GetLogicalDrives();
    char driveLetter = 'A';
    for (int i = 0; i < 26; ++i) {
        if (drivesMask & 1) {
            drives.push_back(std::string(1, driveLetter) + ":\\");
        }
        drivesMask >>= 1;
        ++driveLetter;
    }
    return drives;
}

void LogToConsole(std::string msg, bool debug)
{
    if (debug)
    {
        if (ClientDebugMode)
        {
            
            consoleOutput.push_back("[DEBUG] " + msg);
        }
        else
        {
            return;
        }
    }
    else 
    {
        consoleOutput.push_back(msg);
    }
}


std::string scanForDataFolder(const std::string& path) 
{
    if (!std::filesystem::exists(path)) 
    {
        return ""; 
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) 
    {
        if (entry.is_directory()) 
        {
            std::string folderName = entry.path().filename().string();

            if (folderName.size() >= 5 && folderName.substr(folderName.size() - 5) == "_Data") 
            {
               std::string stringPath = entry.path().string();
               std::string returnPath = stringPath + "\\";
               return returnPath;
            }
        }
    }

    return "";
}

void createOutputDirectories() 
{
    std::filesystem::path currentDir = std::filesystem::current_path();
    WorkingDir = currentDir.string();
    IL2CPPDumperPath = WorkingDir + "\\IL2CPPDumper\\IL2CPPDumper.exe";

    std::filesystem::path pwnOutputDir = currentDir / "PWN_OUTPUT";
    if (!std::filesystem::exists(pwnOutputDir))
    {
        std::filesystem::create_directory(pwnOutputDir);
    }
    PWNOutputPath = pwnOutputDir.string();

    std::filesystem::path dumperOutputDir = currentDir / "DUMPER_OUTPUT";
    if (!std::filesystem::exists(dumperOutputDir)) 
    {
        std::filesystem::create_directory(dumperOutputDir);
    }
    DumperOutputPath = dumperOutputDir.string();

    std::filesystem::path junkDir = pwnOutputDir / "JUNK";
    if (!std::filesystem::exists(junkDir)) 
    {
        std::filesystem::create_directory(junkDir);
    }
    PWNJunkPath = junkDir.string();
}


bool copyGameFiles(const std::string& sourceDLLPath, const std::string& sourceDATPath, const std::string& destinationDir) 
{

    if (!std::filesystem::exists(sourceDLLPath) || !std::filesystem::exists(sourceDATPath)) 
    {
        LogToConsole("Source files do not exist.\n" + sourceDLLPath + "\n" + sourceDATPath + "\n" + "These were the computed paths for the files, double check to see if you can find the issue.");
        return false;
    }

    if (!std::filesystem::exists(destinationDir)) 
    {
        LogToConsole("Destination directory does not exist. This means that the Output folders PWN made were tampered with or were failed to be created in the first place (possibly due to location&privilege conflicts)");
        return false;
    }

    std::filesystem::path dllFilename = std::filesystem::path(sourceDLLPath).filename();
    std::filesystem::path jsonFilename = std::filesystem::path(sourceDATPath).filename();

    std::filesystem::path destDLLPath = destinationDir / dllFilename;
    std::filesystem::path destJSONPath = destinationDir / jsonFilename;

    try
    {
        std::filesystem::copy_options options = std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(sourceDLLPath, destDLLPath, options);
        std::filesystem::copy(sourceDATPath, destJSONPath, options);

        if (options == std::filesystem::copy_options::overwrite_existing) 
        {
            LogToConsole("Overwrote Existing PWN Project Files", true);
        }

        LogToConsole("Game Files Copied Succesfully.");
    }

    catch (std::filesystem::filesystem_error& e)
    {
        LogToConsole("Error copying files");
        return false;
    }
}


bool DumpGame()
{

    for (const auto& entry : std::filesystem::directory_iterator(DumperOutputPath)) 
    {
        if (entry.is_regular_file()) 
        {
            std::filesystem::remove(entry.path());
        }
    }

    std::string dllFileName = "GameAssembly.dll";
    std::string metadataFileName = "global-metadata.dat";

    std::string command = "\"" + IL2CPPDumperPath + "\" " + dllFileName + " " + metadataFileName + " " + DumperOutputPath;

    LogToConsole(command, true);
    
    int result = system(command.c_str());

    if (result == 0)
    {
        LogToConsole("Dump Command Successful.", true);
    }
    else
    {
        LogToConsole("Error Executing Dump Command, make sure IL2CPPDumper is located with the program.");
    }
    GameDumped = true;
    return true;
}



