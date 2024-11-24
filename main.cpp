#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "algorithms/greedy.h"

using namespace std;

vector<int> getSetSystemParameters(const string& s) {
    vector<int> result;
    istringstream ss(s);
    int tmp;
    string token;
    ss >> token;
    if (token != "p") {
        cerr << "Invalid Format!" << endl;
    }
    ss >> token;
    if (token != "hs") {
        cerr << "Invalid Format!" << endl;
    }
    while(ss >> tmp) {
        result.push_back(tmp);
    }
    return result;
}

vector<int> getAllIntsFromString(const string& s) {
    vector<int> result;
    istringstream ss(s);
    int tmp;
    while(ss >> tmp) {
        result.push_back(tmp);
    }
    return result;
}

string toString(vector<vector<int>> setSystem) {
    stringstream ss;
    for (vector<int> line : setSystem) {
        ss << "[ ";
        for (int element : line) {
            ss << element << " ";
        }
        ss << "]" << endl;
    }
    return ss.str();
}

string toString(vector<int> vec) {
    stringstream ss;
    for (int elem : vec) {
        ss << elem << endl;
    }
    return ss.str();
}

int main(int argc, char *argv[]) {
    // Fehlererkennung
    if (argc != 2) {
        throw invalid_argument("Es muss genau der Pfad zum Set System übergeben werden.");
    }
    string inputFileName = argv[1];

    // Modell
    vector<vector<int>> setSystem;
    vector<int> setSystemParameters;

    // Eingabedatei parsen und Modell füllen
    ifstream inputFile (inputFileName);
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            if (line[0] == 'c') {

            }
            else if (line[0] == 'p') {
                setSystemParameters = getSetSystemParameters(line);
                if (setSystemParameters.size() >= 2) {
                    setSystem.reserve(setSystemParameters[1]);
                }
                else {
                    cout << "setSystemParameters konnten nicht ermittelt werden." << endl;
                }
            }
            else {
                setSystem.push_back(getAllIntsFromString(line));
            }
        }
        inputFile.close();
    }

    // Eingabe ausgeben
    cout << "Eingabe:" << endl;
    cout << "n: " << setSystemParameters[0] << " m: " << setSystemParameters[1] << endl;
    cout << toString(setSystem) << endl;

    // Berechnung
    vector<int> solution = calculateSolution(setSystemParameters[0], setSystemParameters[1], setSystem);
    cout << "Folgende Loesung wurde berechnet:" << endl;
    cout << toString(solution) << endl;

    // Lösung in eine Datei schreiben
    string outputFileName = inputFileName.replace(inputFileName.find(".hgr"), 4, ".sol");
    ofstream outputFile(outputFileName);
    if (outputFile.is_open()) {
        for (int num : solution) {
            outputFile << num << endl;
        }
        outputFile.close();
    }

    return 0;
}