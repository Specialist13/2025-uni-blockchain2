all:
	 g++ main.cpp helpers/user_generation.cpp helpers/transaction_generation.cpp helpers/genesis_block_creation.cpp \
	     blockchain/transaction.cpp blockchain/hash_function.cpp blockchain/block.cpp \
	     -o main --std=c++17
