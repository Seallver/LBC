#include <windows.h>
#include <bcrypt.h>
#include<iostream>

#pragma comment(lib, "bcrypt.lib")

bool generate_random_bytes(uint8_t* buffer, size_t length);

int generate_random_number(int N);