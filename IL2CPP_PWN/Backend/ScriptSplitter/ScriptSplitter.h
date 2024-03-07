#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

class ScriptSplitter 
{
private:

    std::string programPath;
    std::string inputFilePath;
    std::string outputDirectory;

    std::string getExecutablePath();

public:

    ScriptSplitter();
    bool splitScripts();
};
