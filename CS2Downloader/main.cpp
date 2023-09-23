#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "global.hpp"
#include "downloader.hpp"
#include "CS2Patch.hpp"

std::string wantsMovementPatch;


namespace Operations {
	void download() {
		puts("Preparing Download..");
		Downloader::PrepareDownload();
		puts("Prepared Download!");
		
		Patcher::CleanPatchFiles();
		puts("Starting Download...");
		Downloader::DownloadCS2();
		puts("Finished Download!.");
	}

	void GamePatches() {
		puts("Starting GamePatches...");
		puts("This can take a long time...");
		Downloader::DownloadMods();
		puts("Finished GamePatches!");
	}

	void cleanManifests() {
		puts("Cleaning up downloader files...");
		if (!Globals::usesNoManifests) {
			std::filesystem::remove_all("manifestFiles");
		}
		puts("Cleaned up!");
		puts("Download complete!");
	}

	void patch_files() {
		puts("Starting Patches...");
		Patcher::PatchClient();
		puts("Do you want to install the Patch for the server?");
		puts("Press 'Y' for 'Yes' or 'N' for 'No' on your keyboard");
		std::cin >> wantsMovementPatch;
	
		for (char& c : wantsMovementPatch) { 
			c = std::tolower(c);
		}
	
		if (wantsMovementPatch.find("y") != std::string::npos) {
			Patcher::PatchServer();
		}
	
		puts("Finished Patches!");
	}
}


bool getArgFlag(const char* strInput, const char* strToFind) {
	if (strcmp(strInput, strToFind) == 0) {
		return true;
	}

	return false;
}


int main(int argc, char* argv[]) {
	if (Downloader::needsUpdate()) {
		puts("update required, press enter to download new update.");
		waitforinput();
		Downloader::UpdateInstaller();
	}
	
	for (int i = 1; i < argc; ++i) {
		if ( getArgFlag(argv[i], "disablemanifest") ) {
			Globals::usesNoManifests = true;
		}
	}


	std::string m_inputOperation;
	
	puts("Daived: Thx for use my app");

	puts("What action do you want to perform?");
	puts("download, patch");
	std::cin >> m_inputOperation;

	if (m_inputOperation == "download") {
		Operations::download();
	else if (m_inputOperation == "patch") {
		// Patch files
		Operations::patch_files();

		std::string m_doGamePatches;
		
		puts("Do GamePatches (mods), Y or N");
		std::cin >> m_doGamePatches;

		m_doGamePatches = std::tolower(m_doGamePatches);

		if (m_doGamePatches.find("y") != std::string::npos) {
			Operations::GamePatches();
		}
	}
	
	// Clean up
	Operations::cleanManifests();
	
	puts("Daived: Thanks for use my app, Press 'Enter' to close the installer!");
	waitforinput();
	return 0;
}
