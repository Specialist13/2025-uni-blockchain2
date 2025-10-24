#include "generate_transactions.h"

#include <iostream>

void generate_transactions(const std::string& filename, int num_transactions){
    std::ofstream fout(filename);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> user_dist(0, 999);
    std::uniform_real_distribution<double> balance_dist(0.01, 1.02);

    std::vector<std::string> public_keys;
    std::vector<double> balances;
    std::ifstream user_file("./data/users.txt");

    std::string first_name, last_name, public_key, private_key;
    double balance;
    while (user_file >> first_name >> last_name >> public_key >> private_key >> balance) {
        public_keys.push_back(public_key);
    }
    user_file.close();

    for (int i = 0; i < num_transactions; i++) {
        int sender_index = user_dist(rng);

        
    }
    fout.close();
}