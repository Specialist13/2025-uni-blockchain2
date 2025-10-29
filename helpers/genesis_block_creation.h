#ifndef GENESIS_BLOCK_CREATION_H
#define GENESIS_BLOCK_CREATION_H

#include <string>
#include <vector>
#include <fstream>
#include "../source/block/block.h"
#include "../source/transaction/transaction.h"
#include "../source/hash_function.h"
#include "../source/utxo/UTXO_set.h"

Block createGenesisBlock();

#endif
