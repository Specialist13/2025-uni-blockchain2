#ifndef MENU_H
#define MENU_H

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "../helpers/generate_users.h"
#include "../helpers/generate_transactions.h"
#include "../helpers/genesis_block_creation.h"
#include "../source/mining/mining_manager.h"
#include "../source/block/block_storage.h"
#include "../source/utxo/UTXO_set.h"

void displayMenu();
void handleChoice(int choice);
void initializeBlockchain();

#endif
