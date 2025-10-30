#ifndef TRANSACTION_QUEUE_H
#define TRANSACTION_QUEUE_H

#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <algorithm>
#include "transaction.h"
#include "../utxo/UTXO_set.h"

class TransactionQueue {
private:
    std::vector<Transaction> pending_transactions;
    std::string file_path;

public:
    explicit TransactionQueue(const std::string& file_path_);

    void loadFromFile();
    void saveToFile() const;

    void addTransaction(const Transaction& tx);
    std::vector<Transaction> getRandomTransactions(int count) const;
    void removeTransactions(const std::vector<Transaction>& txs);
    void clear();

    size_t size() const;
};

#endif
