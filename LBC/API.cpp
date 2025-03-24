#include"API.h"
#define HEAD_OFFSET 17


void api_enc(vector<uint32_t>& ciphertext, string & key, vector<uint32_t> plaintext, int key_bits,int enc_type,string pubkey) {
	vector<uint32_t> key_vec;


	if (key_bits % 128 != 0) {
		cerr << "key bits error\n";
	}

	key_vec.resize(key_bits / 32);

	if (!generate_random_bytes(reinterpret_cast<uint8_t*>(key_vec.data()), key_vec.size() * sizeof(uint32_t))) {
		cerr << "Failed to generate random key." << endl;
	}


	array<uint32_t, 16> iv;
	if (!generate_random_bytes(reinterpret_cast<uint8_t*>(iv.data()), iv.size() * sizeof(uint32_t))) {
		cerr << "Failed to generate random iv." << endl;
	}

	switch (enc_type)
	{
	case 0: {
		LBC_CBC myLbc_cbc(key_vec, iv);
		ciphertext = myLbc_cbc.ret_ciphertext(plaintext);
		break;
	}
	case 1: {//ECB模式
		LBC_ECB myLbc_ecb(key_vec);
		ciphertext = myLbc_ecb.ret_ciphertext(plaintext);
		break;
	}
	default:
		break;
	}

	//处理密文头部
	//固定字符，版本号
	uint8_t Fixed_charac = 0x01;
	//工作模式标识
	uint8_t enc_type_flag = enc_type;

	uint32_t head = (static_cast<uint32_t>(Fixed_charac) << 8) | enc_type_flag;

	for (size_t i = 16;i > 0;i--) {
		ciphertext.insert(ciphertext.begin(), iv[i-1]);
	}

	ciphertext.insert(ciphertext.begin(), head);

	string str_key = "!";
	for (size_t i = 0;i < key_vec.size();i++) {
		str_key += to_string(key_vec[i]);
		str_key += "!";
	}

	NTRU myNTRU;

	key = myNTRU.encrypt(str_key, pubkey);

}


void api_dec(vector<uint32_t> ciphertext, string key, vector<uint32_t> & plaintext, string prikey) {

	NTRU myNTRU;

	string str_key = myNTRU.decrypt(key, prikey);

	size_t start, end;

	vector<uint32_t> block_key;

	for (size_t i = 0;i < str_key.length();i++) {
		if (str_key[i] == '!') {
			start = i + 1;
			for (size_t j = start;j < str_key.length();j++) {
				if (str_key[j] == '!') {
					end = j;
					break;
				}
			}
			string substr;
			substr.resize(end - start);
			for (size_t t = start;t < end;t++)
				substr[t-start] = str_key[t];
			block_key.push_back(static_cast<uint32_t>(std::stoul(substr)));
			if (end == str_key.length()-1)
				break;
		}
	}
	
	array<uint32_t, 16> iv;
	for (size_t i = 0;i < 16;i++)
		iv[i] = ciphertext[i + 1];

	int enc_type = ciphertext[0] & 0xFF;

	vector<uint32_t> C(ciphertext.begin() + HEAD_OFFSET, ciphertext.end());

	switch (enc_type)
	{
	case 0: {
		LBC_CBC myLbc_cbc(block_key, iv);
		plaintext = myLbc_cbc.ret_plaintext(C);
		break;
	}
	case 1: {//ECB模式
		LBC_ECB myLbc_ecb(block_key);
		plaintext = myLbc_ecb.ret_plaintext(C);
		break;
	}
	default:
		break;
	}
}


void api_key_gen(string& pubkey, string& prikey) {
	char base64_[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+=";
	NTRU ntru;
	ntru.generate_keys();
	vector<int> public_key = ntru.get_public_key();
	pair<vector<int>, vector<int>> private_key = ntru.get_private_key();
	ostringstream ospubkey;
	ostringstream osprikey;
	for (int i : public_key) {
		ospubkey << base64_[i + 31];
	}
	pubkey = ospubkey.str();
	for (int i : private_key.first) {
		if (i == 1) {
			osprikey << i;
		}
		else if (i == 0) {
			osprikey << i;
		}
		else if (i == -1) {
			osprikey << 2;
		}
	}
	osprikey << " ";
	for (int i : private_key.second) {
		if (i == 1) {
			osprikey << i;
		}
		else if (i == 0) {
			osprikey << i;
		}
		else if (i == -1) {
			osprikey << 2;
		}
	}
	prikey = osprikey.str();
}