#include "menu.h"

static int g_difficulty = 3;
static int g_txPerBlock = 100;
static std::unique_ptr<MiningManager> g_miner;

void displayMenu() {
    std::cout << "Hello, welcome to the SlaSimCoin Interface!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    initializeBlockchain();
    int choice;
    while (choice != 7) {
        choice = 0;
        std::cout << "===== Blockchain Menu =====" << std::endl;
        std::cout << "1. Create New Transaction" << std::endl;
        std::cout << "2. View Blockchain" << std::endl;
        std::cout << "3. View UTXO Set" << std::endl;
        std::cout << "4. Start Mining" << std::endl;
        std::cout << "5. Show Mining Stats" << std::endl;
        std::cout << "6. Configure Mining" << std::endl;
        std::cout << "7. Exit" << std::endl;
        std::cout << "===========================" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        handleChoice(choice);
    }
}
void handleChoice(int choice) {
    switch (choice) {
        case 1:
            int num_transactions;
            std::cout << "How many transactions would you like to create? ";
            std::cin >> num_transactions;
            generate_transactions("./data/transaction_queue.json", num_transactions);
            break;
        case 2:
            std::cout << "Viewing blockchain..." << std::endl;
            // TODO: Call function to view blockchain
            break;
        case 3:
            std::cout << "Viewing UTXO set..." << std::endl;
            UTXOSet::getInstance()->outputToConsole();
            break;
        case 4: {
            if (!g_miner) {
                std::cout << "Miner not initialized." << std::endl;
                break;
            }
            std::cout << "Starting mining with txPerBlock=" << g_txPerBlock << ", difficulty=" << g_difficulty << std::endl;
            g_miner->startMining(g_txPerBlock);
            break;
        }
        case 5:
            if (g_miner) g_miner->displayStatistics();
            else std::cout << "Miner not initialized." << std::endl;
            break;
        case 6: {
            std::cout << "Enter difficulty (current: " << g_difficulty << "): ";
            int d; std::cin >> d; if (d > 0) g_difficulty = d;
            std::cout << "Enter tx per block (current: " << g_txPerBlock << "): ";
            int t; std::cin >> t; if (t > 0) g_txPerBlock = t;
            g_miner = std::make_unique<MiningManager>("./data/transaction_queue.json", "./data/blockchain.json", g_difficulty);
            std::cout << "Mining configuration updated." << std::endl;
            break;
        }
        case 7:
            std::cout << "Exiting menu." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
    }
}

void initializeBlockchain() {
    std::ifstream fin("./data/blockchain.json");
    if (!fin) {
        std::cout << "This is a fresh start. Creating genesis block..." << std::endl;
        generate_users("./data/users.txt", 1000);
        Block genesis_block = createGenesisBlock();
        std::cout << "Genesis block created." << std::endl;
        UTXOSet::getInstance()->saveToFile("./data/utxo_set.json");

        BlockchainStorage storage("./data/blockchain.json");
        std::vector<Block> chainBlocks;
        chainBlocks.push_back(genesis_block);
        storage.save(chainBlocks);
        return;
    }
    else {
        std::cout << "Loading existing blockchain..." << std::endl;
        UTXOSet::getInstance()->loadFromFile("./data/utxo_set.json");
        std::cout << "Blockchain loaded." << std::endl;
    }
    
    g_miner = std::make_unique<MiningManager>("./data/transaction_queue.json", "./data/blockchain.json", g_difficulty);
    std::cout << "Blockchain initialized." << std::endl;
}
