#pragma once
#include"LBC.h"

#define BLOCK_SIZE 16

class LBC_CBC {
private:
	vector<uint32_t> plaintext;
	vector<uint32_t> ciphertext;
	vector<uint32_t> key;
	uint32_t block_num;
	uint32_t last_block;
	array<uint32_t, BLOCK_SIZE> IV;
public:
	LBC_CBC();

	LBC_CBC(vector<uint32_t> key_, array<uint32_t, BLOCK_SIZE> IV_);
	

	void encrypt();

	void decrypt();


	vector<uint32_t> ret_ciphertext(vector<uint32_t> plaintext_);

	vector<uint32_t> ret_plaintext(vector<uint32_t> ciphertext_);

};


