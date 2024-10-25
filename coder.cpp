#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cerrno>

const char FILE_SIGNATURE[6] = {'A', 'B', 'O', 'B', 'A', '1'};
const uint16_t VERSION = 0;

bool encodeFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::fstream inputFile(inputFilePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << inputFilePath << std::endl;
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
        return false;
    }
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::vector<char> fileData(fileSize);
    if (!inputFile.read(fileData.data(), fileSize)) {
        std::cerr << "Error reading input file: " << inputFilePath << std::endl;
        return false;
    }
    inputFile.close();


    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFilePath << std::endl;
        return false;
    }

    outputFile.write(FILE_SIGNATURE, 6);

    outputFile.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

    uint64_t originalFileSize = static_cast<uint64_t>(fileSize);

    outputFile.write(reinterpret_cast<const char*>(&originalFileSize), sizeof(originalFileSize));

    outputFile.write(fileData.data(), fileSize);

    outputFile.close();

    std::cout << "File encoded successfully" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc !=3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>" << std::endl;
        return 1;
    }

    const std::string inputFilePath = argv[1];
    const std::string outputFilePath = argv[2];

    if (!encodeFile(inputFilePath, outputFilePath)) {
        return 1;
    }
    return 0;
}