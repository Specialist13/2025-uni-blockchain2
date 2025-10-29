#ifndef GENERATE_TRANSACTIONS_H
#define GENERATE_TRANSACTIONS_H

#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include "../source/hash_function.h"
#include "../source/utxo/UTXO_set.h"
#include "../source/transaction/transaction.h"
#include "../include/nlohmann/json.hpp"

void generate_transactions(const std::string& filename, int num_transactions);

#endif
