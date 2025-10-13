#include "transaction_generation.h"

void generate_transactions(const std::string& filename, int num_transactions){
    std::ofstream file(filename);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> user_dist(0, 999);
    std::uniform_real_distribution<double> balance_dist(0.1, 102.0);

    std::vector<std::string> public_keys;
    std::vector<double> balances;
    std::ifstream user_file("../data/users.txt");

    std::string first_name, last_name, public_key;
    double balance;
    while (user_file >> first_name >> last_name >> public_key >> balance) {
        public_keys.push_back(public_key);
        balances.push_back(balance);
    }
    user_file.close();

    for (int i = 0; i < num_transactions; i++) {
        int sender_index = user_dist(rng);
        std::string sender = public_keys[sender_index];
        std::string receiver = public_keys[user_dist(rng)];
        double amount = balances[sender_index]*balance_dist(rng);

        
    }
}