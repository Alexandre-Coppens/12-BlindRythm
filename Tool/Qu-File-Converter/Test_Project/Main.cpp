#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

void addSemicolonsToFile(const std::string& filename, std::string name) {
    bool start = false;
    int lineNbr = 0;
    int num = 0;
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error: Cannot open file for reading: " << filename << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    std::string line2;
    std::string tmpLine;

    lines.push_back("Row Name,Timer,Line");

    // Read and modify lines
    while (std::getline(inFile, line)) {
        // Trim any trailing whitespace
        while (!line.empty() && std::isspace(line.back())) {
            line.pop_back();
        }
        if (start) {
            if (!line.empty() && line.back() != ';') {
                tmpLine = "";
                for (char x : line) {
                    if (isdigit(x) || x == '.') {
                        tmpLine += x;
                    }
                }
                if (line.find("StartTime:") != std::string::npos) {
                    tmpLine += ',';
                    line2 += tmpLine;
                }
                if (line.find("Lane:") != std::string::npos) {
                    lines.push_back(std::to_string(num) + "," + line2 + tmpLine);
                    num++;
                    line2 = "";
                }
                lineNbr++;
            }
        }
        else {
            if (line == "HitObjects:") {
                start = true;
            }
        }
    }

    inFile.close();

    // Write modified lines back to file
    std::ofstream outFile("Outputs/" + name + ".csv");
    if (!outFile) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& modifiedLine : lines) {
        outFile << modifiedLine << '\n';
    }

    outFile.close();
    std::cout << "New map created at Outputs/[MapName].csv: " << filename << std::endl;
}

int main() {
    std::string path = "Inputs";

    const std::filesystem::path resourcePath{ path };

    for (const auto& entry : std::filesystem::directory_iterator(resourcePath)) {
        if (!entry.is_regular_file()) {
            std::cout << "??    " << entry.path().filename().string() << '\n';
            continue;
        }

        const auto filenameStr = entry.path().filename().string();
        const auto fileStemStr = entry.path().stem().string();
        const auto fileExtensionStr = entry.path().extension().string();

        std::cout << "      -file: " << filenameStr << '\n';
        if (fileExtensionStr == ".qua") {
            addSemicolonsToFile(path + "/" + filenameStr, fileStemStr);
        }
        else {
            std::cout << "QUALOADER::FILEEXTENSION::NOTQUA.          " << '\n';
        }
    }
    return 0;
}