#ifndef BLOCK_H
#define BLOCK_H

#include <string>

class Block {
private:
    std::string previous_block_hash;
    std::string merkle_root_hash;
    int version;
    int difficulty;
    long long timestamp;
    long long nonce;
public:
    Block(std::string previous_block_hash_, std::string merkle_root_hash_, int version_, int difficulty_, long long timestamp_, long long nonce_);

    std::string getPreviousBlockHash() const;
    std::string getMerkleRootHash() const;
    int getVersion() const;
    int getDifficulty() const;
    long long getTimestamp() const;
    long long getNonce() const;

    std::string toString() const;
};

#endif