#ifndef UTXO_SET_H
#define UTXO_SET_H

#include <unordered_set>
#include <string>
#include <fstream>
#include "UTXO.h"

class UTXOSet {
private:
    static UTXOSet* instance;
    std::unordered_set<UTXO> utxos;

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

    std::vector<UTXO> getAllUTXOsForAddress(const std::string& publicKey) const;
    double getBalanceForAddress(const std::string& publicKey) const;
    void clear();

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};
#endif