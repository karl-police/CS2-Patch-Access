#include "CS2Patch.hpp"
#include <Windows.h>
#include <iostream>
#include <filesystem>

bool Patcher::PatchClient() {
    const char* steamCheckBytes[2] = { "\x75\x73\xFF\x15", "\xEB\x73\xFF\x15" };
    const char* versionCheckBytes[4] = { "\x6C\x69\x6D\x69\x74\x65\x64\x62\x65\x74\x61", "\x66\x75\x6C\x6C\x76\x65\x72\x73\x69\x6F\x6E", "\x6C\x69\x6D\x69\x74\x65\x64\x62\x65\x74\x61", "\x66\x75\x6C\x6C\x76\x65\x72\x73\x69\x6F\x6E"};

    if (!ReplaceBytes("game/csgo/bin/win64/client.dll", steamCheckBytes[0], steamCheckBytes[1], 4)) {
        puts("failed to patch steam check");
        return false;
    }

    if (!ReplaceBytes("game/csgo/bin/win64/client.dll", versionCheckBytes[0], versionCheckBytes[1], 11) || !ReplaceBytes("game/csgo/bin/win64/client.dll", versionCheckBytes[2], versionCheckBytes[3], 11)) {
        puts("failed to patch version check");
        return false;
    }

    return true;
}

bool Patcher::PatchServer() {
    const char* clampCheckBytes[2] = { "\x76\x59\xF2\x0F\x10\x4F\x3C", "\xEB\x59\xF2\x0F\x10\x4F\x3C" };

    if (!ReplaceBytes("game/csgo/bin/win64/server.dll", clampCheckBytes[0], clampCheckBytes[1], 7)) {
        puts("failed to patch movement clamp");
        return false;
    }

    return true;
}

void RemoveExistingPatchFiles(const char* path) {
    std::filesystem::path filePath = path;
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}

void CreateOriginalBackup(const char* path) {
    namespace fs = std::filesystem;
    
    fs::path filePath = path;
    if (fs::exists(filePath)) {
        fs::path pathObj(filePath); // Create an instance of fs::path and initialize it with the filePath.
        std::string fileName = pathObj.filename().string();
        std::string fileLoc = pathObj.parent_path().string();

        std::string fullFilePath = fileLoc + "/" + fileName;

        // if not already existing
        if (!fs::exists(fullFilePath + ".backup")) {
            puts( ("Creating backup for: " + fileName).c_str() );
            std::string fileDest = fullFilePath + ".backup";
            
            std::filesystem::copy_file(filePath, fileDest);
        }
    }
}

void Patcher::BackupFiles() {
    CreateOriginalBackup("game/csgo/bin/win64/client.dll");
    CreateOriginalBackup("game/csgo/bin/win64/server.dll");
}

void Patcher::CleanPatchFiles() {
    RemoveExistingPatchFiles("game/csgo/bin/win64/client.dll");
    RemoveExistingPatchFiles("game/csgo/bin/win64/server.dll");
}

bool Patcher::ReplaceBytes(const char* filename, const char* searchPattern, const char* replaceBytes, size_t replaceLength) {
    FILE* file = fopen(filename, "rb+");
    if (!file) {
        printf("failed to open file: %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    unsigned char* buffer = (unsigned char*)malloc(fileSize);
    if (!buffer) {
        printf("failed to found memory.\n");
        fclose(file);
        return false;
    }

    fread(buffer, 1, fileSize, file);

    unsigned char* position = buffer;
    unsigned char* endPosition = buffer + fileSize - replaceLength;

    while (position <= endPosition) {
        if (memcmp(position, searchPattern, replaceLength) == 0) {
            fseek(file, static_cast<long>(position - buffer), SEEK_SET);
            fwrite(replaceBytes, 1, replaceLength, file);
            break;
        }
        position++;
    }

    if (position > endPosition) {
        printf("patch out of date: %s | %i\n", filename, (int)replaceLength);
        return false;
    }

    free(buffer);
    fclose(file);
    return true;
}
