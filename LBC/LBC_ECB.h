#pragma once
#include"LBC.h"

#define BLOCK_SIZE 16

class LBC_ECB {
private:
	vector<uint32_t> plaintext;
	vector<uint32_t> ciphertext;
	vector<uint32_t> key;
	uint32_t block_num;
	uint32_t last_block;
public:
	LBC_ECB();

	LBC_ECB(vector<uint32_t> key_);


	void encrypt();

	void decrypt();


	vector<uint32_t> ret_ciphertext(vector<uint32_t> plaintext_);

	vector<uint32_t> ret_plaintext(vector<uint32_t> ciphertext_);

};


