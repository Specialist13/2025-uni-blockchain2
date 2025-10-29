#include "transaction_queue.h"
#include "../include/nlohmann/json.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_set>

using json = nlohmann::json;

TransactionQueue::TransactionQueue(const std::string& file_path_)
    : file_path(file_path_) {}

void TransactionQueue::loadFromFile() {
    pending_transactions.clear();
    std::ifstream ifs(file_path);
    if (!ifs) return;
    json j;
    ifs >> j;
    for (const auto& item : j) {
        // Each item is a Transaction JSON object
        std::string transaction_id = item["transaction_id"];
        std::vector<TransactionInputs> inputs;
        std::vector<TransactionOutputs> outputs;

        for (const auto& in : item["inputs"]) {
            inputs.emplace_back(
                in["previous_transaction_id"],
                in["output_index"],
                in["signature"]
            );
        }
        for (const auto& out : item["outputs"]) {
            outputs.emplace_back(
                out["receiver_public_key"],
                out["amount"]
            );
        }

        pending_transactions.emplace_back(transaction_id, inputs, outputs);
    }
}

void TransactionQueue::saveToFile() const {
    json j = json::array();
    for (const auto& tx : pending_transactions) {
        j.push_back(tx.toJson());
    }
    std::ofstream ofs(file_path);
    ofs << j.dump(4);
}

void TransactionQueue::addTransaction(const Transaction& tx) {
    pending_transactions.push_back(tx);
}

std::vector<Transaction> TransactionQueue::getRandomTransactions(int count) const {
    std::vector<int> indices(pending_transactions.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::mt19937 rng(std::random_device{}());
    std::shuffle(indices.begin(), indices.end(), rng);

    std::vector<Transaction> result;
    result.reserve(std::min<int>(count, indices.size()));
    for (int i = 0; i < count && i < static_cast<int>(indices.size()); ++i) {
        result.push_back(pending_transactions[indices[i]]);
    }
    return result;
}

void TransactionQueue::removeTransactions(const std::vector<Transaction>& txs) {
    std::unordered_set<std::string> ids;
    for (const auto& tx : txs) ids.insert(tx.getTransactionId());
    std::vector<Transaction> kept;
    kept.reserve(pending_transactions.size());
    for (const auto& tx : pending_transactions) {
        if (ids.find(tx.getTransactionId()) == ids.end()) kept.push_back(tx);
    }
    pending_transactions.swap(kept);
}

void TransactionQueue::clear() {
    pending_transactions.clear();
}

size_t TransactionQueue::size() const {
    return pending_transactions.size();
}
