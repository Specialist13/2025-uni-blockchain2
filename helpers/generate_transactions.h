#ifndef GENERATE_TRANSACTIONS_H
#define GENERATE_TRANSACTIONS_H

#include <string>
#include <fstream>
#include <random>
#include <vector>
#include "../blockchain/hash_function.h"

void generate_transactions(const std::string& filename, int num_transactions);

#endif