#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
private:
    std::string transaction_id;
    std::string sender_public_key;
    std::string receiver_public_key;
    double amount;
public:
    Transaction(std::string transaction_id_, std::string sender_public_key_, std::string receiver_public_key_, double amount_);

    std::string getTransactionId() const;
    std::string getSenderPublicKey() const;
    std::string getReceiverPublicKey() const;
    double getAmount() const;

    std::string toString() const;
};

#endif