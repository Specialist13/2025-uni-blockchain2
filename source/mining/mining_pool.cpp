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

void MiningPool::startMining(int txPerBlock, int maxBlocks) {
    // Mine up to `maxBlocks` total across the pool. For each block we run a
    // competitive round where all miners race; the winner atomic is reset
    // per-round so that multiple blocks can be mined in sequence.
    int mined = 0;
    while (mined < maxBlocks) {
        sharedQueue.loadFromFile();
        if (sharedQueue.size() == 0) {
            std::cout << "No transactions left in queue. Stopping mining pool." << std::endl;
            break;
        }

        // fresh winner for this round
        auto winner = std::make_shared<std::atomic<int>>(-1);

        // Run each miner concurrently; each miner will try to mine a single block
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(miners.size()); ++i) {
            // ask each miner to attempt at most 1 block in this round
            miners[i].startMining(txPerBlock, 1, winner, i);
        }

        int win = winner->load();
        if (win != -1) {
            ++mined;
            std::cout << "Round complete: miner " << win << " won. Total mined by pool this run: " << mined << std::endl;
            // continue to next round to mine another block
            continue;
        } else {
            // no winner in this round; likely miners timed out. Stop to avoid busy-looping
            std::cout << "No miner found a block in this round. Stopping mining pool." << std::endl;
            break;
        }
    }
}
