#pragma once
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

inline std::vector<std::string> consoleOutput; 
inline std::vector<std::string> IL2CPP_Games;
inline std::vector<std::string> filterFunctions;

inline bool ClientDebugMode = true;
inline bool RenderCursor = false;

inline bool GameChosen = false;
inline bool GameChosenPopup = false;

inline bool DriveChosen = false;
inline bool DriveChosenPopup = false;

inline bool filterFunctionsPopup = false;
inline bool FunctionFilters = false;

inline bool GamesAlreadyScanned = false;

inline bool GameDataTransferred = false;

inline bool GameDumped = false;
inline bool ScriptSplit = false;



inline std::string ChosenDriveString;
inline std::string IntermediatePath;
inline std::string GamesScanPath;

inline std::string GameChosenString;
inline std::string GameChosenPath;
inline std::string GameDataPath;
inline std::string GameMetadataPath;
inline std::string GameDLLPath;

inline std::string IL2CPPDumperPath;
inline std::string PWNOutputPath;
inline std::string PWNJunkPath;
inline std::string JunkScriptMethodPath;
inline std::string JunkScriptMetadataPath;
inline std::string DumperOutputPath;
inline std::string WorkingDir;

