#include"LBC_ECB.h"


LBC_ECB::LBC_ECB() {
	block_num = 0;
	last_block = 0;
}

LBC_ECB::LBC_ECB(vector<uint32_t> key_) {
	block_num = 0;
	last_block = 0;
	key.resize(key_.size());
	for (size_t i = 0;i < key_.size();i++)
		key[i] = key_[i];
}

void LBC_ECB::encrypt() {
	int key_bits = key.size() * 32;
	LBC myLBC(key, key_bits);

	array<uint32_t, BLOCK_SIZE> m_pointer;//明文寄存器
	array<uint32_t, BLOCK_SIZE> c_pointer;//密文寄存器


	//短块处理逻辑
	if (last_block == 0) {	//恰好分组，不需要填充或偷垒
		for (size_t t = 0;t < block_num;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				m_pointer[i] = plaintext[t * 16 + i];
			c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				ciphertext.push_back(c_pointer[i]);
		}
	}
	else if (block_num == 1) {	//只有一个短分组，无法密文偷垒，只能进行填充，采用PKCS#7按字节填充
		uint8_t padding_byte_num = 64 - last_block * 4;
		uint32_t padding = (static_cast<uint32_t>(padding_byte_num) << 24) |
			(static_cast<uint32_t>(padding_byte_num) << 16) |
			(static_cast<uint32_t>(padding_byte_num) << 8) |
			(static_cast<uint32_t>(padding_byte_num));
		for (size_t i = 0;i < plaintext.size();i++)
			m_pointer[i] = plaintext[i];
		for (size_t i = plaintext.size();i < BLOCK_SIZE;i++)
			m_pointer[i] = padding;
		c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			ciphertext.push_back(c_pointer[i]);
	}
	else {	//有多个分组且存在短分组，采用密文偷垒
		size_t t = 0;
		for (t;t < block_num - 2;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				m_pointer[i] = plaintext[t * 16 + i];
			c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				ciphertext.push_back(c_pointer[i]);
		}

		array<uint32_t, BLOCK_SIZE> C_n_pointer; //C_n寄存器
		for (size_t i = 0;i < BLOCK_SIZE;i++)
			m_pointer[i] = plaintext[t * 16 + i];
		C_n_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		t++;

		//给最后一组填充上0加密得到C_n-1
		for (size_t i = 0;i < last_block;i++)
			m_pointer[i] = plaintext[t * 16 + i];
		for (size_t i = last_block;i < BLOCK_SIZE;i++)
			m_pointer[i] = C_n_pointer[i]; //0 ^ C_n_pointer[i]
		c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		//先存C_n-1
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			ciphertext.push_back(c_pointer[i]);
		//最后存C_n
		for (size_t i = 0;i < last_block;i++)
			ciphertext.push_back(C_n_pointer[i]);
	}
}

void LBC_ECB::decrypt() {
	int key_bits = key.size() * 32;
	LBC myLBC(key, key_bits);

	array<uint32_t, BLOCK_SIZE> c_pointer;//密文寄存器
	array<uint32_t, BLOCK_SIZE> m_pointer;//明文寄存器


	//短块处理逻辑
	if (last_block == 0) {	//恰好分组，不需要填充或偷垒
		for (size_t t = 0;t < block_num;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				c_pointer[i] = ciphertext[t * 16 + i];
			m_pointer = myLBC.lbc_ret_plaintext(c_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				plaintext.push_back(m_pointer[i]);
		}
		int last_byte = plaintext[BLOCK_SIZE - 1] & 0xff;
		bool flag = 1;
		for (size_t i = 0;i < last_byte;i++) {
			if (((plaintext[BLOCK_SIZE - i / 4 - 1] >> ((i % 4) * 8)) & 0xff) != last_byte) {
				flag = 0;
				break;
			}
		}
		if (flag) {
			plaintext.resize(BLOCK_SIZE - last_byte / 4);
		}
	}
	else if (block_num == 1) {	//不会出现只有一个短分组
		cout << "The ciphertext format is incorrect" << endl;
	}
	else {	//有多个分组且存在短分组，采用密文偷垒
		size_t t = 0;
		for (t;t < block_num - 2;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				c_pointer[i] = ciphertext[t * 16 + i];
			m_pointer = myLBC.lbc_ret_plaintext(c_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				plaintext.push_back(m_pointer[i]);
		}

		//处理C_n-1的解密
		for (size_t i = 0;i < BLOCK_SIZE;i++)
			c_pointer[i] = ciphertext[t * 16 + i];
		t++;
		m_pointer = myLBC.lbc_ret_plaintext(c_pointer);

		//给Cn填充上0得到Pn和C'
		vector<uint32_t> Pn_pointer;
		for (size_t i = 0;i < last_block;i++)
			Pn_pointer.push_back(m_pointer[i]);

		//组合得到密文偷垒后的密文
		for (size_t i = 0;i < last_block;i++)
			c_pointer[i] = ciphertext[t * 16 + i];
		for (size_t i = last_block;i < BLOCK_SIZE;i++)
			c_pointer[i] = m_pointer[i];

		//处理C_n的解密
		m_pointer = myLBC.lbc_ret_plaintext(c_pointer);
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			plaintext.push_back(m_pointer[i]);

		//最后填入Pn
		for (size_t i = 0;i < Pn_pointer.size(); i++)
			plaintext.push_back(Pn_pointer[i]);
	}
}


vector<uint32_t> LBC_ECB::ret_ciphertext(vector<uint32_t> plaintext_) {
	for (size_t i = 0;i < plaintext_.size();i++)
		plaintext.push_back(plaintext_[i]);
	last_block = plaintext.size() % BLOCK_SIZE;
	block_num = last_block == 0 ? plaintext.size() / BLOCK_SIZE : plaintext.size() / BLOCK_SIZE + 1;
	encrypt();
	return ciphertext;
}

vector<uint32_t> LBC_ECB::ret_plaintext(vector<uint32_t> ciphertext_) {
	for (size_t i = 0;i < ciphertext_.size();i++)
		ciphertext.push_back(ciphertext_[i]);
	last_block = ciphertext.size() % BLOCK_SIZE;
	block_num = last_block == 0 ? ciphertext.size() / BLOCK_SIZE : ciphertext.size() / BLOCK_SIZE + 1;
	decrypt();
	return plaintext;
}