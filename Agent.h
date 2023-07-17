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
    float Q_table[Environment::n_states][Environment::n_actions];
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

        // initialize Q_table to 0.0001 to check if it's been updated
        for (int i = 0; i < Environment::n_states; i++) {
            for (int j = 0; j < Environment::n_actions; j++) {
                Q_table[i][j] = 0.0001;
            }
        }

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
        int state_0, state_1, state_2, action_0, action_1, action_2;
        float reward_0, reward_1;
        float exploration_prob;
        bool done;
        StepInfo info;

        for (int i = 0; i < n_episodes; i++) {
            environment.reset(); // returns state index
            exploration_prob = std::max(min_exploration_prob, max_exploration_prob * std::exp(-exploration_decay * i));
            
            // 50% chance of random oponent going first
            if (unif(rng) < 0.5) {
                action_0 = environment.sample_action();
                environment.step(action_0);
                // impossible to win or lose on first move so whatever
            }

            for (int j = 1; j < max_iter_episode; j++) {
                // player move
                state_0 = environment.get_state();
                if (unif(rng) < exploration_prob) {
                    action_0 = environment.sample_action();
                } else {
                    action_0 = best_action();
                }
                info = environment.step(action_0);
                done = info.done;
                state_1 = info.next_state;
                reward_0 = info.reward;
                if (done) {
                    Q_table[state_0][action_0] = reward_0;
                    break;
                }
                // opponent move
                action_1 = environment.sample_action();
                info = environment.step(action_1);
                reward_1 = info.reward;
                done = info.done;
                if (done) {
                    // update opponent action
                    Q_table[state_1][action_1] = reward_1;
                    // update player action
                    if (Q_table[state_0][action_0] == 0.0001) {
                        Q_table[state_0][action_0] = -reward_1;
                    } else {
                        Q_table[state_0][action_0] = std::min(Q_table[state_0][action_0], -reward_1);
                    }
                    break;
                }
                // get our best reponse
                state_2 = info.next_state;
                action_2 = best_action();
                // update player action
                Q_table[state_0][action_0] = (1 - lr) * Q_table[state_0][action_0] + lr * (reward_1 + discount * Q_table[state_2][action_2]);

                
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
    void play_against_random(int N, float &winrate, float &loserate) {
        winrate = 0;
        loserate = 0;
        for (int i = 0; i < N; i++) {
            environment.reset();
            // 50% chance of random oponent going first
            if (unif(rng) < 0.5) {
                int action = environment.sample_action();
                environment.step(action);
            }


            while (true) {
                // agent plays
                int action = best_action();
                StepInfo info = environment.step(action);
                if (info.done) {
                    if (info.reward == 1) {
                        winrate += 1;
                    }
                    break;
                }
                // opponent plays
                action = environment.sample_action();
                info = environment.step(action);
                if (info.done) {
                    if (info.reward == 1) {
                        loserate += 1;
                    } 
                    break;
                }
            }

        }
        winrate /= N;
        loserate /= N;
    }

};






#endif