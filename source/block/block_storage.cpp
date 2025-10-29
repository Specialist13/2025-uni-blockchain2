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
    for (const auto& item : j) {
        std::string previous_block_hash = item["previous_block_hash"];
        std::string merkle_root_hash = item["merkle_root_hash"];
        int version = item["version"];
        int difficulty = item["difficulty"];
        long long timestamp = item["timestamp"];
        long long nonce = item["nonce"];

        std::vector<Transaction> txs;
        for (const auto& txj : item["transactions"]) {
            std::string txid = txj["transaction_id"];
            std::vector<TransactionInputs> ins;
            std::vector<TransactionOutputs> outs;
            for (const auto& inj : txj["inputs"]) {
                ins.emplace_back(
                    inj["previous_transaction_id"],
                    inj["output_index"],
                    inj["signature"]
                );
            }
            for (const auto& outj : txj["outputs"]) {
                outs.emplace_back(
                    outj["receiver_public_key"],
                    outj["amount"]
                );
            }
            txs.emplace_back(txid, ins, outs);
        }

        blocks.emplace_back(previous_block_hash, merkle_root_hash, version, difficulty, timestamp, nonce, txs);
    }
    return blocks;
}
