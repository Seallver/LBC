#pragma once
#include<iostream>
#include<array>
using namespace std;

class SM3 {
private:	
	array<uint32_t, 16> Mess;

	array<uint32_t, 8> Hash;

	uint32_t shift_left(uint32_t value, int shift);

	uint32_t P(uint32_t value);

	uint32_t FF(uint32_t value1, uint32_t value2, uint32_t value3, size_t index);

	uint32_t GG(uint32_t value1, uint32_t value2, uint32_t value3, size_t index);

	uint32_t P_(uint32_t value);

	void CF();

public:
	SM3();

	SM3(array<uint32_t, 12> mess);

	array<uint32_t, 8> retHash();

	void reset(array<uint32_t, 12> Mess_);
};

