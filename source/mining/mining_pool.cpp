#include "mining_pool.h"
#include <iostream>
#include <memory>

MiningPool::MiningPool(int numMiners,
                       const std::string& queueFile,
                       const std::string& blockchainFile,
                       int difficultyPerMiner)
    : queueFile(queueFile), blockchainFile(blockchainFile), difficulty(difficultyPerMiner), numMiners(numMiners),
      sharedQueue(queueFile), sharedStorage(blockchainFile) {
    omp_init_lock(&blockCommitLock);
    utxo = UTXOSet::getInstance();
    // create miners (they'll each have their own state files but share nothing here)
    for (int i = 0; i < numMiners; ++i) {
        miners.emplace_back(queueFile, blockchainFile, difficultyPerMiner);
    }
}

MiningPool::~MiningPool() {
    omp_destroy_lock(&blockCommitLock);
}

void MiningPool::startMining(int txPerBlock, int maxBlocksPerMiner) {
    // Shared atomic that holds the winning miner id, -1 means none yet
    auto winner = std::make_shared<std::atomic<int>>(-1);

    // Run each miner concurrently; each miner will check `winner` periodically
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int>(miners.size()); ++i) {
        miners[i].startMining(txPerBlock, maxBlocksPerMiner, winner, i);
    }
}
