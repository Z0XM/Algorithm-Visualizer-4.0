#pragma once
#include <cstdlib>
#include <fstream>
#include "OneDAlgo.hpp"

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

ONE_D_ALGORITHMS_FOR(Custom)
{
	SETUP_ALGO(0);

	bool isProcessRunning(WCHAR* process)
	{
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return false;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32)){
			CloseHandle(hProcessSnap);
			return false;
		}

		do{
			if (wcscmp(pe32.szExeFile, process) == 0)
				return true;
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return false;
	}

	void onLoad() override
	{
		pg_algo->setInactive();
		pg_head->setActiveRegion(sf::FloatRect(pg_algo->getActiveRegionPosition(), sf::Vector2f(pg_algo->getActiveRegion().width, pg_algo->getActiveRegion().height * 2.f)));

		btn_edit = zui::copy(btn_shuffle);
		placeElement(btn_edit.get(), pg_head.get(), 0, 3, "Edit");

		btn_run = zui::copy(btn_shuffle);
		placeElement(btn_run.get(), pg_head.get(), 0, 4, "Run");
		
		btn_edit->setAction([this]
			{
				std::string cmd =
					"cd \"assets/Custom Algo\" && "
					"subl custom.cpp || code custom.cpp || notepad custom.cpp";

				std::system(cmd.c_str());
			}
		);
		
		btn_run->setAction([this]
			{
				std::string cmd =
					"cd \"assets/Custom Algo\" && "
					"g++ custom.cpp -o custom -I \"../../../SFML GCC x64/SFML/include\" -I \"../../VisVec\" "
					"-L \"../../../SFML GCC x64/SFML/lib\" -lsfml-graphics -lsfml-window -lsfml-system && "
					"custom.exe ";

				std::string vec_as_str(std::to_string(vec.size()));
				for (int i = 0; i < vec.size(); i++) {
					vec_as_str += " " + std::to_string(vec[i]);
				}

				cmd += vec_as_str;

				std::system(cmd.c_str());

				WCHAR process[] = L"custom.exe";

				while (isProcessRunning(process));

				vec.loadAnimationDataFromFile("./assets/Custom Algo/data_output.txt");
			}
		);
	}
	void onUnload() override
	{
		zui::destroy(btn_edit);
		zui::destroy(btn_run);

		std::ofstream file("./assets/Custom Algo/data_output.txt", std::ofstream::trunc);
		file.close();
	}

	zui::Textbox_ptr btn_edit, btn_save, btn_run;
};