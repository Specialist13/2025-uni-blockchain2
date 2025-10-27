all:
	 g++ main.cpp helpers/generate_users.cpp helpers/generate_transactions.cpp helpers/genesis_block_creation.cpp \
	     blockchain/transaction.cpp blockchain/hash_function.cpp blockchain/block.cpp \
		 blockchain/UTXO.cpp blockchain/UTXO_set.cpp \
	     -o main --std=c++17
