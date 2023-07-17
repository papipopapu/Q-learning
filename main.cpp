#include "Agent.h"
#include "ConnectThree.h"

int get_state_idx(int state[9], int player_turn) {
    int state_idx = 0;
    for (int i=0; i<9; i++) {
        state_idx += std::pow(3, i) * state[i]; 
    }
    return state_idx;
}


int main() {
    // get the state_idx of all winning states
    ConnectThree game;
    int N = 100000;
    Agent<ConnectThree> agent(N, 10, 1./N, 1, 0.05, 1.0, 0.1);
    agent.run();
    agent.save_Q_table("Q_table.txt");    
    /* agent.load_Q_table("Q_table.txt"); */

    float winrate, loserate;
    agent.play_against_random(10000, winrate, loserate);
    std::cout << "Winrate against random: " << winrate << std::endl;
    std::cout << "Loserate against random: " << loserate << std::endl;
    
    return 0;
}