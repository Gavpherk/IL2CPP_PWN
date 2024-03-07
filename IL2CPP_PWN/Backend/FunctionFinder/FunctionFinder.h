#pragma once
#include <string>
#include <vector>
#include "../Types/Types.h"

class FunctionFinder
{
public:

    FunctionFinder(std::string jsonFilePath);
    bool parseJsonFile(std::string jsonFilePath);
    int softFilter();
    bool dumpJFunctionsToJson(std::string outputPath);
    std::vector<std::string> parseStringArrayJson(std::string jsonFilePath);
    bool saveFilterFunctionsToJson(std::string outputPath);

private:

    std::pair<std::string, std::string> splitName(std::string name);
    std::string jsonFilePath;
    std::vector<JFunction*> FinalFunctionsList;
};
