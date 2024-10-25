#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint> // For fixed-width integer types like uint16_t, uint64_t
#include <cstring> // For memcmp

// Define the same 6-byte signature for the format
const char FILE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t EXPECTED_VERSION = 0; // Expecting version 0 for this task

bool decodeFile(const std::string& encodedFilePath, const std::string& outputFilePath) {
    // Open the encoded file in binary mode
    std::ifstream inputFile(encodedFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open encoded file: " << encodedFilePath << std::endl;
        return false;
    }

    // Read the 6-byte signature
    char fileSignature[6];
    inputFile.read(fileSignature, 6);
    if (std::memcmp(fileSignature, FILE_SIGNATURE, 6) != 0) {
        std::cerr << "Error: File signature does not match!" << std::endl;
        return false;
    }

    // Read the 2-byte version
    uint16_t version;
    inputFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != EXPECTED_VERSION) {
        std::cerr << "Error: Unsupported file version: " << version << std::endl;
        return false;
    }

    // Read the original file size (8 bytes)
    uint64_t originalFileSize;
    inputFile.read(reinterpret_cast<char*>(&originalFileSize), sizeof(originalFileSize));

    // Read the raw data
    std::vector<char> fileData(originalFileSize);
    inputFile.read(fileData.data(), originalFileSize);
    inputFile.close();

    // Open the output file in binary mode
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFilePath << std::endl;
        return false;
    }

    // Write the raw file data
    outputFile.write(fileData.data(), originalFileSize);
    outputFile.close();

    std::cout << "File decoded successfully!" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <encoded_file> <output_file>" << std::endl;
        return 1;
    }

    const std::string encodedFilePath = argv[1];
    const std::string outputFilePath = argv[2];

    if (!decodeFile(encodedFilePath, outputFilePath)) {
        return 1;
    }

    return 0;
}
