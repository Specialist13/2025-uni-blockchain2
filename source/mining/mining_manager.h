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

public:
    MiningManager(const std::string& queueFile,
                  const std::string& blockchainFile,
                  int difficulty_ = 3);

    void startMining(int txPerBlock = 100);
    std::vector<Transaction> selectValidTransactions(int count);
    bool mineAndCommitBlock(const std::vector<Transaction>& txs);
    void displayStatistics() const;
};

#endif
