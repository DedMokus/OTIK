#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

// New format signature
const char FILE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t VERSION = 1; // Format version
const uint16_t COMPRESSION_CODE = 0; // 0 = no compression
const uint16_t ERROR_CORRECTION_CODE = 0; // 0 = no error correction

bool encodeFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    // Open the input file in binary mode
    std::ifstream inputFile(inputFilePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open input file: " << inputFilePath << std::endl;
        return false;
    }

    // Get the size of the input file
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Read the input file content
    std::vector<char> fileData(fileSize);
    if (!inputFile.read(fileData.data(), fileSize)) {
        std::cerr << "Error reading input file: " << inputFilePath << std::endl;
        return false;
    }
    inputFile.close();

    // Open the output file in binary mode
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFilePath << std::endl;
        return false;
    }

    // Write the signature (6 bytes)
    outputFile.write(FILE_SIGNATURE, 6);

    // Write the version (2 bytes)
    outputFile.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

    // Write the compression algorithm code (2 bytes)
    outputFile.write(reinterpret_cast<const char*>(&COMPRESSION_CODE), sizeof(COMPRESSION_CODE));

    // Write the error-correction algorithm code (2 bytes)
    outputFile.write(reinterpret_cast<const char*>(&ERROR_CORRECTION_CODE), sizeof(ERROR_CORRECTION_CODE));

    // Write the original file size (8 bytes)
    uint64_t originalFileSize = static_cast<uint64_t>(fileSize);
    outputFile.write(reinterpret_cast<const char*>(&originalFileSize), sizeof(originalFileSize));

    // Here, we could write additional service data if needed (optional)

    // Write the raw file data (no compression applied for now)
    outputFile.write(fileData.data(), fileSize);

    // Close the output file
    outputFile.close();

    std::cout << "File encoded successfully!" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    const std::string inputFilePath = argv[1];
    const std::string outputFilePath = argv[2];

    if (!encodeFile(inputFilePath, outputFilePath)) {
        return 1;
    }

    return 0;
}
