#include "transaction.h"

Transaction::Transaction(std::string transaction_id_, std::string sender_public_key_, std::string receiver_public_key_, double amount_)
    : transaction_id(transaction_id_), sender_public_key(sender_public_key_), receiver_public_key(receiver_public_key_), amount(amount_) {}

std::string Transaction::getTransactionId() const { return transaction_id; }

std::string Transaction::getSenderPublicKey() const { return sender_public_key; }

std::string Transaction::getReceiverPublicKey() const { return receiver_public_key; }

double Transaction::getAmount() const { return amount; }

std::string Transaction::toString() const {
    return transaction_id + sender_public_key + receiver_public_key + std::to_string(amount);
}
