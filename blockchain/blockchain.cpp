#include "blockchain.h"
size_t Blockchain::getBlockCount() const { return blocks.size(); }

const Block& Blockchain::getBlock(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= blocks.size()) {
        throw std::out_of_range("Block index out of range");
    }
    return blocks[static_cast<size_t>(index)];
}

const Block& Blockchain::getLatestBlock() const {
    if (blocks.empty()) {
        throw std::runtime_error("Blockchain is empty");
    }
    return blocks.back();
}

std::string Blockchain::computeBlockHash(const Block& block) {
    return block.computePreviousBlockHash();
}

bool Blockchain::isChained(const Block& prev, const Block& next) {
    const std::string prevHash = computeBlockHash(prev);
    return next.getPreviousBlockHash() == prevHash;
}

bool Blockchain::addBlock(const Block& block) {
    if (!blocks.empty()) {
        const Block& prev = blocks.back();
        if (!Blockchain::isChained(prev, block)) {
            return false;
        }
    }
    blocks.push_back(block);
    return true;
}

#include <stdexcept>
