#ifndef CONNECTTHREE_H
#define CONNECTTHREE_H
#include "Types.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

class ConnectThree {
    private:
    std::vector<int> empty;  
    int state_idx = 0;
    int player_turn = 1;
    int state[9] = {0};
    public:
    static const int n_states = 19683;
    static const int n_actions = 9;
    ConnectThree() {
        empty.reserve(9);
        for (int i=0; i<9; i++) empty.push_back(i);
    }
    bool has_winner() {
        // check rows
        int last = 3 - player_turn;
        for (int i=0; i<3; i++) {
            if (state[i*3] == last && state[i*3 + 1] == last && state[i*3 + 2] == last) {
                return true;
            }
        }
        // check columns
        for (int i=0; i<3; i++) {
            if (state[i] == last && state[i + 3] == last && state[i + 6] == last) {
                return true;
            }
        }
        // check diagonals
        if (state[0] == last && state[4] == last && state[8] == last) {
            return true;
        }
        if (state[2] == last && state[4] == last && state[6] == last) {
            return true;
        }

        return false;
    }
    StepInfo step(int action) { // action := square to put the piece
        StepInfo info;

        state[action] = player_turn;
        empty.erase(std::find(empty.begin(), empty.end(), action));
        state_idx += std::pow(3, action) * player_turn ;
        player_turn = 3 - player_turn;

        info.next_state = state_idx;
        if (has_winner()) {
            info.reward = 1;
            info.done = true;
        } else if (empty.size() == 0) {
            info.reward = 0;
            info.done = true;
        } else {
            info.reward = 0;
            info.done = false;
        }
        return info; // always from the point of view of the one to play
    }





    int get_state() {
        return state_idx;
    }
    std::vector<int> get_legal_actions() {
        return empty;
    }
    int sample_action() {
        // since size is max 9, slkdfjñlkdsgjñlsdkg
        return empty[rand() % empty.size()];
    }
    void print() {
        std::cout << " " << state[0] << " | " << state[1] << " | " << state[2] << std::endl;
        std::cout << "-----------" << std::endl;
        std::cout << " " << state[3] << " | " << state[4] << " | " << state[5] << std::endl;
        std::cout << "-----------" << std::endl;
        std::cout << " " << state[6] << " | " << state[7] << " | " << state[8] << std::endl;
    }
    int reset() {
        empty.clear();
        for (int i=0; i<9; i++) {
            state[i] = 0;
            empty.push_back(i);
        }
        state_idx = 0;
        player_turn = 1;  
        return 0;
    }

                                       
};


#endif