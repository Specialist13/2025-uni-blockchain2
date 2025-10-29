#ifndef MINING_MANAGER_H
#define MINING_MANAGER_H

#include <vector>
#include <string>
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

public:
    MiningManager(const std::string& queueFile,
                  const std::string& blockchainFile,
                  int difficulty_ = 3);

    void startMining(int txPerBlock = 100, int maxBlocks = 2147483647);
    std::vector<Transaction> selectValidTransactions(int count);
    bool mineAndCommitBlock(const std::vector<Transaction>& txs);
    void displayStatistics() const;

    size_t getTotalBlocksMined() const { return totalBlocksMined; }
    double getLastHashrate() const { return lastHashrateHps; }
    double getAverageHashrate() const { return avgHashrateHps; }
    int getDifficulty() const { return difficulty; }
};

#endif
