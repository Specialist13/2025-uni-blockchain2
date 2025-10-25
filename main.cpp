#include "main.h"

int main() {
    //generate_users("./data/users.txt", 1000);
    //generate_transactions("./data/transactions.txt", 10000);
    UTXOSet* utxo_set = UTXOSet::getInstance();
    Block genesis_block=createGenesisBlock();
    utxo_set->saveToFile("./data/utxo_set.json");
    //genesis_block.outputBlockInfo();
    return 0;
}