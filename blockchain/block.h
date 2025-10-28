#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "transaction.h"
#include "hash_function.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

class Block {
private:
    std::string previous_block_hash;
    std::string merkle_root_hash;
    int version;
    int difficulty;
    long long timestamp;
    long long nonce;

    std::vector<Transaction> transactions;
public:
    Block(std::string previous_block_hash_, std::string merkle_root_hash_, int version_, int difficulty_, long long timestamp_, long long nonce_, std::vector<Transaction> transactions_);

    std::string getPreviousBlockHash() const;
    std::string getMerkleRootHash() const;
    int getVersion() const;
    int getDifficulty() const;
    long long getTimestamp() const;
    long long getNonce() const;

    const std::vector<Transaction>& getTransactions() const;

    std::string toString() const;

    json toJson() const;

    std::string computePreviousBlockHash() const;

    static std::string computeMerkleRoot(const std::vector<Transaction>& transactions);

    void outputBlockInfo() const;
    void saveToFile(const std::string& path) const;

    bool mineBlock(int leadingZeros = 3);
    static bool hasLeadingZeros(const std::string& hexHash, int leadingZeros);
};

#endif
