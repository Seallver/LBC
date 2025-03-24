#pragma once
#include<iostream>
#include<array>
using namespace std;



class SM4 {
private:
	array<uint32_t,4> key;

	array<uint32_t, 32>key_list;

	int Round;

	array<uint32_t, 4> ctx;

	uint32_t KeyGen();

	uint32_t S(uint32_t S_in);

	uint32_t shift_left(uint32_t value, int shift);

	uint32_t L(uint32_t S_out);

	uint32_t L_keygen(uint32_t S_out);

	uint32_t T(uint32_t T_in);

	uint32_t T_keygen(uint32_t T_in);

	void Shift_Block(uint32_t T_out);

	uint32_t Add_Key(uint32_t RoundKey);

	void R();

public:

	SM4();

	SM4(array<uint32_t, 4> key_, array<uint32_t, 4> ctx_);

	array<uint32_t, 4> ret_ciphertext();

	void reset(array<uint32_t, 4> key_, array<uint32_t, 4> ctx_);

	array<uint32_t, 4> ret_plaintext();
};