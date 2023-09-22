#include "fstream"

int main() {
    auto inFile = std::ifstream("Path.txt");
    auto outFile = std::ofstream ("PathSingleLine.txt");
    auto ch = '\0';
    while (inFile.peek() != EOF) {
        inFile.get(ch);
        if (ch != '\n') {
            outFile << ch;
        }
    }
    inFile.close();
    outFile.close();
}