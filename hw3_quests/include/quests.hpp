/**
 * @file quests.hpp
 * @author Wuqiong Zhao (wqzhao@ucsd.edu)
 * @brief Quests and Greedy Solver
 * @version 0.1
 * @date 2025-02-11
 *
 * @copyright Copyright (c) 2025 Wuqiong Zhao
 *
 */

#ifndef __QUESTS_HPP__
#define __QUESTS_HPP__

#include <algorithm>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace hw3 {

template <typename T_XP = int, typename T_Gold = int,
          typename = std::enable_if_t<std::is_arithmetic_v<T_XP> && std::is_arithmetic_v<T_Gold>>>
struct Quest {
    T_XP min_xp;
    T_Gold gold_reward;
    T_XP xp_reward;
    bool completed = false;

    Quest(T_XP min, T_Gold gold, T_XP xp) : min_xp(min), gold_reward(gold), xp_reward(xp) {}

    bool operator<(const Quest& other) const {
        return gold_reward > other.gold_reward; // Sort by descending gold reward
    }

    std::string info() const {
        return std::format("min XP: {} | gold reward: {}, XP reward {}", min_xp, gold_reward, xp_reward);
    }
};

template <typename T_XP = int, typename T_Gold = int,
          typename = std::enable_if_t<std::is_arithmetic_v<T_XP> && std::is_arithmetic_v<T_Gold>>>
using Quests = std::vector<Quest<T_XP, T_Gold>>;

template <typename T_XP = int, typename T_Gold = int,
          typename = std::enable_if_t<std::is_arithmetic_v<T_XP> && std::is_arithmetic_v<T_Gold>>>
class QuestSolver {
    using T_Quest = Quest<T_XP, T_Gold>;

  private:
    Quests<T_XP, T_Gold> quests;
    T_XP current_xp     = 0;
    T_Gold current_gold = 0;

  public:
    QuestSolver() = default;

    void insert_quest(T_XP min_xp, T_Gold gold_reward, T_XP xp_reward) {
        auto pos = std::lower_bound(quests.begin(), quests.end(), T_Quest(min_xp, gold_reward, xp_reward));
        quests.insert(pos, T_Quest(min_xp, gold_reward, xp_reward));
    }

    std::vector<int> solve_greedy(T_Gold target_gold) {
        std::vector<int> quest_sequence;

        while (current_gold < target_gold) {
            int best_quest_idx = -1;

            for (int i = 0; i < quests.size(); i++) {
                if (!quests[i].completed && quests[i].min_xp <= current_xp) {
                    best_quest_idx = i;
                    break; // First available quest is best due to sorting.
                }
            }

            if (best_quest_idx == -1) break;

            quests[best_quest_idx].completed = true;
            current_xp += quests[best_quest_idx].xp_reward;
            current_gold += quests[best_quest_idx].gold_reward;
            quest_sequence.push_back(best_quest_idx);
        }

        return quest_sequence;
    }

    const T_Quest& operator[](size_t idx) const { return quests[idx]; }
};

} // namespace hw3

#endif
