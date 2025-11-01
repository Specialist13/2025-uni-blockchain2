#include "generate_transactions.h"
#include <iostream>

void generate_transactions(const std::string& filename, int num_transactions){
    json existing = json::array();
    {
        std::ifstream fin(filename);
        if (fin) {
            try { fin >> existing; } catch (...) { existing = json::array(); }
        }
    }
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> user_dist(0, 999);
    std::uniform_real_distribution<double> balance_dist(0.01, 1.02);

    std::vector<std::string> public_keys, private_keys;
    std::ifstream user_file("./data/users.txt");

    std::string first_name, last_name, public_key, private_key;
    double balance;
    while (user_file >> first_name >> last_name >> public_key >> private_key >> balance) {
        public_keys.push_back(public_key);
        private_keys.push_back(private_key);
    }
    user_file.close();

    json j = existing;
    for (int i = 0; i < num_transactions; i++) {
        int sender_index = user_dist(rng);
        while (UTXOSet::getInstance()->getAvailableBalanceForAddress(public_keys[sender_index]) == 0.0 && 
               UTXOSet::getInstance()->getAvailableMempoolBalanceForAddress(public_keys[sender_index]) == 0.0) {
            sender_index = user_dist(rng);
        }
        int receiver_index = user_dist(rng);
        while (receiver_index == sender_index) {
            receiver_index = user_dist(rng);
        }
        double amount = (UTXOSet::getInstance()->getAvailableBalanceForAddress(public_keys[sender_index])+
                        UTXOSet::getInstance()->getAvailableMempoolBalanceForAddress(public_keys[sender_index]))
                        * pow(balance_dist(rng), 4);

        std::vector<UTXO> sender_utxos = UTXOSet::getInstance()->getAvailableUTXOsForAddress(public_keys[sender_index]);
        std::vector<UTXO> sender_mempool_utxos = UTXOSet::getInstance()->getAvailableMempoolUTXOsForAddress(public_keys[sender_index]);
        sort(sender_utxos.begin(), sender_utxos.end(), [](const UTXO& a, const UTXO& b) {
            return a.getAmount() > b.getAmount();
        });
        sort(sender_mempool_utxos.begin(), sender_mempool_utxos.end(), [](const UTXO& a, const UTXO& b) {
            return a.getAmount() > b.getAmount();
        });
        double total_used = 0.0;
        std::vector<TransactionInputs> inputs;
        if (!sender_utxos.empty()){
            for (auto utxo : sender_utxos) {
                if (amount <= total_used) break;
                double utxo_amount = utxo.getAmount();
                std::string signature_to_hash = utxo.getTransactionID() + std::to_string(utxo.getOutputIndex()) + private_keys[sender_index];
                std::string signature = SlaSimHash(signature_to_hash); 
                TransactionInputs input(utxo.getTransactionID(), utxo.getOutputIndex(), signature);
                inputs.push_back(input);
                total_used += utxo_amount;
                UTXOSet::getInstance()->reserveUTXO(utxo);
            }
        }
        if (!sender_mempool_utxos.empty() && amount > total_used){
            for (auto utxo : sender_mempool_utxos) {
                if (amount <= total_used) break;
                double utxo_amount = utxo.getAmount();
                std::string signature_to_hash = utxo.getTransactionID() + std::to_string(utxo.getOutputIndex()) + private_keys[sender_index];
                std::string signature = SlaSimHash(signature_to_hash); 
                TransactionInputs input(utxo.getTransactionID(), utxo.getOutputIndex(), signature);
                inputs.push_back(input);
                total_used += utxo_amount;
                UTXOSet::getInstance()->reserveMempoolUTXO(utxo);
            }
        }
        TransactionOutputs output1(public_keys[receiver_index], amount);
        TransactionOutputs output2(public_keys[sender_index], std::max(total_used - amount, 0.0));
        std::string to_hash;
        for (const auto& input : inputs) {
            to_hash += input.toString();
        }

        to_hash += output1.toString() + output2.toString();
        std::string transaction_id = SlaSimHash(to_hash);
        Transaction transaction(transaction_id, inputs, { output1, output2 });
        UTXO mempoolUTXO1(transaction_id, 0, output1.getReceiverPublicKey(), output1.getAmount());
        UTXO mempoolUTXO2(transaction_id, 1, output2.getReceiverPublicKey(), output2.getAmount());
        // avoid duplicating the same transaction in the existing queue/file
        bool already_exists = false;
        for (const auto &item : j) {
            if (item.contains("transaction_id") && item["transaction_id"] == transaction_id) {
                already_exists = true;
                break;
            }
        }
        if (!already_exists) {
            // add mempool UTXOs (will be ignored if already present in the set)
            bool added1 = UTXOSet::getInstance()->addMempoolUTXO(mempoolUTXO1);
            bool added2 = UTXOSet::getInstance()->addMempoolUTXO(mempoolUTXO2);
            if (!added1 || !added2) {
                std::cerr << "Warning: attempted to add duplicate mempool UTXO for tx " << transaction_id << std::endl;
            }
            j.push_back(transaction.toJson());
        } else {
            std::cerr << "Skipping duplicate transaction " << transaction_id << " already in queue." << std::endl;
        }
    }
    std::ofstream fout(filename);
    fout << j.dump(4);
    fout.close();
}
