#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "greedy.h"

using namespace std;

unordered_set<int> calculateSolution(int n, int m, vector<vector<int>> setSystem) {
    cout << "(using greedy algorithm...)" << endl;

    vector<int> count(n);
    unordered_set<int> solutionSet;
    int dominantVertex;

    while (!exitCondition(setSystem)) { // Das sollte irgendwie einfacher gehen, schau ich noch
        count = updateCount(n, m, setSystem);
        dominantVertex = findDominantVertex(count);
        solutionSet.insert(dominantVertex);
        removeEdgesContainingVertex(dominantVertex, setSystem);
    }

    return solutionSet;
}

vector<int> updateCount(int n, int m, const vector<vector<int>>& setSystem) {
    vector<int> count(n);

    for (int i = 0; i<m; i++) {
        for (int j = 0; j<setSystem[i].size(); j++) {
            count[setSystem[i][j]-1]++;
        }
    }

    return count;
}

int findDominantVertex(const vector<int>& count) {
    auto it = max_element(count.begin(), count.end());
    int index = distance(count.begin(), it);
    return index + 1;
}

void removeEdgesContainingVertex(int vertex, vector<vector<int>>& setSystem) {
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
}

bool exitCondition(const vector<vector<int>>& setSystem) {
    for (int i = 0; i<setSystem.size(); i++) {
        if (!setSystem[i].empty()) {
            return false;
        }
    }
    return true;
}