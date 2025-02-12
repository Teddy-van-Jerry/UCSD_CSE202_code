#include "quests.hpp"
using namespace hw3;

int main() {
    QuestSolver solver; // template with default types as <int, int>

    // quests setup
    solver.insert_quest(0, 3, 1);
    solver.insert_quest(0, 2, 5);
    solver.insert_quest(4, 8, 1);

    auto solution = solver.solve_greedy(10);

    std::cout << "Quest sequence:\n";
    for (auto quest_idx : solution) {
        std::cout << std::format("<{}> {}", quest_idx, solver[quest_idx].info()) << std::endl;
    }

    return 0;
}
