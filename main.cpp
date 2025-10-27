#include "main.h"

int main() {
    //generate_users("./data/users.txt", 1000);
    UTXOSet* utxo_set = UTXOSet::getInstance();
    utxo_set->loadFromFile("./data/utxo_set.json");
    generate_transactions("./data/transactions.json", 10000);
    
    //Block genesis_block=createGenesisBlock();
    //utxo_set->saveToFile("./data/utxo_set.json");
    //genesis_block.outputBlockInfo();
    return 0;
}