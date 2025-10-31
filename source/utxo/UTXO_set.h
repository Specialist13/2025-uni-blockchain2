#ifndef UTXO_SET_H
#define UTXO_SET_H

#include <unordered_set>
#include <fstream>
#include <iostream>
#include "UTXO.h"
#include "../transaction/transaction.h"

class UTXOSet {
private:
    static UTXOSet* instance;
    std::unordered_set<UTXO> utxos;

    std::unordered_set<UTXO> mempool_utxos;

    std::unordered_set<UTXO> reserved_utxos;

    UTXOSet() {};
    
    UTXOSet(const UTXOSet&) = delete;
    UTXOSet& operator=(const UTXOSet&) = delete;

public:
    static UTXOSet* getInstance() {
        if (instance == nullptr) {
            instance = new UTXOSet();
        }
        return instance;
    }

    bool addUTXO(const UTXO& utxo);
    bool removeUTXO(const UTXO& utxo);
    bool containsUTXO(const UTXO& utxo) const;
    size_t size() const;

    bool addMempoolUTXO(const UTXO& utxo);
    bool removeMempoolUTXO(const UTXO& utxo);
    bool containsMempoolUTXO(const UTXO& utxo) const;
    size_t sizeMempool() const;

    bool reserveUTXO(const UTXO& utxo);
    bool unreserveUTXO(const UTXO& utxo);
    bool isReserved(const UTXO& utxo) const;
    std::vector<UTXO> getAvailableUTXOsForAddress(const std::string& publicKey) const;
    double getAvailableBalanceForAddress(const std::string& publicKey) const;

    std::vector<UTXO> getAllUTXOsForAddress(const std::string& publicKey) const;
    double getBalanceForAddress(const std::string& publicKey) const;
    void clear();

    void outputToConsole() const;

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    
    bool spendUTXOs(const std::vector<TransactionInputs>& inputs);
    void addTransactionOutputs(const std::string& transaction_id, const std::vector<TransactionOutputs>& outputs);
    bool validateTransaction(const Transaction& tx) const;
    void processTransaction(const Transaction& tx);
    std::vector<UTXO> getUTXOsForTransaction(const std::string& transaction_id) const;
};
#endif
