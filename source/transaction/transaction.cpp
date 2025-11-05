#include "transaction.h"
#include "../utxo/UTXO.h"
#include "../keys/store.h"
#include <cctype>
#include <iostream>

TransactionInputs::TransactionInputs(std::string previous_transaction_id_, int output_index_, std::string signature_)
    : previous_transaction_id(std::move(previous_transaction_id_)),
      output_index(output_index_),
      signature(std::move(signature_)) {}

std::string TransactionInputs::toString() const {
    return previous_transaction_id + std::to_string(output_index) + signature;
}

TransactionOutputs::TransactionOutputs(std::string receiver_public_key_, double amount_)
    : receiver_public_key(std::move(receiver_public_key_)), amount(amount_) {}

std::string TransactionOutputs::toString() const {
    return receiver_public_key + std::to_string(amount);
}

Transaction::Transaction(std::string transaction_id_, std::vector<TransactionInputs> inputs_, std::vector<TransactionOutputs> outputs_)
    : transaction_id(std::move(transaction_id_)),
      inputs(std::move(inputs_)),
      outputs(std::move(outputs_)) {}

std::string Transaction::getTransactionId() const { return transaction_id; }
const std::vector<TransactionInputs>& Transaction::getInputs() const { return inputs; }
const std::vector<TransactionOutputs>& Transaction::getOutputs() const { return outputs; }

std::string Transaction::toString() const {
    std::string serial;
    for (const TransactionInputs& input : inputs)
        serial += input.toString();
    for (const TransactionOutputs& output : outputs)
        serial += output.toString();
    return transaction_id + serial;
}

std::string Transaction::computeTransactionHash() const {
    std::string str = Transaction::toString();
    return SlaSimHash(str);
}

json Transaction::toJson() const {
    json j;
    j["transaction_id"] = transaction_id;
    
    json inputs_array = json::array();
    for (const TransactionInputs& input : inputs) {
        json input_obj;
        input_obj["previous_transaction_id"] = input.previous_transaction_id;
        input_obj["output_index"] = input.output_index;
        input_obj["signature"] = input.signature;
        inputs_array.push_back(input_obj);
    }
    j["inputs"] = inputs_array;
    
    json outputs_array = json::array();
    for (const TransactionOutputs& output : outputs) {
        json output_obj;
        output_obj["receiver_public_key"] = output.receiver_public_key;
        output_obj["amount"] = output.amount;
        outputs_array.push_back(output_obj);
    }
    j["outputs"] = outputs_array;

    return j;
}

bool Transaction::isValid() const {
    // Coinbase transactions are valid even with synthetic input
    if (isCoinbase()) {
        if (outputs.empty()) {
            return false;
        }
        return validateOutputs();
    }
    
    if (inputs.empty() || outputs.empty()) {
        return false;
    }
    
    if (transaction_id.empty()) {
        return false;
    }
    
    if (!validateOutputs()) {
        return false;
    }
    
    return true;
}

bool Transaction::validateInputs(const std::vector<UTXO>& available_utxos) const {
    // Check if all inputs reference valid UTXOs by going through the available UTXOs and
    // Checking if the previous transaction id and output index match
    for (const TransactionInputs& input : inputs) {
        bool found = false;
        for (const UTXO& utxo : available_utxos) {
            if (utxo.getTransactionID() == input.getPreviousTransactionId() && 
                utxo.getOutputIndex() == input.getOutputIndex()) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool Transaction::validateOutputs() const {
    for (const TransactionOutputs& output : outputs) {
        if (output.getAmount() <= 0) {
            return false;
        }

        if (output.getReceiverPublicKey().empty()) {
            return false;
        }
    }
    return true;
}

double Transaction::getTotalInputAmount(const std::vector<UTXO>& available_utxos) const {
    double total = 0.0;
    for (const TransactionInputs& input : inputs) {
        for (const UTXO& utxo : available_utxos) {
            if (utxo.getTransactionID() == input.getPreviousTransactionId() && 
                utxo.getOutputIndex() == input.getOutputIndex()) {
                total += utxo.getAmount();
                break;
            }
        }
    }
    return total;
}

double Transaction::getTotalOutputAmount() const {
    double total = 0.0;
    for (const TransactionOutputs& output : outputs) {
        total += output.getAmount();
    }
    return total;
}

bool Transaction::hasValidSignature(const std::vector<UTXO>& available_utxos) const {
    const size_t EXPECTED_SIGNATURE_LENGTH = 64;
    
    Keys::Store* key_store = Keys::Store::getInstance();
    
    for (const TransactionInputs& input : inputs) {
        if (input.getSignature().empty() || input.getSignature().length() != EXPECTED_SIGNATURE_LENGTH) {
            return false;
        }
        
        const UTXO* referenced_utxo = nullptr;
        for (const UTXO& utxo : available_utxos) {
            if (utxo.getTransactionID() == input.getPreviousTransactionId() && 
                utxo.getOutputIndex() == input.getOutputIndex()) {
                referenced_utxo = &utxo;
                break;
            }
        }
        
        if (referenced_utxo == nullptr) { return false; }
        
        std::string utxo_owner_public_key = referenced_utxo->getReceiverPublicKey();
        std::string private_key = key_store->getPrivateKey(utxo_owner_public_key);
        
        if (private_key.empty()) {
            std::cerr << "Private key not found for public key: " << utxo_owner_public_key << std::endl;
            return false;
        }
        
        // Reconstruct the expected signature
        // Signature format: SlaSimHash(transaction_id + output_index + private_key)
        std::string signature_input = input.getPreviousTransactionId() + 
                                      std::to_string(input.getOutputIndex()) + 
                                      private_key;
        std::string expected_signature = SlaSimHash(signature_input);
        
        if (expected_signature != input.getSignature()) {
            std::cerr << "Signature mismatch for input: tx_id=" << input.getPreviousTransactionId() 
                      << ", output_index=" << input.getOutputIndex() << std::endl;
            return false;
        }
    }
    
    return true;
}

Transaction Transaction::createCoinbaseTransaction(const std::string& receiver_public_key, double amount) {
    std::vector<TransactionInputs> coinbase_inputs;
    coinbase_inputs.push_back(TransactionInputs("COINBASE", -1, ""));
    
    std::vector<TransactionOutputs> coinbase_outputs;
    coinbase_outputs.push_back(TransactionOutputs(receiver_public_key, amount));
    
    // Compute transaction ID from coinbase-specific data
    std::string coinbase_data = "COINBASE" + std::to_string(-1) + receiver_public_key + std::to_string(amount);
    std::string transaction_id = SlaSimHash(coinbase_data);
    
    return Transaction(transaction_id, coinbase_inputs, coinbase_outputs);
}

bool Transaction::isCoinbase() const {
    if (inputs.size() != 1) {
        return false;
    }
    const TransactionInputs& input = inputs[0];
    return input.getPreviousTransactionId() == "COINBASE" && input.getOutputIndex() == -1;
}
