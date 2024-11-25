#include <vector>
#include <unordered_set>
#include <unordered_map>

std::unordered_set<int> calculateSolution(int, int, std::vector<std::vector<int>>);
std::unordered_map<int, int> updateCount(int, int, const std::vector<std::vector<int>>&);
int findDominantVertex(const std::unordered_map<int, int>&);
void removeEdgesContainingVertex(int, std::vector<std::vector<int>>&);
bool exitCondition(const std::vector<std::vector<int>>&);