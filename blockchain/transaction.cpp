#include "transaction.h"

TransactionInputs::TransactionInputs(std::string previous_transaction_id_, int output_index_, std::string signature_)
    : previous_transaction_id(std::move(previous_transaction_id_)), output_index(output_index_), signature(std::move(signature_)) {}

std::string TransactionInputs::toString() const {
    return previous_transaction_id + std::to_string(output_index) + signature;
}

TransactionOutputs::TransactionOutputs(std::string receiver_public_key_, double amount_)
    : receiver_public_key(std::move(receiver_public_key_)), amount(amount_) {}

std::string TransactionOutputs::toString() const {
    return receiver_public_key + std::to_string(amount);
}

Transaction::Transaction(std::string transaction_id_, std::vector<TransactionInputs> inputs_, std::vector<TransactionOutputs> outputs_)
    : transaction_id(transaction_id_), inputs(std::move(inputs_)), outputs(std::move(outputs_)) {}

std::string Transaction::getTransactionId() const { return transaction_id; }

const std::vector<TransactionInputs>& Transaction::getInputs() const { return inputs; }

const std::vector<TransactionOutputs>& Transaction::getOutputs() const { return outputs; }

std::string Transaction::toString() const {
    std::string serial;
    for (const auto& input : inputs) serial += input.toString();
    for (const auto& output : outputs) serial += output.toString();
    return transaction_id + serial;
}
