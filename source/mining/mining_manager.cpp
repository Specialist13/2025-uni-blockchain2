#include "mining_manager.h"
#include <ctime>
#include <iostream>

MiningManager::MiningManager(const std::string& queueFile,
                             const std::string& blockchainFile,
                             int difficulty_)
    : queue(queueFile), storage(blockchainFile), utxo(UTXOSet::getInstance()), difficulty(difficulty_) {
    queue.loadFromFile();
    chain.setBlocks(storage.load());
}

void MiningManager::startMining(int txPerBlock) {
    std::cout << "Starting mining. Pending txs: " << queue.size() << std::endl;
    while (queue.size() > 0) {
        queue.loadFromFile();
        std::vector<Transaction> txs = selectValidTransactions(txPerBlock);
        if (txs.empty()) {
            std::cout << "No valid transactions available. Stopping." << std::endl;
            break;
        }
        if (!mineAndCommitBlock(txs)) {
            std::cout << "Failed to mine/commit block. Stopping." << std::endl;
            break;
        }
        displayStatistics();
    }
}

std::vector<Transaction> MiningManager::selectValidTransactions(int count) {
    int multiplier = 10;
    std::vector<Transaction> candidates = queue.getRandomTransactions(std::max(count * multiplier, count));
    std::vector<Transaction> selected;
    selected.reserve(count);

    for (const Transaction& tx : candidates) {
        if (utxo->validateTransaction(tx)) {
            selected.push_back(tx);
            if (static_cast<int>(selected.size()) == count) break;
        }
    }
    return selected;
}

bool MiningManager::mineAndCommitBlock(const std::vector<Transaction>& txs) {
    std::string previousHash = chain.getBlockCount() > 0
        ? Blockchain::computeBlockHash(chain.getLatestBlock())
        : std::string(64, '0');

    int version = 1;
    long long timestamp = static_cast<long long>(std::time(nullptr));
    long long nonce = 0;

    std::string merkle = Block::computeMerkleRoot(txs);
    Block block(previousHash, merkle, version, difficulty, timestamp, nonce, txs);

    std::cout << "Mining block with " << txs.size() << " txs..." << std::endl;
    const auto start = std::chrono::steady_clock::now();
    if (!block.mineBlock(difficulty)) {
        return false;
    }
    const auto end = std::chrono::steady_clock::now();
    lastBlockSeconds = std::chrono::duration<double>(end - start).count();
    lastBlockHashes = static_cast<unsigned long long>(block.getNonce());
    lastHashrateHps = (lastBlockSeconds > 0.0) ? (static_cast<double>(lastBlockHashes) / lastBlockSeconds) : 0.0;
    ++totalBlocksMined;
    totalHashesTried += lastBlockHashes;
    // Rolling simple average per-block hashrate approximation
    avgHashrateHps = (totalBlocksMined > 0)
        ? ((avgHashrateHps * (static_cast<double>(totalBlocksMined - 1)) + lastHashrateHps) / static_cast<double>(totalBlocksMined))
        : lastHashrateHps;

    // Process: transactions (update UTXO set)
    for (const Transaction& tx : txs) {
        utxo->processTransaction(tx);
    }

    if (!chain.addBlock(block)) {
        std::cerr << "Block chaining validation failed." << std::endl;
        return false;
    }

    // Persist: blockchain, UTXO set, and remove mined txs from queue
    storage.save(chain.getBlocks());
    utxo->saveToFile("data/utxo_set.json");
    queue.removeTransactions(txs);
    queue.saveToFile();

    std::cout << "Block committed. Chain height: " << chain.getBlockCount() << std::endl;
    return true;
}

void MiningManager::displayStatistics() const {
    std::cout << "Chain height: " << chain.getBlockCount() << std::endl;
    std::cout << "Blocks mined (session): " << totalBlocksMined << std::endl;
    std::cout << "Last block: hashes=" << lastBlockHashes << ", time=" << lastBlockSeconds << "s, hashrate="
              << lastHashrateHps << " H/s" << std::endl;
    std::cout << "Avg hashrate: " << avgHashrateHps << " H/s, Difficulty: " << difficulty << std::endl;
}
