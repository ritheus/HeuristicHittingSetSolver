#include <vector>
#include <iostream>
#include <sstream>
#include "greedy.h"

using namespace std;

vector<int> calculateSolution(int n, int m, vector<vector<int>> setSystem) {
    cout << "(using greedy algorithm...)" << endl;

    vector<int> count(n);
    vector<int> solutionSet;
    int dominantVertex;

    bool solution_found = false;

    while (!exitCondition(setSystem)) {
        count = updateCount(n, m, setSystem);
        dominantVertex = findDominantVertex(count);
        solutionSet.push_back(dominantVertex);
        setSystem = removeEdgesContainingVertex(dominantVertex, setSystem);
        toStringGreedy(setSystem);
    }

    // Insert solution size
    solutionSet.insert(solutionSet.begin(), solutionSet.size());

    return solutionSet;
}

vector<int> updateCount(int n, int m, vector<vector<int>> setSystem) {
    vector<int> count(n);

    for (int i = 0; i<m; i++) {
        for (int j = 0; j<setSystem[i].size(); j++) {
            if (!setSystem[i].empty()) {
                count[setSystem[i][j]-1]++;
            }
        }
    }

    return count;
}

int findDominantVertex(vector<int> count) {
    auto it = max_element(count.begin(), count.end());
    int index = distance(count.begin(), it);
    return index + 1;
}

vector<vector<int>> removeEdgesContainingVertex(int vertex, vector<vector<int>> setSystem) {
    vector<int> toRemove;

    for (int i = 0; i<setSystem.size(); i++) {
        bool cond = find(setSystem[i].begin(), setSystem[i].end(), vertex) != setSystem[i].end();
        if (cond) {
            toRemove.push_back(i);
        }
    }

    for (int i = 0; i<toRemove.size(); i++) {
        //setSystem.erase(setSystem.begin() + i); führt dazu, dass updateCount() so nicht mehr funktioniert
        setSystem[toRemove[i]] = vector<int>(0);
    }

    return setSystem;
}

bool exitCondition(vector<vector<int>> setSystem) {
    for (int i = 0; i<setSystem.size(); i++) {
        //cout << "i: " << i << " " << setSystem[i].empty() << endl;
        if (!setSystem[i].empty()) {
            return false;
        }
    }
    return true;
}

string toStringGreedy(vector<vector<int>> setSystem) {
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