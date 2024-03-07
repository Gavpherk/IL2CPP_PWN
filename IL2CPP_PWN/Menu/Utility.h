#pragma once
#include <vector>
#include <string>
#include <windows.h>

std::vector<std::string> getAvailableDrives();

void LogToConsole(std::string msg, bool debug = false);

std::string scanForDataFolder(const std::string& path);

void createOutputDirectories();

bool copyGameFiles(const std::string& sourceDLLPath, const std::string& sourceJSONPath, const std::string& destinationDir);

bool DumpGame();
