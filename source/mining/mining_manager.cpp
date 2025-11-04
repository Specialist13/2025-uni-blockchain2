#include "mining_manager.h"
#include <ctime>
#include <chrono>
#include <iostream>

MiningManager::MiningManager(const std::string& queueFile,
                             const std::string& blockchainFile,
                             int difficulty_)
    : queue(queueFile), storage(blockchainFile), utxo(UTXOSet::getInstance()), difficulty(difficulty_) {
    queue.loadFromFile();
    chain.setBlocks(storage.load());
}

bool MiningManager::startMining(int txPerBlock, int maxBlocks, std::shared_ptr<std::atomic<int>> winner, int minerId) {
    queue.loadFromFile();
    std::cout << "Starting mining. Pending txs: " << queue.size() << std::endl;
    int minedThisRun = 0;
    // adaptive timeout: start at 5 seconds, double on each timeout event
    double initialTimeout = 5.0;
    double currentTimeout = initialTimeout;
    while ((winner == nullptr || winner->load() == -1) && queue.size() > 0 && minedThisRun < maxBlocks) {
        queue.loadFromFile();
        if (winner && winner->load() != -1) break; // somebody else won
        std::vector<Transaction> txs = selectValidTransactions(txPerBlock);
        if (txs.empty()) {
            std::cout << "No valid transactions available. Stopping." << std::endl;
            return false;
        }
        int res = mineAndCommitBlock(txs, winner, minerId, currentTimeout);
        if (res == 1) {
            // success: reset timeout
            currentTimeout = initialTimeout;
            ++minedThisRun;
        } else if (res == 2) {
            // timeout: no miner found a block within currentTimeout, increase wait window
            currentTimeout *= 2.0;
            std::cout << "No miner found a block within " << (currentTimeout / 2.0) << "s - increasing timeout to " << currentTimeout << "s and retrying." << std::endl;
            // continue without incrementing minedThisRun
            continue;
        } else {
            std::cout << "Failed to mine/commit block or aborted. Stopping." << std::endl;
            return false;
        }
        displayStatistics();
    }
    return true;
}

std::vector<Transaction> MiningManager::selectValidTransactions(int count) {
    int multiplier = 10;
    std::vector<Transaction> candidates = queue.getRandomTransactions(std::max(count * multiplier, count));
    std::vector<Transaction> selected;
    selected.reserve(count);

    for (const Transaction& tx : candidates) {
        if (utxo->validateTransaction(tx)) {
            selected.push_back(tx);
            if (static_cast<int>(selected.size()) == count) break;
        }
    }
    return selected;
}

int MiningManager::mineAndCommitBlock(const std::vector<Transaction>& txs, std::shared_ptr<std::atomic<int>> winner, int minerId, double timeoutSeconds) {
    std::string previousHash = chain.getBlockCount() > 0
        ? Blockchain::computeBlockHash(chain.getLatestBlock())
        : std::string(64, '0');

    int version = 1;
    long long timestamp = static_cast<long long>(std::time(nullptr));
    long long nonce = 0;

    std::string merkle = Block::computeMerkleRoot(txs);
    Block block(previousHash, merkle, version, difficulty, timestamp, nonce, txs);

    std::cout << "Mining block with " << txs.size() << " txs..." << std::endl;
    const auto start = std::chrono::steady_clock::now();

    // Manual mining loop so we can check `winner` and abort early if another miner won.
    std::string target(difficulty, '0');
    while (true) {
        if (winner && winner->load() != -1 && winner->load() != minerId) {
            std::cout << "Aborting mining because miner " << winner->load() << " already won." << std::endl;
            return 0;
        }

        // increment nonce and test
        ++nonce;
        block.setNonce(nonce);
        std::string header = previousHash + merkle + std::to_string(version) + std::to_string(difficulty) + std::to_string(timestamp) + std::to_string(nonce);
        std::string hash = SlaSimHash(header);
        if (Block::hasLeadingZeros(hash, difficulty)) {
            // Try to claim the win if a winner atomic is provided
            if (winner) {
                int expected = -1;
                if (winner->compare_exchange_strong(expected, minerId)) {
                    std::cout << "Miner " << minerId << " claims the win with nonce=" << nonce << ", hash=" << hash << std::endl;
                    break; // proceed to commit
                } else {
                    std::cout << "Miner " << minerId << " found a block but lost the race to miner " << expected << std::endl;
                    return false;
                }
            } else {
                // No winner tracking; proceed (single-threaded miner)
                std::cout << "Mined block with nonce=" << nonce << ", hash=" << hash << std::endl;
                break;
            }
        }
        // Periodic optional check to avoid busy-checking winner too often
        if (nonce % 100000 == 0) {
            if (winner && winner->load() != -1 && winner->load() != minerId) {
                std::cout << "Aborting mining mid-run because another miner won." << std::endl;
                return 0;
            }
            // Check timeout at periodic intervals to avoid expensive time calls every nonce
            const auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - start).count();
            if (elapsed >= timeoutSeconds) {
                std::cout << "Timeout reached (" << elapsed << "s) without finding a block." << std::endl;
                return 2; // timeout
            }
        }
    }

    const auto end = std::chrono::steady_clock::now();
    lastBlockSeconds = std::chrono::duration<double>(end - start).count();
    lastBlockHashes = static_cast<unsigned long long>(block.getNonce());
    lastHashrateHps = (lastBlockSeconds > 0.0) ? (static_cast<double>(lastBlockHashes) / lastBlockSeconds) : 0.0;
    ++totalBlocksMined;
    totalHashesTried += lastBlockHashes;
    // Rolling simple average per-block hashrate approximation
    avgHashrateHps = (totalBlocksMined > 0)
        ? ((avgHashrateHps * (static_cast<double>(totalBlocksMined - 1)) + lastHashrateHps) / static_cast<double>(totalBlocksMined))
        : lastHashrateHps;

    // Process: transactions (update UTXO set)
    for (const Transaction& tx : txs) {
        utxo->processTransaction(tx);
    }

    if (!chain.addBlock(block)) {
        std::cerr << "Block chaining validation failed." << std::endl;
        return 0;
    }

    // Persist: blockchain, UTXO set, and remove mined txs from queue
    storage.save(chain.getBlocks());
    utxo->saveToFile("data/utxo_set.json");
    queue.removeTransactions(txs);
    queue.saveToFile();

    std::cout << "Block committed by miner " << (winner ? std::to_string(winner->load()) : std::string("-")) << ". Chain height: " << chain.getBlockCount() << std::endl;
    return 1;
}

void MiningManager::displayStatistics() const {
    std::cout << "Chain height: " << chain.getBlockCount() << std::endl;
    std::cout << "Blocks mined (session): " << totalBlocksMined << std::endl;
    std::cout << "Last block: hashes=" << lastBlockHashes << ", time=" << lastBlockSeconds << "s, hashrate="
              << lastHashrateHps << " H/s" << std::endl;
    std::cout << "Avg hashrate: " << avgHashrateHps << " H/s, Difficulty: " << difficulty << std::endl;
}
