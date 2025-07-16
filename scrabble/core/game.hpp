#pragma once
#include "core/dictionary.hpp"
#include "player.hpp"
#include "bag.hpp"
#include "board.hpp"


namespace core {
    class Game {
        public:
        Game(int num_players, Dictionary::DictionaryType dict_type, const std::vector<std::string>& custom_names = {});
        bool IsGameOver() const;
        void NextTurn();
        bool ExecuteMove(const std::string& move_type, const std::string& word, char dir, int row, int col);
        Player GetWinner() const;
        const Player& GetCurrentPlayer() const;
        int GetBagSize() const;
        void EndGame();

        private:
        std::vector<Player> players_;
        Bag bag_;
        Board board_;
        Dictionary dictionary_;
        int current_player_index_;
        int consecutive_passes_;
        bool game_over_;
    };
}