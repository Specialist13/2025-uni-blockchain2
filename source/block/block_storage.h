#ifndef BLOCK_STORAGE_H
#define BLOCK_STORAGE_H

#include <string>
#include <vector>
#include "block.h"

class BlockchainStorage {
private:
    std::string file_path;
public:
    explicit BlockchainStorage(const std::string& path);

    void save(const std::vector<Block>& blocks) const;
    std::vector<Block> load() const; // returns empty if file missing
};

#endif
