#include "generate_transactions.h"
#include <iostream>

void generate_transactions(const std::string& filename, int num_transactions){
    std::ofstream fout(filename);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> user_dist(0, 999);
    std::uniform_real_distribution<double> balance_dist(0.01, 1.02);

    std::vector<std::string> public_keys, private_keys;
    std::vector<double> balances;
    std::ifstream user_file("./data/users.txt");

    std::string first_name, last_name, public_key, private_key;
    double balance;
    while (user_file >> first_name >> last_name >> public_key >> private_key >> balance) {
        public_keys.push_back(public_key);
        private_keys.push_back(private_key);
    }
    user_file.close();

    for (int i = 0; i < num_transactions; i++) {
        int sender_index = user_dist(rng);
        int receiver_index = user_dist(rng);
        while (receiver_index == sender_index) {
            receiver_index = user_dist(rng);
        }
        double amount = UTXOSet::getInstance()->getBalanceForAddress(public_keys[sender_index]) * balance_dist(rng);
        std::vector<UTXO> sender_utxos = UTXOSet::getInstance()->getAllUTXOsForAddress(public_keys[sender_index]);
        sort(sender_utxos.begin(), sender_utxos.end(), [](const UTXO& a, const UTXO& b) {
            return a.getAmount() > b.getAmount();
        });
        for (auto utxo : sender_utxos) {
            if (amount <= 0) break;
            double utxo_amount = utxo.getAmount();
            double used_amount = std::min(utxo_amount, amount);
            fout << utxo.getTransactionID() << " " << utxo.getOutputIndex() << " " << private_keys[sender_index] << " ";
            amount -= used_amount;
        }
    }
    fout.close();
}