#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <cstring>

namespace fs = std::filesystem;

const char ARCHIVE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t ARCHIVE_VERSION = 2;
const uint16_t EXPECTED_COMPRESSION_CODE = 0;
const uint16_t EXPECTED_ERROR_CORRECTION_CODE = 0;

bool extractArchive(const std::string& inputFilePath, const std::string& outputDirPath) {
    std::ifstream archiveFile(inputFilePath, std::ios::binary);
    if (!archiveFile.is_open()) {
        std::cerr << "Error: Unable to open archive file for reading." << std::endl;
        return false;
    }

    // Validate signature
    char fileSignature[6];
    archiveFile.read(fileSignature, 6);
    if (std::memcmp(fileSignature, ARCHIVE_SIGNATURE, 6) != 0) {
        std::cerr << "Error: Invalid archive signature!" << std::endl;
        return false;
    }

    // Read version
    uint16_t version;
    archiveFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != ARCHIVE_VERSION) {
        std::cerr << "Error: Unsupported archive version!" << std::endl;
        return false;
    }

    // Read the compression algorithm code (2 bytes)
    uint16_t compressionCode;
    archiveFile.read(reinterpret_cast<char*>(&compressionCode), sizeof(compressionCode));
    if (compressionCode != EXPECTED_COMPRESSION_CODE) {
        std::cerr << "Error: Unsupported compression algorithm!" << std::endl;
        return false;
    }

    // Read the error-correction algorithm code (2 bytes)
    uint16_t errorCorrectionCode;
    archiveFile.read(reinterpret_cast<char*>(&errorCorrectionCode), sizeof(errorCorrectionCode));
    if (errorCorrectionCode != EXPECTED_ERROR_CORRECTION_CODE) {
        std::cerr << "Error: Unsupported error-correction algorithm!" << std::endl;
        return false;
    }

    // Read number of entries
    uint64_t numEntries;
    archiveFile.read(reinterpret_cast<char*>(&numEntries), sizeof(numEntries));

    // Read each entry
    for (uint64_t i = 0; i < numEntries; ++i) {
        // Read relative path
        uint16_t pathLength;
        archiveFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));

        std::string relativePath(pathLength, '\0');
        archiveFile.read(&relativePath[0], pathLength);

        // Read directory/file flag
        uint8_t isDirectoryFlag;
        archiveFile.read(reinterpret_cast<char*>(&isDirectoryFlag), sizeof(isDirectoryFlag));

        // Create directory or extract file
        fs::path outputPath = fs::path(outputDirPath) / relativePath;
        if (isDirectoryFlag) {
            fs::create_directories(outputPath);
        } else {
            // Read file size
            uint64_t fileSize;
            archiveFile.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

            // Read file data
            std::vector<char> fileData(fileSize);
            archiveFile.read(fileData.data(), fileSize);

            // Write the file to disk
            std::ofstream outputFile(outputPath, std::ios::binary);
            outputFile.write(fileData.data(), fileSize);
            outputFile.close();
        }
    }

    archiveFile.close();
    std::cout << "Archive extracted successfully!" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_archive_file> <output_directory>" << std::endl;
        return 1;
    }

    std::string inputFilePath = argv[1];
    std::string outputDirPath = argv[2];

    if (!fs::exists(outputDirPath)) {
        fs::create_directories(outputDirPath);
    }

    if (!extractArchive(inputFilePath, outputDirPath)) {
        return 1;
    }

    return 0;
}
