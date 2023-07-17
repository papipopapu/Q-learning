#ifndef AGENT_H
#define AGENT_H
#include <iostream>
#include <fstream>
#include <cstring>
#include <random>
#include <cmath>
#include "Types.h"

template<class Environment>
class Agent {
    private:
    Environment environment;
    int n_episodes, max_iter_episode;
    float max_exploration_prob, min_exploration_prob, exploration_decay;
    float discount, lr;
    std::mt19937 rng;
    std::uniform_real_distribution<float> unif;
    float Q_table[Environment::n_states][Environment::n_actions] = {0};
    public:
    Agent(int n_episodes, int max_iter_episode,
      float exploration_decay, float max_exploration_prob, float min_exploration_prob, float discount, float lr) {
        
        this->environment = Environment();
        this->n_episodes = n_episodes;
        this->max_iter_episode = max_iter_episode;
        this->exploration_decay = exploration_decay;
        this->max_exploration_prob = max_exploration_prob;
        this->min_exploration_prob = min_exploration_prob;
        this->discount = discount;
        this->lr = lr;

        rng.seed(0);
        unif = std::uniform_real_distribution<float>(0, 1);
    }

    int best_action() { // maybe overoptimize later by storing best action in Q_table
        float best_Q = -10000000;
        int best_action = 0;
        for (int action : environment.get_legal_actions()) {
            if (Q_table[environment.get_state()][action] > best_Q) {
                best_Q = Q_table[environment.get_state()][action];
                best_action = action;
            }
        }
        return best_action;
    }
    int best_action(Environment env) { // maybe overoptimize later by storing best action in Q_table
        float best_Q = -10000000;
        int best_action = 0;
        for (int action : env.get_legal_actions()) {
            if (Q_table[env.get_state()][action] > best_Q) {
                best_Q = Q_table[env.get_state()][action];
                best_action = action;
            }
        }
        return best_action;
    }
    void run() {
        int player_state, opponent_state, opponent_action, player_action, best_player_action, reward;
        float exploration_prob;
        bool done;
        StepInfo info;

        for (int i = 0; i < n_episodes; i++) {
            player_state = environment.reset(); // returns state index
            done = false;
            exploration_prob = std::max(min_exploration_prob, max_exploration_prob * std::exp(-exploration_decay * i));
            
            // lets make the first move without any updates
            if (unif(rng) < exploration_prob) {
                player_action = environment.sample_action();
            } else {
                player_action = this->best_action();
            }
            info = environment.step(player_action);
            opponent_state = info.next_state;
            // obviously you cannot win on your first move, but just in case
            if (info.done) {
                Q_table[player_state][player_action] = Q_table[player_state][player_action] * (1 - lr) + lr * info.reward;
                continue;
            } 

            // now lets do the rest of the moves
            for (int j = 1; j < max_iter_episode; j++) {
                
                // opponent moves
                if (unif(rng) < exploration_prob) {
                    opponent_action = environment.sample_action();
                } else {
                    opponent_action = this->best_action();
                }
                info = environment.step(opponent_action);
                // lets assume the game is deterministic (i.e., if you win on your move, Q(s, a) is set to the reward)
                // if this move ends the game, then Q(s, a) = r, we update the Q table with last state and opponents move
                if (info.done) {
                    Q_table[opponent_state][opponent_action] = Q_table[opponent_state][opponent_action] * (1 - lr) + lr * info.reward;
                    // also update the Q table with the previous state and action,
                    // since there exists a response to the player move that ends the game
                    // lets try doing a normal update for now (i.e., not setting Q(s, a) = r)
                    Q_table[player_state][player_action] = Q_table[player_state][player_action] * (1 - lr) - lr * info.reward;
                    break;
                }
                // otherwise, lets update Q for the previous state and action
                best_player_action = this->best_action();
                Q_table[player_state][player_action] = Q_table[player_state][player_action] * (1 - lr) + lr * (info.reward + discount * Q_table[info.next_state][best_player_action]);

                // now we make the previous state and action the opponents state and action
                player_state = opponent_state;
                player_action = opponent_action;
            }
        }
    }
    void save_Q_table(std::string filename) {
        std::ofstream file;
        file.open(filename);
        for (int i = 0; i < Environment::n_states; i++) {
            for (int j = 0; j < Environment::n_actions; j++) {
                file << Q_table[i][j] << " ";
            }
            file << "\n";
        }
        file.close();
    }
    void load_Q_table(std::string filename) {
        std::ifstream file;
        file.open(filename);
        for (int i = 0; i < Environment::n_states; i++) {
            for (int j = 0; j < Environment::n_actions; j++) {
                file >> Q_table[i][j];
            }
        }
        file.close();
    }
    

};






#endif