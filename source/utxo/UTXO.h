#ifndef UTXO_H
#define UTXO_H

#include <string>
#include <vector>
#include <functional>
#include "../transaction/transaction.h"
#include "../../include/nlohmann/json.hpp"

using json = nlohmann::json;

class UTXO {
private:
    std::string transaction_id;
    int output_index;
    std::string receiver_public_key;
    double amount;

public:
    UTXO(const std::string& tx_id, int index, const std::string& receiver, double amt);

    std::string getTransactionID() const;
    int getOutputIndex() const;
    std::string getReceiverPublicKey() const;
    double getAmount() const;

    bool operator==(const UTXO& other) const;

    json toJson() const;

    static UTXO fromJson(const json& j);
};

namespace std {
    template <>
    struct hash<UTXO> {
        size_t operator()(const UTXO& utxo) const noexcept {
            return hash<string>()(utxo.getTransactionID()) ^ hash<int>()(utxo.getOutputIndex());
        }
    };
}

#endif
