#include "Menu.h"
#include "MenuBackend.h"
#include <iostream>
#include <vector>
#include <filesystem>

LPCSTR Drawing::lpWindowName = "IL2CPP PWN";
ImVec2 Drawing::vWindowSize = { 1600, 600 };
ImGuiWindowFlags Drawing::WindowFlags = 0;
bool Drawing::bDraw = true;
bool Drawing::OutputFolderChose = false;
char PopupChooseFolder[256] = "";
bool Drawing::ChooseFolderPopup;
std::string ChosenOutPutFolder;

void Log(std::string stdmessage)
{
	consoleOutput.push_back(stdmessage);
}

void ClearConsole()
{
	consoleOutput.clear();
}


void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

void Drawing::Draw()
{
	if (isActive())
	{

		DoStyle();

		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::Begin(lpWindowName, &bDraw, WindowFlags);
		{

			if (ImGui::BeginTabBar("TabBar")) 
			{
				if (ImGui::BeginTabItem("Dumper")) 
				{
					renderDumperTab();
					
						if (ImGui::Button("Skip To Function Finder"))
						{
							GameChosen = true;
							GameDumped = true;
							ScriptSplit = true;
							DriveChosen = true;
						}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Script Splitter")) 
				{
					renderScriptSplitterTab();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Function Finder")) 
				{
					renderFunctionFinderTab();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("TypeInfo Extractor")) 
				{
					renderTypeInfoExtractorTab();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			
			if (ImGui::Button("Clear Console"))
			{
				consoleOutput.clear();
			}

			renderConsole();

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}

}
