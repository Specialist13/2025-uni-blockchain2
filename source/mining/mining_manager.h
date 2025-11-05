#ifndef MINING_MANAGER_H
#define MINING_MANAGER_H

#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include "../transaction/transaction_queue.h"
#include "../block/block.h"
#include "../block/blockchain.h"
#include "../block/block_storage.h"
#include "../utxo/UTXO_set.h"

class MiningManager {
private:
    TransactionQueue queue;
    Blockchain chain;
    BlockchainStorage storage;
    UTXOSet* utxo;

    int difficulty;

    // Stats
    size_t totalBlocksMined = 0;
    unsigned long long totalHashesTried = 0ULL;
    double lastBlockSeconds = 0.0;
    unsigned long long lastBlockHashes = 0ULL;
    double lastHashrateHps = 0.0; // hashes per second
    double avgHashrateHps = 0.0;
    
    std::string getRandomUserPublicKey() const;

public:
    MiningManager(const std::string& queueFile,
                  const std::string& blockchainFile,
                  int difficulty_ = 3);

    // startMining optionally accepts a shared winner flag and this miner's id.
    // If `winner` is non-null and another miner has already won (winner->load() != -1),
    // the method should return early.
    bool startMining(int txPerBlock = 100,
                     int maxBlocks = 2147483647,
                     std::shared_ptr<std::atomic<int>> winner = nullptr,
                     int minerId = -1);

    std::vector<Transaction> selectValidTransactions(int count);
    // Returns: 1 = mined & committed, 2 = timeout (no block found within timeoutSeconds), 0 = aborted/failure
    int mineAndCommitBlock(const std::vector<Transaction>& txs,
                           std::shared_ptr<std::atomic<int>> winner = nullptr,
                           int minerId = -1,
                           double timeoutSeconds = 5.0);
    void displayStatistics() const;

    size_t getTotalBlocksMined() const { return totalBlocksMined; }
    double getLastHashrate() const { return lastHashrateHps; }
    double getAverageHashrate() const { return avgHashrateHps; }
    int getDifficulty() const { return difficulty; }
};

#endif
