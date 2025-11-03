#ifndef KEYS_STORE_H
#define KEYS_STORE_H

#include <string>
#include <unordered_map>
#include <fstream>

namespace Keys {
    class Store {
    private:
        static Store* instance;
        std::unordered_map<std::string, std::string> key_map; // public_key -> private_key
        
        Store() {};
        
        Store(const Store&) = delete;
        Store& operator=(const Store&) = delete;

    public:
        static Store* getInstance() {
            if (instance == nullptr) {
                instance = new Store();
            }
            return instance;
        }

        bool loadFromUsersFile(const std::string& filename);

        std::string getPrivateKey(const std::string& public_key) const;

        bool hasPublicKey(const std::string& public_key) const;

        size_t size() const;

        void clear();
    };
}

#endif

