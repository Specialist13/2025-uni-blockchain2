#include "block_storage.h"
#include "../../include/nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

BlockchainStorage::BlockchainStorage(const std::string& path)
    : file_path(path) {}

void BlockchainStorage::save(const std::vector<Block>& blocks) const {
    json j = json::array();
    for (const Block& b : blocks) {
        j.push_back(b.toJson());
    }
    std::ofstream ofs(file_path);
    ofs << j.dump(4);
}

std::vector<Block> BlockchainStorage::load() const {
    std::vector<Block> blocks;
    std::ifstream ifs(file_path);
    if (!ifs) return blocks;
    json j;
    ifs >> j;

    auto parse_block = [](const json& item) -> Block {
        std::string previous_block_hash = item.at("previous_block_hash");
        std::string merkle_root_hash = item.at("merkle_root_hash");
        int version = item.at("version");
        int difficulty = item.at("difficulty");
        long long timestamp = item.at("timestamp");
        long long nonce = item.at("nonce");

        std::vector<Transaction> txs;
        if (item.contains("transactions") && item["transactions"].is_array()) {
            for (const auto& txj : item["transactions"]) {
                std::string txid = txj.at("transaction_id");
                std::vector<TransactionInputs> ins;
                std::vector<TransactionOutputs> outs;
                if (txj.contains("inputs") && txj["inputs"].is_array()) {
                    for (const auto& inj : txj["inputs"]) {
                        ins.emplace_back(
                            inj.at("previous_transaction_id"),
                            inj.at("output_index"),
                            inj.at("signature")
                        );
                    }
                }
                if (txj.contains("outputs") && txj["outputs"].is_array()) {
                    for (const auto& outj : txj["outputs"]) {
                        outs.emplace_back(
                            outj.at("receiver_public_key"),
                            outj.at("amount")
                        );
                    }
                }
                txs.emplace_back(txid, ins, outs);
            }
        }
        return Block(previous_block_hash, merkle_root_hash, version, difficulty, timestamp, nonce, txs);
    };

    if (j.is_array()) {
        for (const auto& item : j) {
            if (!item.is_object()) continue;
            blocks.emplace_back(parse_block(item));
        }
    } else if (j.is_object()) {
        // Backward compatibility: single block object stored at top-level
        blocks.emplace_back(parse_block(j));
    }
    return blocks;
}
