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
    for (const UTXO& utxo : utxos) {
        if (utxo.getReceiverPublicKey() == publicKey) {
            result.push_back(utxo);
        }
    }
    return result;
}

double UTXOSet::getBalanceForAddress(const std::string& publicKey) const {
    double balance = 0.0;
    for (const UTXO& utxo : utxos) {
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
    for (const UTXO& utxo : utxos) {
        std::cout << "UTXO - TxID: " << utxo.getTransactionID()
                  << ", Output Index: " << utxo.getOutputIndex()
                  << ", Receiver: " << utxo.getReceiverPublicKey()
                  << ", Amount: " << utxo.getAmount() << std::endl;
    }
}

void UTXOSet::saveToFile(const std::string& filename) const {
    json j;
    for (const UTXO& utxo : utxos) {
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

bool UTXOSet::spendUTXOs(const std::vector<TransactionInputs>& inputs) {
    for (const TransactionInputs& input : inputs) {
        UTXO target_utxo(input.getPreviousTransactionId(), input.getOutputIndex(), "", 0.0);
        if (!containsUTXO(target_utxo)) {
            std::cerr << "UTXO not found: " << input.getPreviousTransactionId() 
                      << " output " << input.getOutputIndex() << std::endl;
            return false;
        }
    }
    
    for (const TransactionInputs& input : inputs) {
        UTXO target_utxo(input.getPreviousTransactionId(), input.getOutputIndex(), "", 0.0);
        removeUTXO(target_utxo);
    }
    
    return true;
}

void UTXOSet::addTransactionOutputs(const std::string& transaction_id, const std::vector<TransactionOutputs>& outputs) {
    for (int i = 0; i < static_cast<int>(outputs.size()); ++i) {
        UTXO new_utxo(transaction_id, i, outputs[i].getReceiverPublicKey(), outputs[i].getAmount());
        addUTXO(new_utxo);
    }
}

bool UTXOSet::validateTransaction(const Transaction& tx) const {
    std::vector<UTXO> available_utxos;
    for (const UTXO& utxo : utxos) {
        available_utxos.push_back(utxo);
    }
    
    if (!tx.isValid() || !tx.validateInputs(available_utxos) || !tx.validateOutputs()) {
        return false;
    }
    
    double input_amount = tx.getTotalInputAmount(available_utxos);
    double output_amount = tx.getTotalOutputAmount();
    
    if (input_amount < output_amount) {
        std::cerr << "Insufficient funds: input=" << input_amount 
                  << " output=" << output_amount << std::endl;
        return false;
    }
    
    if (!tx.hasValidSignature()) {
        return false;
    }
    
    return true;
}

void UTXOSet::processTransaction(const Transaction& tx) {
    // Validate transaction first
    if (!validateTransaction(tx)) {
        std::cerr << "Transaction validation failed: " << tx.getTransactionId() << std::endl;
        return;
    }
    
    // Spend input UTXOs
    if (!spendUTXOs(tx.getInputs())) {
        std::cerr << "Failed to spend UTXOs for transaction: " << tx.getTransactionId() << std::endl;
        return;
    }
    
    // Add output UTXOs
    addTransactionOutputs(tx.getTransactionId(), tx.getOutputs());
    
    std::cout << "Successfully processed transaction: " << tx.getTransactionId() << std::endl;
}

std::vector<UTXO> UTXOSet::getUTXOsForTransaction(const std::string& transaction_id) const {
    std::vector<UTXO> result;
    for (const UTXO& utxo : utxos) {
        if (utxo.getTransactionID() == transaction_id) {
            result.push_back(utxo);
        }
    }
    return result;
}
