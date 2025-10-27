#include "genesis_block_creation.h"


Block createGenesisBlock() {
    std::vector<Transaction> transactions;
    std::ifstream users("./data/users.txt");

    std::string first_name, last_name, public_key, private_key;
    int amount;

    while (users >> first_name >> last_name >> public_key >> private_key >> amount) {
        std::string to_hash = public_key + public_key + std::to_string(amount);
        std::string transaction_id = SlaSimHash(to_hash);
        transactions.push_back(Transaction(transaction_id, {}, {TransactionOutputs(public_key, amount)}));
        UTXOSet::getInstance()->addUTXO(UTXO(transaction_id, 0, public_key, amount));
    }

    Block genesis_block(std::string(64, '0'), Block::computeMerkleRoot(transactions), 1, 1, 0, 0, transactions);
    return genesis_block;
}