#include "store.h"
#include <iostream>
#include <sstream>

namespace Keys {
    Store* Store::instance = nullptr;

    bool Store::loadFromUsersFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open users file: " << filename << std::endl;
            return false;
        }

        key_map.clear();
        std::string first_name, last_name, public_key, private_key;
        int balance;
        size_t loaded_count = 0;

        while (file >> first_name >> last_name >> public_key >> private_key >> balance) {
            key_map[public_key] = private_key;
            loaded_count++;
        }

        file.close();
        std::cout << "Loaded " << loaded_count << " key pairs from " << filename << std::endl;
        return true;
    }

    std::string Store::getPrivateKey(const std::string& public_key) const {
        auto it = key_map.find(public_key);
        if (it != key_map.end()) {
            return it->second;
        }
        return "";
    }

    bool Store::hasPublicKey(const std::string& public_key) const {
        return key_map.find(public_key) != key_map.end();
    }

    size_t Store::size() const {
        return key_map.size();
    }

    void Store::clear() {
        key_map.clear();
    }
}

