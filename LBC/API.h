#include"LBC_CBC.h"
#include"LBC_ECB.h"
#include"NTRU.h"
#include"myRandom.h"

#pragma once

void api_enc(vector<uint32_t>& ciphertext, string & key, vector<uint32_t> plaintext, int key_bits, int enc_type, string pubkey);

void api_dec(vector<uint32_t> ciphertext, string key, vector<uint32_t>& plaintext,string prikey);

void api_key_gen(string& pubkey, string& prikey);