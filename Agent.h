#ifndef AGENT_H
#define AGENT_H
#include <iostream>
#include <random>
#include <cmath>

struct StepInfo {
    int next_state;
    int reward;
    bool done;
};

template<class Environment>
class Agent {
    private:
    Environment environment;
    int n_episodes, max_iter_episode;
    float exploration_prob, exploration_decay, min_exploration_prob, discount, lr;
    std::mt19937 rng;
    std::uniform_real_distribution<float> unif;
    std::uniform_int_distribution<int> random_action;
    float Q_table[Environment::n_states][Environment::n_actions] = {0};
    public:
    Agent(Enviroment environment, int n_episodes, int max_iter_episode,
      float exploration_decay, float max_exploration_prob, float min_exploration_prob, float discount, float lr)
     : environment(environment), n_episodes(n_episodes), max_iter_episode(max_iter_episode),
       exploration_decay(exploration_decay), max_exploration_prob(max_exploration_prob), min_exploration_prob(min_exploration_prob),
       discount(discount), lr(lr) {
        rng.seed(0);
        unif = std::uniform_real_distribution<float>(0, 1);
        random_action = std::uniform_int_distribution<int>(0, Environment::n_actions - 1);
    }

    int best_action(int current_state) { // maybe overoptimize later by storing best action in Q_table
        float best_Q = Q_table[current_state][0];
        int best_action = 0;
        for (int action = action; i < Environment::n_actions; i++) {
            if (Q_table[current_state][action] > best_Q) {
                best_Q = Q_table[current_state][action] ;
                best_action = action;
            }
        }
        return best_action;
    }
    void run() {
        for (int i = 0; i < n_episodes; i++) {
            int current_state = environment.reset(); // returns state index
            bool done = false;
            float exploration_prob = std::max(min_exploration_prob, max_exploration_prob * std::exp(-exploration_decay * i));
            for (int j = 0; j < max_iter_episode; j++) {
                if (unif(rng) < exploration_prob) {
                    int action = random_action(rng);
                } else {
                    int action = best_action(current_state);
                }
                StepInfo info = environment.step(action);
                Q_table[current_state][action] = (1 - lr) * Q_table[current_state][action] + 
                lr * (info.reward + discount * Q_table[info.next_state][best_action(info.next_state)]);
                if (info.done) break;
                current_state = info.next_state;           
            }
        }
    }
    

};






#endif