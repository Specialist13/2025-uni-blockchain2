#include "block.h"
#include <sstream>

// Helper: indent each line of `s` by `spaces` spaces.
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
    // Compute a hash over the previous block hash and basic header fields
    std::string header = previous_block_hash + merkle_root_hash + std::to_string(version) + std::to_string(difficulty) + std::to_string(timestamp) + std::to_string(nonce);
    // SlaSimHash expects a non-const reference, create a temp
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

void Block::outputBlockInfo() const {
    std::string json = toJson();

    std::cout << json << std::endl;

    saveToJson("./data/blockchain.json");
}

std::string Block::toJson() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"previous_block_hash\": \"" << previous_block_hash << "\",\n";
    ss << "  \"merkle_root_hash\": \"" << merkle_root_hash << "\",\n";
    ss << "  \"version\": " << version << ",\n";
    ss << "  \"difficulty\": " << difficulty << ",\n";
    ss << "  \"timestamp\": " << timestamp << ",\n";
    ss << "  \"nonce\": " << nonce << ",\n";
    ss << "  \"transactions\": [\n";

    for (size_t i = 0; i < transactions.size(); ++i) {
        ss << indent_block(transactions[i].toJson(), 4);
        if (i + 1 < transactions.size()) ss << ",";
        ss << "\n";
    }

    ss << "  ]\n";
    ss << "}";
    return ss.str();
}


void Block::saveToJson(const std::string& path) const {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }
    ofs << toJson();
    ofs.close();
}