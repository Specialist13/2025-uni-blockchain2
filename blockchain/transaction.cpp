#include "transaction.h"

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
    for (const auto& input : inputs)
        serial += input.toString();
    for (const auto& output : outputs)
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
    for (const auto& input : inputs) {
        json input_obj;
        input_obj["previous_transaction_id"] = input.previous_transaction_id;
        input_obj["output_index"] = input.output_index;
        input_obj["signature"] = input.signature;
        inputs_array.push_back(input_obj);
    }
    j["inputs"] = inputs_array;
    
    json outputs_array = json::array();
    for (const auto& output : outputs) {
        json output_obj;
        output_obj["receiver_public_key"] = output.receiver_public_key;
        output_obj["amount"] = output.amount;
        outputs_array.push_back(output_obj);
    }
    j["outputs"] = outputs_array;

    return j;
}
