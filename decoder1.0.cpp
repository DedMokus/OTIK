#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

const char FILE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t EXPECTED_VERSION = 1;
const uint16_t EXPECTED_COMPRESSION_CODE = 0;
const uint16_t EXPECTED_ERROR_CORRECTION_CODE = 0;

bool decodeFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    // Open the input archive file in binary mode
    std::ifstream inputFile(inputFilePath, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open input file: " << inputFilePath << std::endl;
        return false;
    }

    // Get the size of the input file
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Read the file signature (6 bytes)
    char fileSignature[6];
    inputFile.read(fileSignature, 6);
    if (std::memcmp(fileSignature, FILE_SIGNATURE, 6) != 0) {
        std::cerr << "Error: Invalid file signature!" << std::endl;
        return false;
    }

    // Read the version (2 bytes)
    uint16_t version;
    inputFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != EXPECTED_VERSION) {
        std::cerr << "Error: Unsupported file version!" << std::endl;
        return false;
    }

    // Read the compression algorithm code (2 bytes)
    uint16_t compressionCode;
    inputFile.read(reinterpret_cast<char*>(&compressionCode), sizeof(compressionCode));
    if (compressionCode != EXPECTED_COMPRESSION_CODE) {
        std::cerr << "Error: Unsupported compression algorithm!" << std::endl;
        return false;
    }

    // Read the error-correction algorithm code (2 bytes)
    uint16_t errorCorrectionCode;
    inputFile.read(reinterpret_cast<char*>(&errorCorrectionCode), sizeof(errorCorrectionCode));
    if (errorCorrectionCode != EXPECTED_ERROR_CORRECTION_CODE) {
        std::cerr << "Error: Unsupported error-correction algorithm!" << std::endl;
        return false;
    }

    // Read the original file size (8 bytes)
    uint64_t originalFileSize;
    inputFile.read(reinterpret_cast<char*>(&originalFileSize), sizeof(originalFileSize));

    // Ensure there's enough data in the file
    std::streamsize dataSectionSize = fileSize - inputFile.tellg();
    if (dataSectionSize < static_cast<std::streamsize>(originalFileSize)) {
        std::cerr << "Error: Archive data is corrupted!" << std::endl;
        return false;
    }

    // Read the raw file data
    std::vector<char> fileData(originalFileSize);
    if (!inputFile.read(fileData.data(), originalFileSize)) {
        std::cerr << "Error: Failed to read the original file data!" << std::endl;
        return false;
    }
    inputFile.close();

    // Write the extracted file data to the output file
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFilePath << std::endl;
        return false;
    }
    outputFile.write(fileData.data(), originalFileSize);
    outputFile.close();

    std::cout << "File decoded successfully!" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_archive_file> <output_file>" << std::endl;
        return 1;
    }

    const std::string inputFilePath = argv[1];
    const std::string outputFilePath = argv[2];

    if (!decodeFile(inputFilePath, outputFilePath)) {
        return 1;
    }

    return 0;
}
