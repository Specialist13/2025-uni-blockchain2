#include "UTXO.h"

UTXO::UTXO(const std::string& tx_id, int index, const std::string& receiver, double amt)
        : transaction_id(tx_id), output_index(index), receiver_public_key(receiver), amount(amt) {}

std::string UTXO::getTransactionID() const { return transaction_id; }
int UTXO::getOutputIndex() const { return output_index; }
std::string UTXO::getReceiverPublicKey() const { return receiver_public_key; }
double UTXO::getAmount() const { return amount; }

bool UTXO::operator==(const UTXO& other) const {
    return transaction_id == other.transaction_id && output_index == other.output_index;
}

json UTXO::toJson() const {
    json j;
    j["transaction_id"] = transaction_id;
    j["output_index"] = output_index;
    j["receiver_public_key"] = receiver_public_key;
    j["amount"] = amount;
    return j;
}

UTXO UTXO::fromJson(const json& j) {
    std::string tx_id = j["transaction_id"];
    int index = j["output_index"];
    std::string receiver = j["receiver_public_key"];
    double amt = j["amount"];
    return UTXO(tx_id, index, receiver, amt);
}
