#include "block.h"

Block::Block(std::string previous_block_hash_, std::string merkle_root_hash_, int version_, int difficulty_, long long timestamp_, long long nonce_, std::vector<Transaction> transactions_)
    : previous_block_hash(previous_block_hash_), merkle_root_hash(merkle_root_hash_), version(version_), difficulty(difficulty_), timestamp(timestamp_), nonce(nonce_), transactions(transactions_) {}

std::string Block::getPreviousBlockHash() const { return previous_block_hash; }

std::string Block::getMerkleRootHash() const { return merkle_root_hash; }

int Block::getVersion() const { return version; }

int Block::getDifficulty() const { return difficulty; }

long long Block::getTimestamp() const { return timestamp; }

long long Block::getNonce() const { return nonce; }

const std::vector<Transaction>& Block::getTransactions() const { return transactions; }

std::string Block::toString() const {
    std::string txs;
    for (const auto& tx : transactions) {
        txs += tx.toString();
    }
    return previous_block_hash + merkle_root_hash + std::to_string(version) + std::to_string(difficulty) + std::to_string(timestamp) + std::to_string(nonce) + txs;
}

std::string Block::computePreviousBlockHash() const {
    
}

std::string computeMerkleRoot(const std::vector<Transaction>& transactions) {
    if (transactions.empty()) return std::string(64, '0');

    std::vector<std::string> layer;
    for (const auto& tx : transactions) {
        layer.push_back(tx.computeTransactionHash());
    }

    while (layer.size() > 1) {
        if (layer.size() % 2 != 0) {
            layer.push_back(layer.back());
        }

        std::vector<std::string> new_layer;
        for (int i = 0; i < layer.size(); i += 2) {
            std::string combined = layer[i] + layer[i + 1];
            new_layer.push_back(SlaSimHash(combined));
        }
        layer = new_layer;
    }

    return layer.front();
}

void Block::outputBlockInfo() const {
    
}