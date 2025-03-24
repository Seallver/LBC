#include "API.h"
#include"LBC.h"
#include"LBC_CBC.h"
#include"myRandom.h"
#include"SM4.h"
#include"NTRU.h"
#include <windows.h>
#include <bcrypt.h>
#include<string>
#include<sstream>
char base64_[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+=";

#define KEY_SIZE 512
#define M_SIZE 33
#define ENC_MODE 0

//测试用例
int main() {
	

	int test_id = 1;

	while (test_id!=-1) {
		cout << "* ================== *\n";
		cout << "||Test              ||" << endl;
		cout << "||0. NTRU           ||" << endl;
		cout << "||1. SM4            ||" << endl;
		cout << "||2. LBC            ||" << endl;
		cout << "||3. LBC-CBC        ||" << endl;
		cout << "||4. LBC-ECB        ||" << endl;
		cout << "||5. API            ||" << endl;
		cout << "||-1. exit          ||" << endl;
		cout << "* ================== *\n";
		cout << "* input test_id: ";
		int tmp;
		cin >> tmp;
		test_id = tmp;


		switch (test_id)
		{
		case 0: {
			NTRU ntru;
			ntru.generate_keys();
			vector<int> public_key = ntru.get_public_key();
			pair<vector<int>, vector<int>> private_key = ntru.get_private_key();

			string pubkey;
			string prikey;
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
			cout << "public key：" << endl;
			cout << pubkey << endl;
			cout << "private key：" << endl;
			cout << prikey << endl;

			string message = "hello world";

			cout << "m:\n";
			cout << message << endl;

			string ciphertext;
			ciphertext = ntru.encrypt(message, pubkey);

			cout << "c：" << endl;
			cout << ciphertext << endl;

			cout << "m = dec(c):\n";
			string plaintext;
			plaintext = ntru.decrypt(ciphertext, prikey);

			cout << plaintext << endl;



			break;
		}
		case 1: {
			//SM4test
			cout << "* sm4_test" << endl;
			array<uint32_t, 4>sm4_key, sm4_m, sm4_c;

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(sm4_key.data()), sm4_key.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random key." << endl;
				return 1;
			}

			cout << "key:\n";
			for (size_t i = 0;i < 4;i++) {
				cout << hex << sm4_key[i] << " \t";
			}
			cout << endl;

			cout << "m:\n";
			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(sm4_m.data()), sm4_m.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random plaintext." << endl;
				return 1;
			}

			for (size_t i = 0;i < 4;i++) {
				cout << hex << sm4_m[i] << " \t";
			}
			cout << endl;

			SM4 mySM4(sm4_key, sm4_m);

			sm4_c = mySM4.ret_ciphertext();

			cout << "c:\n";
			for (size_t i = 0;i < 4;i++) {
				cout << hex << sm4_c[i] << " \t";
			}
			cout << endl;

			mySM4.reset(sm4_key, sm4_c);

			sm4_m = mySM4.ret_plaintext();
			cout << "m=Dec(c):\n";
			for (size_t i = 0;i < 4;i++) {
				cout << hex << sm4_m[i] << " \t";
			}
			cout << endl << endl;
			
			break;
		}
		case 2: {
			//LBCtest
			cout << "* LBC_test" << endl;
			array<uint32_t, 16> plaintext;

			cout << "Randomly gen_mainerated plaintext:\n" << endl;
			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(plaintext.data()), plaintext.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random plaintext." << endl;
				return 1;
			}
			for (size_t i = 0;i < 16;i++) {
				cout << hex << plaintext[i] << " \t";
				if ((i + 1) % 4 == 0)
					cout << endl;
			}
			cout << endl << endl;

			cout << "Randomly gen_mainerated keys:\n" << endl;

			int key_size = KEY_SIZE / 32;

			vector<uint32_t> keys(key_size);
			keys.resize(key_size);

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(keys.data()), keys.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random key." << endl;
				return 1;
			}

			for (size_t i = 0;i < key_size;i++) {
				cout << hex << keys[i] << " \t";
				if ((i + 1) % 4 == 0)
					cout << endl;
			}
			cout << endl;

			LBC myLBC(keys, KEY_SIZE);

			cout << "after encrypt, ciphertext:\n" << endl;
			array<uint32_t, 16> res = myLBC.lbc_ret_ciphertext(plaintext);
			for (size_t i = 0;i < 16;i++) {
				cout << hex << res[i] << " \t";
				if ((i + 1) % 4 == 0)
					cout << endl;
			}
			cout << endl << endl;

			cout << "after decrypt, plaintext:\n" << endl;
			res = myLBC.lbc_ret_plaintext(res);
			for (size_t i = 0;i < 16;i++) {
				cout << hex << res[i] << " \t";
				if ((i + 1) % 4 == 0)
					cout << endl;
			}
			cout << endl << endl;
			break;
		}
		case 3: {
			//LBC-CBCtest
			cout << "* LBC-CBC_test" << endl;
			vector<uint32_t> m1, k1, c1;

			array<uint32_t, 16> iv;

			size_t m_size = M_SIZE, k_size = KEY_SIZE / 32;

			m1.resize(m_size);
			k1.resize(k_size);

			cout << "m:\n";

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(m1.data()), m1.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random plaintext." << endl;
				return 1;
			}

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(k1.data()), k1.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random key." << endl;
				return 1;
			}

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(iv.data()), iv.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random IV." << endl;
				return 1;
			}


			for (size_t i = 0;i < m_size;i++) {
				cout << hex << m1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			cout << "k:\n";

			for (size_t i = 0;i < k_size;i++) {
				cout << hex << k1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			LBC_CBC myLbc_cbc(k1, iv);
			c1 = myLbc_cbc.ret_ciphertext(m1);

			cout << "c:\n";

			for (size_t i = 0;i < c1.size();i++) {
				cout << hex << c1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			LBC_CBC myLbc_cbc_dec(k1, iv);
			m1 = myLbc_cbc_dec.ret_plaintext(c1);

			cout << "m = Dec(c):\n";

			for (size_t i = 0;i < m1.size();i++) {
				cout << hex << m1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl << endl;
			break;
		}
		case 4: {
			//LBC-ECBtest
			cout << "* LBC-ECB_test" << endl;
			vector<uint32_t> m1, k1, c1;

			size_t m_size = M_SIZE, k_size = KEY_SIZE / 32;

			m1.resize(m_size);
			k1.resize(k_size);

			cout << "m:\n";

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(m1.data()), m1.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random plaintext." << endl;
				return 1;
			}

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(k1.data()), k1.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random key." << endl;
				return 1;
			}


			for (size_t i = 0;i < m_size;i++) {
				cout << hex << m1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			cout << "k:\n";

			for (size_t i = 0;i < k_size;i++) {
				cout << hex << k1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			LBC_ECB myLbc_ecb(k1);
			c1 = myLbc_ecb.ret_ciphertext(m1);

			cout << "c:\n";

			for (size_t i = 0;i < c1.size();i++) {
				cout << hex << c1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			LBC_ECB myLbc_ecb_dec(k1);
			m1 = myLbc_ecb_dec.ret_plaintext(c1);

			cout << "m = Dec(c):\n";

			for (size_t i = 0;i < m1.size();i++) {
				cout << hex << m1[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl << endl;
			break;
		}
		case 5: {
			//api_test
			cout << "* api_test" << endl;
			NTRU ntru;
			ntru.generate_keys();
			vector<int> public_key = ntru.get_public_key();
			pair<vector<int>, vector<int>> private_key = ntru.get_private_key();

			string pubkey;
			string prikey;
			
			api_key_gen(pubkey, prikey);

			vector<uint32_t>m, c;

			array<uint32_t, 16> IV;

			int m_size = M_SIZE, k_size = KEY_SIZE;

			m.resize(m_size);

			cout << "m:\n";

			if (!generate_random_bytes(reinterpret_cast<uint8_t*>(m.data()), m.size() * sizeof(uint32_t))) {
				cerr << "Failed to generate random plaintext." << endl;
				return 1;
			}

			for (size_t i = 0;i < m_size;i++) {
				cout << hex << m[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;
			
			string k = "";
			
			//调用api
			api_enc(c, k, m, k_size, ENC_MODE, pubkey);


			//生成密钥k
			cout << "k:\n" << k << endl;

			//生成密文c
			cout << "c:\n";

			for (size_t i = 0;i < c.size();i++) {
				cout << hex << c[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl;

			vector<uint32_t>dec_m;

			//调用解密api
			api_dec(c, k, dec_m, prikey);

			//生成明文
			cout << "m = Dec(c):\n";

			for (size_t i = 0;i < dec_m.size();i++) {
				cout << hex << dec_m[i] << " \t";
				if ((i + 1) % 8 == 0)
					cout << endl;
			}
			cout << endl << endl;
			break;
		}
		case -1: {
			cout << "* exit \n";
			break;
		}

		default: {
			cout << "* invalid test id \n";
			cout << endl << endl;
			break;
		}
		}
		
	}






	return 0;
}
