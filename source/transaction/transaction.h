#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include "../hash_function.h"
#include "../../include/nlohmann/json.hpp"

using json = nlohmann::json;

// Forward declaration to avoid circular dependency
class UTXO;

class TransactionInputs {
private:
    std::string previous_transaction_id;
    int output_index;
    std::string signature;

public:
    TransactionInputs(std::string previous_transaction_id_, int output_index_, std::string signature_);
    std::string toString() const;
    
    std::string getPreviousTransactionId() const { return previous_transaction_id; }
    int getOutputIndex() const { return output_index; }
    std::string getSignature() const { return signature; }
    
    friend class Transaction;
};

class TransactionOutputs {
private:
    std::string receiver_public_key;
    double amount;

public:
    TransactionOutputs(std::string receiver_public_key_, double amount_);
    std::string toString() const;
    
    std::string getReceiverPublicKey() const { return receiver_public_key; }
    double getAmount() const { return amount; }
    
    friend class Transaction;
};

class Transaction {
private:
    std::string transaction_id;
    std::vector<TransactionInputs> inputs;
    std::vector<TransactionOutputs> outputs;
public:
    Transaction(std::string transaction_id_, std::vector<TransactionInputs> inputs_, std::vector<TransactionOutputs> outputs_);

    std::string getTransactionId() const;
    const std::vector<TransactionInputs>& getInputs() const;
    const std::vector<TransactionOutputs>& getOutputs() const;

    std::string toString() const;
    json toJson() const;

    std::string computeTransactionHash() const;
    
    bool isValid() const;
    bool validateInputs(const std::vector<UTXO>& available_utxos) const;
    bool validateOutputs() const;
    double getTotalInputAmount(const std::vector<UTXO>& available_utxos) const;
    double getTotalOutputAmount() const;
    bool hasValidSignature() const;
};

#endif
