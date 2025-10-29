# SlaSimCoin — Educational Blockchain + PoW Miner

SlaSimCoin is a compact, educational blockchain showcasing:
- User and transaction generation
- UTXO-based state management
- Proof-of-Work mining (000… leading zeros)
- Block formation, persistence, and a simple console UI

This project is NOT production-grade nor cryptographically secure. It is designed to demonstrate core concepts clearly.

## Table of Contents
- Overview
- Features
- Project Layout
- Build and Run
- Usage (Console UI)
- Implementation Details
- Code Snippets (quick starts)
- Data Files
- Troubleshooting
- Roadmap / Reach Goals

## Overview
You can generate users and transactions, mine blocks with PoW, update the UTXO set, and explore the chain through a terminal menu. Mining difficulty and transactions per block can be configured at runtime.

## Features
- Transaction queue with JSON persistence
- Block mining with nonce iteration and difficulty target (leading zeros)
- UTXO validation and updates on block confirmation
- Chain validation and chaining (`previous_block_hash`)
- JSON persistence for blockchain and UTXO set
- Simple UI to generate transactions, mine, and inspect blocks

## Project Layout
```
source/
  block/
    block.h|cpp            # Block model, PoW, Merkle root, JSON
    blockchain.h|cpp       # In-memory chain logic and chaining validation
    block_storage.h|cpp    # Persist/Load blockchain (JSON)
  transaction/
    transaction.h|cpp      # Transaction model + (de)serialization
    transaction_queue.h|cpp# Random selection, persistence
  utxo/
    UTXO.h|cpp             # UTXO model
    UTXO_set.h|cpp         # UTXO singleton, validation, processing
  mining/
    mining_manager.h|cpp   # Main mining loop, stats, persistence integration
  hash_function.h|cpp      # SlaSimHash (educational)
helpers/
  generate_users.*         # Generates 1k users
  generate_transactions.*  # Generates transactions JSON
  genesis_block_creation.* # Genesis block creation
ui/
  menu.h|cpp               # Console interface
data/
  users.txt                # Generated users
  utxo_set.json            # UTXO persistence
  blockchain.json          # Blockchain persistence
  transaction_queue.json   # Pending transactions
```

## Build and Run
Requirements: g++ with C++17

```bash
make
./main
```

If `./data` files are missing, the app will generate users and a genesis block on first run.

## Usage (Console UI)
When the app starts you’ll see:
1. Create New Transaction
2. View Blockchain
3. View UTXO Set
4. Start Mining
5. Show Mining Stats
6. Configure Mining (difficulty, tx-per-block)
7. Exit

Typical flow:
1) Choose 6 to set difficulty (e.g., 3) and transactions per block (e.g., 100)
2) Choose 1 to add more transactions (optional)
3) Choose 4 to mine blocks
4) Choose 5 to view mining statistics
5) Choose 2 to explore blocks and transactions

## Implementation Details
### Proof-of-Work
- The block header is serialized and hashed by `SlaSimHash`. A block is valid if its hash string starts with N zeros (difficulty).
- Nonce increments by one until a valid hash is found.

### UTXO Validation
- For each transaction, inputs must reference existing UTXOs, outputs must be positive, and input sum ≥ output sum.
- Signatures are simplified (presence check only) for educational purposes.

### Chain and Storage
- `Blockchain` keeps an in-memory vector of `Block`s and ensures `previous_block_hash` chaining.
- `BlockchainStorage` saves/loads the chain to/from JSON. It supports both an array of blocks and a legacy single-block JSON object.

### Mining Manager
- Picks random pending transactions, filters valid ones using the UTXO set, mines a block, processes transactions (spend inputs, create outputs), appends to chain, and persists everything.
- Tracks mining statistics (last hashrate, average hashrate, total blocks mined in session).

## Code Snippets
### Start mining programmatically
```cpp
#include "source/mining/mining_manager.h"

int main() {
    MiningManager miner("./data/transaction_queue.json", "./data/blockchain.json", /*difficulty=*/3);
    miner.startMining(/*txPerBlock=*/100);
}
```

### Compute and check block hash leading zeros
```cpp
#include "source/block/block.h"

Block b(prevHash, merkle, 1, /*difficulty*/3, timestamp, /*nonce*/0, txs);
bool ok = b.mineBlock(3); // iterates nonce until hash starts with 000
```

### Validate and process a single transaction
```cpp
#include "source/utxo/UTXO_set.h"
#include "source/transaction/transaction.h"

UTXOSet* utxo = UTXOSet::getInstance();
if (utxo->validateTransaction(tx)) {
    utxo->processTransaction(tx); // spends inputs and adds outputs
}
```

### Load entire blockchain and print summary
```cpp
#include "source/block/block_storage.h"

BlockchainStorage storage("./data/blockchain.json");
auto blocks = storage.load();
for (size_t i = 0; i < blocks.size(); ++i) {
    const Block& b = blocks[i];
    std::cout << "[#" << i << "] prev=" << b.getPreviousBlockHash().substr(0,8)
              << " merkle=" << b.getMerkleRootHash().substr(0,8)
              << " ts=" << b.getTimestamp()
              << " txs=" << b.getTransactions().size() << std::endl;
}
```

## Data Files
- `data/users.txt`: generated users with public keys and balances
- `data/transaction_queue.json`: pending transactions for mining
- `data/utxo_set.json`: current UTXO set
- `data/blockchain.json`: blockchain (array of blocks)

## Troubleshooting
- Repeating "Invalid choice": the app is waiting for interactive input. Run `./main` in a terminal and type a number.
- JSON error on load: older files may store a single block object. The loader supports both object and array, but if a file is corrupted, delete `data/blockchain.json` and restart.
- Slow mining: reduce difficulty in the menu (option 6) or lower transactions per block.

## Screenshots
CLI images to showcase how the tool works
