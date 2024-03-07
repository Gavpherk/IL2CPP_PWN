#include "FunctionFinder.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <locale>
#include "../../Menu/Utility.h"
#include "../../Include/Globals.h"

FunctionFinder::FunctionFinder(std::string jsonFilePath) : jsonFilePath(jsonFilePath)
{

}

bool isEnglishCharacter(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        (c == '_') || (c == '$') || (c == '-') || (c == '.');
}

int FunctionFinder::softFilter() //removes any elems containing non english or non ascii chars. runs on JUNK_ScriptMethod.json
{
    int elementsRemoved = 0;

    LogToConsole("FunctionFetcher JSONFilePath: " + jsonFilePath, true);

    std::ifstream inputFile(jsonFilePath);
    if (!inputFile.is_open()) 
    {
        LogToConsole("Error Performing soft-filter on Junk Method JSON File.");
        return elementsRemoved;
    }

    json jsonData;
    inputFile >> jsonData;
    inputFile.close();

    json newJsonData;

    for (const auto& element : jsonData)
    {
        std::string name = element["Name"];

        if (!(name.substr(0, 7) == "System." ||
            name.substr(0, 7) == "Interop" ||
            name.substr(0, 9) == "Microsoft" ||
            name.substr(0, 9) == "Internal." || 
            name.substr(0, 11) == "Steamworks." ||
            name.substr(0, 5) == "Mono.") &&
            std::all_of(name.begin(), name.end(), [](unsigned char c)
                { return c < 128 && isEnglishCharacter(c); }))
        {
            newJsonData.push_back(element);
        }
        else
        {
            ++elementsRemoved;
        }
    }

    jsonData = std::move(newJsonData);

    std::ofstream outputFile(jsonFilePath);

    if (!outputFile.is_open()) 
    {
        LogToConsole("Error Performing soft-filter on Junk Method JSON File.");
        return elementsRemoved;
    }

    outputFile << jsonData;
    outputFile.close();

    return elementsRemoved;
}

std::pair<std::string, std::string> FunctionFinder::splitName(std::string name)
{
    std::pair<std::string, std::string> result;


    size_t dividerPos = name.find("$$");


    if (dividerPos != std::string::npos)
    {
        result.first = name.substr(0, dividerPos); // Class name
        result.second = name.substr(dividerPos + 2); // Method name 
    }
    else
    {
        result.first = "Not";
        result.second = "Found";
    }

    return result;
}

bool FunctionFinder::parseJsonFile(std::string jsonFilePath) 
{

    std::ifstream file(jsonFilePath);

    if (!file.is_open()) 
    {
        LogToConsole("Failure Parsing Junk Method JSON.");
        return false;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    for (const auto& element : jsonData) 
    {
        JFunction* function = new JFunction;

        function->Address = element["Address"];
        std::string name = element["Name"];

        std::pair<std::string, std::string> ClassMethod;
        ClassMethod = splitName(name);

        if (FunctionFilters && !filterFunctions.empty())
        {
            if (std::find(filterFunctions.begin(), filterFunctions.end(), ClassMethod.first) == filterFunctions.end()) 
            {
                continue; 
            }
        }

        function->Class = ClassMethod.first;
        function->Method = ClassMethod.second;
        function->TypeSignature = element["TypeSignature"];

        FinalFunctionsList.push_back(function);
    }

    return true;
}

bool FunctionFinder::dumpJFunctionsToJson(std::string outputPath)
{

    json jsonData;
   
    for (const auto& function : FinalFunctionsList) 
    {
        json functionData;
        functionData["Address"] = function->Address;
        functionData["Class"] = function->Class;
        functionData["Method"] = function->Method;
        functionData["TypeSignature"] = function->TypeSignature;

        jsonData.push_back(functionData);
    }
    std::string FunctionFinderOutputPath = outputPath + "\\ScriptMethod.json";
    LogToConsole("FunctionFinder Final Output Path: " + FunctionFinderOutputPath, true);
    std::ofstream outputFile(FunctionFinderOutputPath);

    if (!outputFile.is_open())
    {
        LogToConsole("Failure Saving Dumped Functions To A JSON File.");
        return false;
    }

    outputFile << jsonData.dump(4); 
    outputFile.close();

    FinalFunctionsList.clear();
    return true;
}


std::vector<std::string> FunctionFinder::parseStringArrayJson(std::string jsonFilePath) 
{
    std::vector<std::string> stringArray;

    std::ifstream file(jsonFilePath);

    if (!file.is_open()) 
    {
        LogToConsole("Error Importing Classes, turn off Function Filters.");
        return stringArray;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    if (!jsonData.is_array()) 
    {
        LogToConsole("Error Importing Classes, turn off Function Filters.");
        return stringArray;
    }

    for (const auto& element : jsonData) 
    {
        if (element.is_string()) 
        {
            stringArray.push_back(element);
        }
    }

    return stringArray;
}



bool FunctionFinder::saveFilterFunctionsToJson(std::string outputPath) 
{
    json jsonData;

    for (const auto& filterFunction : filterFunctions) 
    {
        jsonData.push_back(filterFunction);
    }

    LogToConsole("importClasses.json calculated save path: " + outputPath, true);
    std::ofstream outputFile(outputPath);

    if (!outputFile.is_open()) 
    {
        LogToConsole("Error saving ImportClasses.json file to " + WorkingDir);
        return false;
    }

    outputFile << jsonData.dump(4);
    outputFile.close();
    return true;
}