#include "transaction.h"
#include <sstream>
#include <iomanip>

// Helper: indent each line of `s` by `spaces` spaces.
static std::string indent(const std::string& s, int spaces) {
    std::istringstream iss(s);
    std::string line;
    std::string out;
    std::string pad(spaces, ' ');
    bool first = true;
    while (std::getline(iss, line)) {
        if (!first) out += "\n";
        out += pad + line;
        first = false;
    }
    return out;
}

TransactionInputs::TransactionInputs(std::string previous_transaction_id_, int output_index_, std::string signature_)
    : previous_transaction_id(std::move(previous_transaction_id_)),
      output_index(output_index_),
      signature(std::move(signature_)) {}

std::string TransactionInputs::toString() const {
    return previous_transaction_id + std::to_string(output_index) + signature;
}

std::string TransactionInputs::toJson() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"previous_transaction_id\": \"" << previous_transaction_id << "\",\n";
    ss << "  \"output_index\": " << output_index << ",\n";
    ss << "  \"signature\": \"" << signature << "\"\n";
    ss << "}";
    return ss.str();
}

TransactionOutputs::TransactionOutputs(std::string receiver_public_key_, double amount_)
    : receiver_public_key(std::move(receiver_public_key_)), amount(amount_) {}

std::string TransactionOutputs::toString() const {
    return receiver_public_key + std::to_string(amount);
}

std::string TransactionOutputs::toJson() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"receiver_public_key\": \"" << receiver_public_key << "\",\n";
    ss << "  \"amount\": " << std::fixed << std::setprecision(8) << amount << "\n";
    ss << "}";
    return ss.str();
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

std::string Transaction::toJson() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"transaction_id\": \"" << transaction_id << "\",\n";

    ss << "  \"inputs\": [\n";
    for (size_t i = 0; i < inputs.size(); ++i) {
        ss << indent(inputs[i].toJson(), 4);
        if (i + 1 < inputs.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ],\n";

    ss << "  \"outputs\": [\n";
    for (size_t i = 0; i < outputs.size(); ++i) {
        ss << indent(outputs[i].toJson(), 4);
        if (i + 1 < outputs.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";

    ss << "}";
    return ss.str();
}
