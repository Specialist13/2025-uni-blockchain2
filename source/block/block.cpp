#include "block.h"
#include <sstream>

static std::string indent_block(const std::string& s, int spaces) {
    std::istringstream iss(s);
    std::string line;
    std::string out;
    std::string pad(spaces, ' ');
    bool first = true;
    while (std::getline(iss, line)) {
        if (!first) out += "\n";
        out += pad + line;
        first = false;
    }
    return out;
}

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
    std::string header = previous_block_hash + merkle_root_hash + std::to_string(version) + std::to_string(difficulty) + std::to_string(timestamp) + std::to_string(nonce);
    std::string tmp = header;
    return SlaSimHash(tmp);
}

std::string Block::computeMerkleRoot(const std::vector<Transaction>& transactions) {
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

bool Block::hasLeadingZeros(const std::string& hexHash, int leadingZeros) {
    if (leadingZeros <= 0) return true;
    for (int i = 0; i < leadingZeros; ++i) {
        if (i >= hexHash.size()) return false;
        if (hexHash[i] != '0') return false;
    }
    return true;
}

bool Block::mineBlock(int leadingZeros) {
    // TODO: Ensure merkle root is consistent with current transactions
    merkle_root_hash = Block::computeMerkleRoot(transactions);

    long long iterations = 0;
    while (true) {
        std::string header = previous_block_hash + merkle_root_hash + std::to_string(version) + std::to_string(difficulty) + std::to_string(timestamp) + std::to_string(nonce);
        std::string tmp = header;
        std::string hash = SlaSimHash(tmp);
        if (Block::hasLeadingZeros(hash, leadingZeros)) {
            std::cout << "Mined block with nonce=" << nonce << ", hash=" << hash << std::endl;
            return true;
        }
        ++nonce;
        ++iterations;
        if (iterations % 500000 == 0) {
            std::cout << "Mining... nonce=" << nonce << " lastHashPrefix=" << hash.substr(0, 8) << std::endl;
        }
        // TODO: add a guard to break after very long attempts (omitted for v0.1)
    }
}

void Block::outputBlockInfo() const {
    json j = toJson();
    std::cout << j.dump(4) << std::endl;
}

json Block::toJson() const {
    json j;
    j["previous_block_hash"] = previous_block_hash;
    j["merkle_root_hash"] = merkle_root_hash;
    j["version"] = version;
    j["difficulty"] = difficulty;
    j["timestamp"] = timestamp;
    j["nonce"] = nonce;

    json txs = json::array();
    for (const auto& tx : transactions) {
        txs.push_back(tx.toJson());
    }
    j["transactions"] = txs;

    return j;
}


void Block::saveToFile(const std::string& path) const {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }
    json j = toJson();
    ofs << j.dump(4);
    ofs.close();
}
