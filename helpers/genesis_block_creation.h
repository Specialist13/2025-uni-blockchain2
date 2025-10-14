#ifndef GENESIS_BLOCK_CREATION_H
#define GENESIS_BLOCK_CREATION_H

#include <string>
#include <vector>
#include <fstream>
#include "../blockchain/block.h"
#include "../blockchain/transaction.h"
#include "../blockchain/hash_function.h"

Block createGenesisBlock();

#endif