#include <vector>
#include <set>

std::set<int> calculateSolution(int, int, std::vector<std::vector<int>>);
std::vector<int> updateCount(int, int, const std::vector<std::vector<int>>&);
int findDominantVertex(const std::vector<int>&);
void removeEdgesContainingVertex(int, std::vector<std::vector<int>>&);
bool exitCondition(const std::vector<std::vector<int>>&);