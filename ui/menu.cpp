#include "menu.h"

void displayMenu() {
    std::cout << "Hello, welcome to the SlaSimCoin Interface!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    initializeBlockchain();
    int choice = 0;
    while (choice != 4) {
        std::cout << "===== Blockchain Menu =====" << std::endl;
        std::cout << "1. Create New Transaction" << std::endl;
        std::cout << "2. View Blockchain" << std::endl;
        std::cout << "3. View UTXO Set" << std::endl;
        std::cout << "4. Exit" << std::endl;
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
            // Call function to view blockchain
            break;
        case 3:
            std::cout << "Viewing UTXO set..." << std::endl;
            // Call function to view UTXO set
            break;
        case 4:
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
        genesis_block.saveToFile("./data/blockchain.json");
        return;
    }
    
    std::cout << "Blockchain initialized." << std::endl;
}