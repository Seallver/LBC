#pragma once
#include"SM3.h"
#include"SM4.h"
#include<vector>
#include<cstdint>


class LBC {
private:
	SM3 mySM3;

	SM4 mySM4;

	array<array<uint32_t, 4>, 4> key;

	array<uint32_t, 16> ctx;

public:
	LBC();

	LBC(vector<uint32_t> key_list, int bits);

	void encrypt_aRound(array<uint32_t, 4> key_SM3, array<uint32_t, 4> key_SM4_1, array<uint32_t, 4> key_SM4_2);

	void decrypt_aRound(array<uint32_t, 4> key_SM3, array<uint32_t, 4> key_SM4_1, array<uint32_t, 4> key_SM4_2);

	array<uint32_t, 16> lbc_ret_ciphertext(array<uint32_t, 16> plaintext);

	array<uint32_t, 16> lbc_ret_plaintext(array<uint32_t, 16> plaintext);

};