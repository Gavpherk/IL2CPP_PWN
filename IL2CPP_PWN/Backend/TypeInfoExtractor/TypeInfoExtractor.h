#pragma once
#include <string>
#include <vector>
#include "../Types/Types.h"

class TypeInfoExtractor 
{

public:
    TypeInfoExtractor(std::string jsonFilePath);

    bool saveJTypeInfoToJson(std::string outputPath);
    bool parseJsonFile();
    bool cleanseJsonFile();


private:
    std::vector<JTypeInfo> typeInfoList;
    std::string jsonFilePath;

};
