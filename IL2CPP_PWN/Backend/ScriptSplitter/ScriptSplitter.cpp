#include "ScriptSplitter.h"
#include <iomanip>
#include "../../Include/Globals.h"
#include <Windows.h>
#include "../../Menu/Utility.h"

std::string ScriptSplitter::getExecutablePath() 
{
    std::string path;
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    path = std::string(buffer);
    size_t pos = path.find_last_of("/\\");
    path = path.substr(0, pos);
    return path;
}

ScriptSplitter::ScriptSplitter() 
{
    programPath = WorkingDir;
    inputFilePath = DumperOutputPath + "\\script.json";
    outputDirectory = PWNJunkPath;
}

bool ScriptSplitter::splitScripts() 
{
    LogToConsole("ScriptSplitter inputFilePath: " + inputFilePath);
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) 
    {
        LogToConsole("Error: Could not open input file " + inputFilePath);
        return false;
    }

    json data;
    inputFile >> data;

    if (data.find("ScriptMetadata") != data.end()) 
    {
        json scriptMetadata = data["ScriptMetadata"];
        json filteredMetadata;

        for (const auto& elem : scriptMetadata)
        {
            if (elem.find("Name") != elem.end() && elem["Name"].is_string()) 
            {
                std::string name = elem["Name"];
                if (name.substr(0, 7) == "System." ||
                    name.substr(0, 7) == "Interop" ||
                    name.substr(0, 9) == "Microsoft" ||
                    name.substr(0, 9) == "Internal." ||
                    name.substr(0, 11) == "Steamworks." ||
                    name.substr(0, 5) == "Mono.")
                {
                    continue;
                }
                  
                if (name.size() >= 9 && name.substr(name.size() - 9) == "_TypeInfo") 
                {
                    filteredMetadata.push_back(elem);
                }
            }
        }

        std::ofstream outputFile(outputDirectory + "\\JUNK_ScriptMetadata.json");
        JunkScriptMetadataPath = outputDirectory + "\\JUNK_ScriptMetadata.json";
        outputFile << std::setw(4) << filteredMetadata << std::endl;
        outputFile.close();
    }

    if (data.find("ScriptMethod") != data.end()) 
    {
        json scriptMethod = data["ScriptMethod"];
        std::ofstream outputFile(outputDirectory + "\\JUNK_ScriptMethod.json");
        JunkScriptMethodPath = outputDirectory + "\\JUNK_ScriptMethod.json";
        outputFile << std::setw(4) << scriptMethod << std::endl;
        outputFile.close();
    }

    LogToConsole("Pulled neccesary information from script.json into respective files, you can now continue to Function Finder.");
    ScriptSplit = true;
    return true;
}
