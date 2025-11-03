#ifndef MINING_POOL_H
#define MINING_POOL_H

#include "mining_manager.h"
#include <omp.h>
#include <vector>
#include <atomic>
#include <memory>
#include <unordered_set>
#include <string>

#include "../transaction/transaction_queue.h"
#include "../block/block_storage.h"
#include "../block/blockchain.h"
#include "../utxo/UTXO_set.h"
#include "../utxo/UTXO.h"
#include "../block/block.h"

class MiningPool {
private:
    std::vector<MiningManager> miners;
    std::string queueFile;
    std::string blockchainFile;
    int difficulty;
    int numMiners;
    
    std::shared_ptr<std::atomic<bool>> stopCurrentBlock;
    std::shared_ptr<std::atomic<bool>> blockFound;
    std::shared_ptr<std::atomic<int>> winningMiner;
    
    omp_lock_t blockCommitLock;
    
    // Shared state
    TransactionQueue sharedQueue;
    BlockchainStorage sharedStorage;
    Blockchain sharedChain;
    UTXOSet* utxo;
    
    // Snapshot of UTXO state for current mining round
    std::unordered_set<UTXO> utxoSnapshot;
    
public:
    MiningPool(int numMiners_,
               const std::string& queueFile_,
               const std::string& blockchainFile_,
               int difficulty_ = 3);
    
    ~MiningPool();

    void startMining(int txPerBlock = 100, int maxBlocks = 2147483647);
    
private:
    bool mineBlockCompetitive(int minerId,
                             const std::vector<Transaction>& txs,
                             Block& resultBlock);
    
    std::vector<Transaction> selectValidTransactionsWithSnapshot(int count);
    
    void captureUTXOSnapshot();
    
    bool validateTransactionAgainstSnapshot(const Transaction& tx) const;
};

#endif