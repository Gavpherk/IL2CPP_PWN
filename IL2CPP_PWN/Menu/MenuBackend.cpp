#include "../pch.h"
#include "MenuBackend.h"
#include "Utility.h"
#include <iostream>
#include <filesystem>
#include "../Backend/ScriptSplitter/ScriptSplitter.h"
#include "../Backend/TypeInfoExtractor/TypeInfoExtractor.h"

std::vector<std::string> scanGames(const std::string& path) 
{
    std::vector<std::string> foundGames;
    if (GamesAlreadyScanned)
        return foundGames;

    if (!std::filesystem::exists(path)) 
    {
        LogToConsole(path + " directory does not exist.");
        return foundGames;
    }
    for (const auto& entry : std::filesystem::directory_iterator(path)) 
    {

        if (entry.is_directory()) 
        {

            std::filesystem::path dllPath = entry.path() / "GameAssembly.dll";
            if (std::filesystem::exists(dllPath)) 
            {
                LogToConsole("Game Found: " + entry.path().filename().string());
                foundGames.push_back(entry.path().filename().string());
            }
        }
    }

    GamesAlreadyScanned = true;
    return foundGames;
}


void renderFunctionFilterPopup() 
{
    ImGui::OpenPopup("Function Filter");

    ImGui::SetNextWindowSize(ImVec2(340, 130), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Function Filter", NULL, ImGuiWindowFlags_NoResize)) 
    {
        static char filterBuffer[256] = "";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Type here...").x) * 0.5f - 60);
        ImGui::InputText("##FunctionFilter", filterBuffer, 256);

        ImGui::NewLine();
        float halfPopupWidth = ImGui::GetWindowSize().x * 0.5f / 2 - 35;
        ImGui::SetCursorPosX(halfPopupWidth);

        if (ImGui::Button("Add"))
        {
            filterFunctions.push_back(filterBuffer);
            LogToConsole("Added " + (std::string)filterBuffer + " Class to the filters. Press the button again to add another.");
            filterFunctionsPopup = false;
            ImGui::CloseCurrentPopup();
        }

        
        ImGui::SameLine();
        if (ImGui::Button("FIXING")) 
        {
            /*
            if (!p_FunctionFinder)
            {
                FunctionFinder g_FunctionFinder(JunkScriptMethodPath);
                p_FunctionFinder = &g_FunctionFinder;
                filterFunctions = g_FunctionFinder.parseStringArrayJson(WorkingDir + "\\importClasses.json");
                LogToConsole("importClasses expected path: " + WorkingDir + "\\importClasses.json");

            }
            else if (p_FunctionFinder)
            {
                filterFunctions = p_FunctionFinder->parseStringArrayJson(WorkingDir + "\\importClasses.json");
                LogToConsole("importClasses expected path: " + WorkingDir + "\\importClasses.json");
            }
            */
        }
        

        if (ImGui::IsItemHovered()) 
        {
            ImGui::SetTooltip("Allows you to import a list of classes from JSON");
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) 
        {
            ImGui::CloseCurrentPopup(); 
            filterFunctionsPopup = false;
        }

        ImGui::EndPopup();
    }
}

void renderChooseGamePopup(std::vector<std::string> games)
{

    ImGui::OpenPopup("Target Game");
    ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal("Target Game", NULL, ImGuiWindowFlags_NoResize))
    {
        for (const std::string& game : games)
        {
            if (ImGui::Button(game.c_str()))
            {
                GameChosenPath = GamesScanPath + game + "\\";
                GameDLLPath = GameChosenPath + "GameAssembly.dll";
                GameDataPath = scanForDataFolder(GameChosenPath);
                GameMetadataPath = GameDataPath + "il2cpp_data\\Metadata\\global-metadata.dat";
                GameChosenString = game;
                GameChosenPopup = false;
                GameChosen = true;
                createOutputDirectories();
                copyGameFiles(GameDLLPath, GameMetadataPath, WorkingDir);
                ImGui::CloseCurrentPopup();
                LogToConsole("IL2CPPDumper Path: " + IL2CPPDumperPath, true);
                LogToConsole("Game Path: " + GameChosenPath, true);
                LogToConsole("DLL Path: " + GameDLLPath, true);
                LogToConsole("Data Path: " + GameDataPath, true);
                LogToConsole("Metadata Path: " + GameMetadataPath, true);
            }
        }
    }
    ImGui::EndPopup();
}

void renderChooseDrivePopup()
{
    ImGui::OpenPopup("Target Drive");
    ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal("Target Drive", NULL, ImGuiWindowFlags_NoResize))
    {
        std::vector<std::string> drives = getAvailableDrives();

        for (const std::string& drive : drives)
        {
            if (ImGui::Button(drive.c_str()))
            {
                ChosenDriveString = drive;
                if (ChosenDriveString == "C:\\")
                {
                    IntermediatePath = "Program Files (x86)\\Steam";
                }
                else
                {
                    IntermediatePath = "SteamLibrary";
                }

                GamesScanPath = ChosenDriveString + IntermediatePath + "\\steamapps\\common\\";
                LogToConsole(GamesScanPath, true);
                DriveChosen = true;
                DriveChosenPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}

void renderDumperTab() 
{
    if (!DriveChosen)
    {
        if (ImGui::Button("Choose Drive"))
        {
            DriveChosenPopup = !DriveChosenPopup;
        }

        if (DriveChosenPopup)
        {
            renderChooseDrivePopup();
        }
    }

    if (DriveChosen)
    {
        if (ImGui::Button("Choose Game"))
        {
            GameChosenPopup = !GameChosenPopup;
        }

        if (GameChosenPopup)
        {
            if (!GamesAlreadyScanned)
            {
                IL2CPP_Games = scanGames(GamesScanPath);
            }

           renderChooseGamePopup(IL2CPP_Games);
        }

    }

    if (GameChosen)
    {
        if (ImGui::Button("Dump"))
        {
            DumpGame();
        }
    }
}

void renderScriptSplitterTab() 
{
    if (ImGui::Button("Split JSON")) 
    {
        ScriptSplitter g_ScriptSplitter;
        g_ScriptSplitter.splitScripts();
    }
}

void renderFunctionFinderTab() 
{

    if (!ScriptSplit)
    {
        ImGui::Text("You must dump the game, and use Script Splitter before continuing to this tab.");
        return;
    }
    
    const char* filterButtonText = "Turn On Filters";

    if (FunctionFilters)
    {
        filterButtonText = "Turn Off Filters";
    }

    if (ImGui::Button(filterButtonText)) 
    {
        FunctionFilters = !FunctionFilters;
    }

    ImGui::SameLine();

    if(ImGui::Button("Explain"))
    {
        LogToConsole("Some games are very large, so we can add filters to only capture methods from the classes we choose, \nfor example if you want access to the unity Transform Class you would specify UnityEngine.Transform, UnityEngine.Camera, GameManager, etc \nrefer to your dumps for more directed information on what to put as your filter.");
    }

    if (FunctionFilters)
    {
        if (ImGui::Button("Add Class"))
        {
            filterFunctionsPopup = !filterFunctionsPopup;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Adds a Class to the filter. Every Class NOT in the filter, will not be imported to the final JSON.");
        }

        if (ImGui::Button("Save Class List To JSON"))
        {
            if (!p_FunctionFinder)
            {
                FunctionFinder g_FunctionFinder(JunkScriptMethodPath);
                p_FunctionFinder = &g_FunctionFinder;
                g_FunctionFinder.saveFilterFunctionsToJson(WorkingDir + "\\importClasses.json");
            }
            else if (p_FunctionFinder)
            {
                p_FunctionFinder->saveFilterFunctionsToJson(WorkingDir + "\\importClasses.json");
            }
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Allows you to import a list of classes from JSON for later uses on the same game.");
        }

        if (filterFunctions.size() != 0)
        {
            if (ImGui::Button("Clear Filter List"))
            {
                filterFunctions.clear();
            }
        }

    }

    if (filterFunctionsPopup)
    {
        renderFunctionFilterPopup();
    }

    if (ImGui::Button("Dump Functions"))
    {
        if (!p_FunctionFinder)
        {
            FunctionFinder g_FunctionFinder(PWNJunkPath + "\\JUNK_ScriptMethod.json");
            p_FunctionFinder = &g_FunctionFinder;
            removedelems = g_FunctionFinder.softFilter();
        }
        else if(p_FunctionFinder)
        { 
            removedelems = p_FunctionFinder->softFilter();       
        }

        std::string removedElems = std::to_string(removedelems);
        LogToConsole("Removed " + removedElems + " from the Method Dump IL2CPP Dumper generated. This was a soft filter For Non-ASCII or Non-English chars, and some Il2CPP/Unity Bloat.");

        if (!p_FunctionFinder->parseJsonFile(PWNJunkPath + "\\JUNK_ScriptMethod.json"))
        {
            LogToConsole("Unknown Error Parsing JUNK JSON Files. " + PWNJunkPath);
        }

        if (!p_FunctionFinder->dumpJFunctionsToJson(PWNOutputPath))
        {
            LogToConsole("Unkown Error Parsing JUNK JSON Files. " + PWNJunkPath);
        }
    }
}

void renderTypeInfoExtractorTab()
{
    if (ImGui::Button("Dump TypeInfo's")) 
    {
        TypeInfoExtractor g_TypeInfoExtractor(PWNJunkPath + "\\JUNK_ScriptMetaData.json");
        g_TypeInfoExtractor.cleanseJsonFile();
        g_TypeInfoExtractor.parseJsonFile();
        g_TypeInfoExtractor.saveJTypeInfoToJson(PWNOutputPath + "\\ScriptMetadata.json");
        LogToConsole("TypeInfo's Dumped.");
        LogToConsole("All Files Meant To Be Used In The Cheat Can Be Found Here: " + PWNOutputPath);
    }
}


void renderConsole() 
{
    ImGui::SetNextWindowPos(ImVec2(540, 270));
    if (ImGui::BeginChild("Console", ImVec2(1250, 500)))
    {
        for (const std::string& output : consoleOutput)
        {
            ImGui::TextUnformatted(output.c_str());
        }
        ImGui::EndChild();
    }
}


void DoStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.70f, 0.18f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.65f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.ChildRounding = 12.00f;
    style.FrameRounding = 12.00f;
    style.GrabRounding = 12.00f;
    style.PopupRounding = 7.00f;
    style.ScrollbarRounding = 9.00f;
    style.TabRounding = 12.00f;
    style.WindowRounding = 12.00f;
    style.WindowPadding = ImVec2(3, 6);
    style.FramePadding = ImVec2(20, 7);
    style.CellPadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(20, 9);
    style.ItemInnerSpacing = ImVec2(10, 13);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 21.00f;
    style.ScrollbarSize = 1.00f;
    style.GrabMinSize = 13.00f;
    style.WindowBorderSize = 1.00f;
    style.ChildBorderSize = 1.00f;
    style.PopupBorderSize = 1.00f;
    style.FrameBorderSize = 1.00f;
    style.TabBorderSize = 1.00;
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    return;
}


void renderCursorPosition()
{
    ImVec2 cursorPos = ImGui::GetMousePos();

    ImGui::SetNextWindowPos(cursorPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Cursor Position", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Cursor Position: (%.1f, %.1f)", cursorPos.x, cursorPos.y);
    ImGui::End();
}
