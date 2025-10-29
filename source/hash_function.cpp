#include "hash_function.h"

std::string SlaSimHash(std::string& input) {
    if (input.empty()) {
        return "";
    }
    std::vector<int> binary;
    std::string output;

    for (char c : input) {
        std::string bin = std::bitset<8>(c).to_string();
        for (char b : bin) {
            binary.push_back(b == '1' ? 1 : 0);
        }
    }

    std::vector<int> numbers;

    int num = 0;

    for (int i = 0; i < binary.size(); i++) {
        num += binary[i] << (7 - i%8);
        if (i % 8 == 7) {
            numbers.push_back(num*(i/8+1));
            num = 0;
        }
    }

    long long total = 0;
    for (int n : numbers) {
        total += n;
    }

    std::vector<int> xor_results = {binary[0], binary[1], binary[2], binary[3]};
    std::vector<int> temp;
    for (int i = 4; i < binary.size(); i++) {
        temp.push_back(xor_results[i % 4] ^ binary[i]);
        if (i % 4 == 3) {
            xor_results = temp;
            temp.clear();
        }
    }
    

    long long xor_numerical_value = 0;
    for (int i = 0; i < xor_results.size(); i++) {
        xor_numerical_value += xor_results[i] << (xor_results.size() - 1 - i);
    }

    long long offset = 1;
    for (int i = 0; i < xor_numerical_value; i++) {
        offset *= total;
        offset %= (1LL << 8);
    }

    for (int i = 0; i < 8; i++) {
        int zero_count = 0;
        int one_count = 0;

        for (int j = input.size() * i; j < input.size() * (i + 1); j++) {
            if (binary[j] == 0) {
                zero_count++;
            } else {
                one_count++;
            }
        }

        long long partial_numerical_hash = 1;
        for (int j = 0; j < one_count + offset + 10; j++) {
            partial_numerical_hash *= (zero_count + total + 10 + partial_numerical_hash % 10);
            partial_numerical_hash % (1LL << 32) == 0 ? partial_numerical_hash -= 1 : partial_numerical_hash;
            partial_numerical_hash %= (1LL << 32);
        }

        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << partial_numerical_hash;
        output += ss.str();
    }

    return output;
}
