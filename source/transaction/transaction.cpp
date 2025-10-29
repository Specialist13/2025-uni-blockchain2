#include "transaction.h"
#include "../utxo/UTXO.h"

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

bool Transaction::hasValidSignature() const {
    // Simplified signature verification - just check if signature is not empty
    // TODO: verify cryptographic signatures
    for (const TransactionInputs& input : inputs) {
        if (input.getSignature().empty()) {
            return false;
        }
    }
    return true;
}
