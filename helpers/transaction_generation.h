#ifndef TRANSACTION_GENERATION_H
#define TRANSACTION_GENERATION_H

#include <string>
#include <fstream>
#include <random>
#include <vector>
#include "../blockchain/hash_function.h"

void generate_transactions(const std::string& filename, int num_transactions);

#endif