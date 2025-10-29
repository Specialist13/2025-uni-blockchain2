#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include "block.h"

// The Blockchain class owns and validates the in-memory chain only.
class Blockchain {
private:
    std::vector<Block> blocks;

public:
    Blockchain() = default;

    bool addBlock(const Block& block);
    const Block& getBlock(int index) const;
    const Block& getLatestBlock() const;
    size_t getBlockCount() const;
    
    static std::string computeBlockHash(const Block& block);
    static bool isChained(const Block& prev, const Block& next);
    const std::vector<Block>& getBlocks() const { return blocks; }
    void setBlocks(std::vector<Block> newBlocks) { blocks = std::move(newBlocks); }
};

#endif
