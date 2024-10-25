#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <cstring>

namespace fs = std::filesystem;

const char ARCHIVE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t ARCHIVE_VERSION = 2;
const uint16_t COMPRESSION_CODE = 0; // 0 = no compression
const uint16_t ERROR_CORRECTION_CODE = 0; // 0 = no error correction

struct ArchiveEntry {
    std::string relativePath;
    bool isDirectory;
    uint64_t fileSize;
    std::vector<char> fileData;
};

// Function to traverse the directory and collect files and directories
void collectEntries(const fs::path& dirPath, const fs::path& basePath, std::vector<ArchiveEntry>& entries) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        ArchiveEntry archiveEntry;
        archiveEntry.relativePath = fs::relative(entry.path(), basePath).string();
        archiveEntry.isDirectory = fs::is_directory(entry);
        if (!archiveEntry.isDirectory) {
            archiveEntry.fileSize = fs::file_size(entry);
            std::ifstream file(entry.path(), std::ios::binary);
            archiveEntry.fileData.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        }
        entries.push_back(archiveEntry);
    }
}

// Function to write the archive file
void writeArchive(const std::string& outputFilePath, const std::vector<ArchiveEntry>& entries) {
    std::ofstream archiveFile(outputFilePath, std::ios::binary);
    if (!archiveFile.is_open()) {
        std::cerr << "Error: Unable to open archive file for writing." << std::endl;
        return;
    }

    // Write the archive signature
    archiveFile.write(ARCHIVE_SIGNATURE, 6);

    // Write the archive version
    archiveFile.write(reinterpret_cast<const char*>(&ARCHIVE_VERSION), sizeof(ARCHIVE_VERSION));

        // Write the compression algorithm code (2 bytes)
    archiveFile.write(reinterpret_cast<const char*>(&COMPRESSION_CODE), sizeof(COMPRESSION_CODE));

    // Write the error-correction algorithm code (2 bytes)
    archiveFile.write(reinterpret_cast<const char*>(&ERROR_CORRECTION_CODE), sizeof(ERROR_CORRECTION_CODE));

    // Write the number of entries (files + directories)
    uint64_t numEntries = entries.size();
    archiveFile.write(reinterpret_cast<const char*>(&numEntries), sizeof(numEntries));

    // Write each entry
    for (const auto& entry : entries) {
        uint16_t pathLength = entry.relativePath.size();
        archiveFile.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
        archiveFile.write(entry.relativePath.c_str(), pathLength);

        uint8_t isDirectoryFlag = entry.isDirectory ? 1 : 0;
        archiveFile.write(reinterpret_cast<const char*>(&isDirectoryFlag), sizeof(isDirectoryFlag));

        if (!entry.isDirectory) {
            archiveFile.write(reinterpret_cast<const char*>(&entry.fileSize), sizeof(entry.fileSize));
            archiveFile.write(entry.fileData.data(), entry.fileSize);
        }
    }

    archiveFile.close();
    std::cout << "Archive created successfully!" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory_to_archive> <output_archive_file>" << std::endl;
        return 1;
    }

    fs::path directoryPath = argv[1];
    std::string outputFilePath = argv[2];

    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        std::cerr << "Error: Directory does not exist or is not a directory." << std::endl;
        return 1;
    }

    // Collect all entries (files and directories)
    std::vector<ArchiveEntry> entries;
    collectEntries(directoryPath, directoryPath, entries);

    // Write the archive file
    writeArchive(outputFilePath, entries);

    return 0;
}
