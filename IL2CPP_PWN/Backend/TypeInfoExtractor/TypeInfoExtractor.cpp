#include "TypeInfoExtractor.h"
#include "../../Include/Globals.h"
#include <fstream>
#include "../../Menu/Utility.h"

TypeInfoExtractor::TypeInfoExtractor(std::string jsonFilePath) : jsonFilePath(jsonFilePath)
{

}

bool TypeInfoExtractor::cleanseJsonFile() 
{
    std::ifstream file(jsonFilePath);

    if (!file.is_open()) 
    {
        LogToConsole("Error Applying Soft-Filter to JUNK TypeInfo File.");
        return false;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    auto it = jsonData.begin();
    while (it != jsonData.end()) 
    {
        std::string name = (*it)["Name"];
        if (name.substr(0, 7) == "System.") 
        {
            it = jsonData.erase(it);
        }
        else
        {
            ++it;
        }
    }

    std::ofstream outputFile(jsonFilePath);

    if (!outputFile.is_open()) 
    {
        LogToConsole("Error Applying Soft-Filter to JUNK TypeInfo File.");
        return false;
    }

    outputFile << jsonData;
    outputFile.close();
}

bool TypeInfoExtractor::parseJsonFile() 
{
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) 
    {
        LogToConsole("Error Serializing JUNK JSON file.");
        return false;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    for (const auto& element : jsonData)
    {
        JTypeInfo info;
        info.Address = element["Address"];
        info.Name = element["Name"];
        typeInfoList.push_back(info);
    }

    return true;
}



bool TypeInfoExtractor::saveJTypeInfoToJson(std::string outputPath) 
{
    json jsonData;

    for (const auto& info : typeInfoList) 
    {
        json infoData;
        infoData["address"] = info.Address;
        infoData["name"] = info.Name;
        jsonData.push_back(infoData);
    }

    std::ofstream outputFile(outputPath);

    if (!outputFile.is_open()) 
    {
        LogToConsole("Error Saving TypeInfoDump to JSON File.");
        return false;
    }

    outputFile << jsonData.dump(4);
    outputFile.close();
}