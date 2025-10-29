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
            {
                BlockchainStorage storage("./data/blockchain.json");
                std::vector<Block> blocks = storage.load();
                if (blocks.empty()) {
                    std::cout << "No blocks found." << std::endl;
                    break;
                }

                std::cout << "Total blocks: " << blocks.size() << std::endl;
                for (size_t i = 0; i < blocks.size(); ++i) {
                    const Block& b = blocks[i];
                    std::string prev = b.getPreviousBlockHash();
                    std::string prevShort = prev.size() >= 8 ? prev.substr(0, 8) : prev;
                    std::cout << "[#" << i << "] prev=" << prevShort
                              << " merkle=" << b.getMerkleRootHash().substr(0, 8)
                              << " ts=" << b.getTimestamp()
                              << " txs=" << b.getTransactions().size() << std::endl;
                }

                std::cout << "Enter block index for details (-1 to return): ";
                int idx; std::cin >> idx;
                if (idx >= 0 && idx < static_cast<int>(blocks.size())) {
                    const Block& b = blocks[static_cast<size_t>(idx)];
                    std::cout << "Previous Hash:  " << b.getPreviousBlockHash() << std::endl;
                    std::cout << "Merkle Root:    " << b.getMerkleRootHash() << std::endl;
                    std::cout << "Version:        " << b.getVersion() << std::endl;
                    std::cout << "Difficulty:     " << b.getDifficulty() << std::endl;
                    std::cout << "Timestamp:      " << b.getTimestamp() << std::endl;
                    std::cout << "Nonce:          " << b.getNonce() << std::endl;
                    std::cout << "Transactions:   " << b.getTransactions().size() << std::endl;

                    const auto& txs = b.getTransactions();
                    for (size_t t = 0; t < txs.size(); ++t) {
                        const Transaction& tx = txs[t];
                        std::cout << "  - tx[#" << t << "] id=" << tx.getTransactionId()
                                  << " inputs=" << tx.getInputs().size()
                                  << " outputs=" << tx.getOutputs().size() << std::endl;
                    }

                    size_t totalTxs = 0;
                    for (const auto& blk : blocks) totalTxs += blk.getTransactions().size();
                    std::cout << "\n[Stats] Total Blocks=" << blocks.size()
                              << ", Total Transactions=" << totalTxs
                              << ", UTXO Set Size=" << UTXOSet::getInstance()->size() << std::endl;
                }
            }
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
            if (g_miner) {
                std::cout << "\n=== Mining Statistics ===" << std::endl;
                g_miner->displayStatistics();
                std::cout << "========================\n" << std::endl;
            }
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
