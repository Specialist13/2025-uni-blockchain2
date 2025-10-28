#include "UTXO_set.h"

UTXOSet* UTXOSet::instance = nullptr;

bool UTXOSet::addUTXO(const UTXO& utxo) {
    auto result = utxos.insert(utxo);
    return result.second;
}

bool UTXOSet::removeUTXO(const UTXO& utxo) {
    return utxos.erase(utxo) > 0;
}

bool UTXOSet::containsUTXO(const UTXO& utxo) const {
    return utxos.find(utxo) != utxos.end();
}

size_t UTXOSet::size() const {
    return utxos.size();
}

std::vector<UTXO> UTXOSet::getAllUTXOsForAddress(const std::string& publicKey) const {
    std::vector<UTXO> result;
    for (const auto& utxo : utxos) {
        if (utxo.getReceiverPublicKey() == publicKey) {
            result.push_back(utxo);
        }
    }
    return result;
}

double UTXOSet::getBalanceForAddress(const std::string& publicKey) const {
    double balance = 0.0;
    for (const auto& utxo : utxos) {
        if (utxo.getReceiverPublicKey() == publicKey) {
            balance += utxo.getAmount();
        }
    }
    return balance;
}

void UTXOSet::clear() {
    utxos.clear();
}

void UTXOSet::outputToConsole() const {
    for (const auto& utxo : utxos) {
        std::cout << "UTXO - TxID: " << utxo.getTransactionID()
                  << ", Output Index: " << utxo.getOutputIndex()
                  << ", Receiver: " << utxo.getReceiverPublicKey()
                  << ", Amount: " << utxo.getAmount() << std::endl;
    }
}

void UTXOSet::saveToFile(const std::string& filename) const {
    json j;
    for (const auto& utxo : utxos) {
        j.push_back(utxo.toJson());
    }
    std::ofstream ofs(filename);
    ofs << j.dump(4);
}

void UTXOSet::loadFromFile(const std::string& filename) {
    json j;
    std::ifstream ifs(filename);
    ifs >> j;
    for (const auto& item : j) {
        utxos.insert(UTXO::fromJson(item));
    }
}
