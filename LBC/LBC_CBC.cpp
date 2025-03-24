#include"LBC_CBC.h"


LBC_CBC::LBC_CBC() {
	block_num = 0;
	last_block = 0;
	for (size_t i = 0;i < BLOCK_SIZE;i++)
		IV[i] = 0;
}

LBC_CBC::LBC_CBC(vector<uint32_t> key_, array<uint32_t, BLOCK_SIZE> IV_) {
	key.resize(key_.size());
	for (size_t i = 0;i < key_.size();i++)
		key[i] = key_[i];
	for (size_t i = 0;i < BLOCK_SIZE;i++)
		IV[i] = IV_[i];
}

void LBC_CBC::encrypt() {
	int key_bits = key.size() * 32;
	LBC myLBC(key, key_bits);

	array<uint32_t, BLOCK_SIZE> m_pointer;//���ļĴ���
	array<uint32_t, BLOCK_SIZE> c_pointer;//���ļĴ���

	//��0�����Ľ��ΪIV
	for (size_t i = 0;i < BLOCK_SIZE;i++)
		c_pointer[i] = IV[i];

	//�̿鴦���߼�
	if (last_block == 0) {	//ǡ�÷��飬����Ҫ����͵��
		for (size_t t = 0;t < block_num;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				m_pointer[i] = plaintext[t * 16 + i] ^ c_pointer[i];
			c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				ciphertext.push_back(c_pointer[i]);
		}
	}
	else if (block_num == 1) {	//ֻ��һ���̷��飬�޷�����͵�ݣ�ֻ�ܽ�����䣬����PKCS#7���ֽ����
		uint8_t padding_byte_num = 64 - last_block * 4;
		uint32_t padding = (static_cast<uint32_t>(padding_byte_num) << 24) |
			(static_cast<uint32_t>(padding_byte_num) << 16) |
			(static_cast<uint32_t>(padding_byte_num) << 8) |
			(static_cast<uint32_t>(padding_byte_num));
		for (size_t i = 0;i < plaintext.size();i++)
			m_pointer[i] = plaintext[i] ^ c_pointer[i];
		for (size_t i = plaintext.size();i < BLOCK_SIZE;i++)
			m_pointer[i] = padding ^ c_pointer[i];
		c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			ciphertext.push_back(c_pointer[i]);
	}
	else {	//�ж�������Ҵ��ڶ̷��飬��������͵��
		size_t t = 0;
		for (t;t < block_num - 2;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				m_pointer[i] = plaintext[t * 16 + i] ^ c_pointer[i];
			c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				ciphertext.push_back(c_pointer[i]);
		}

		array<uint32_t, BLOCK_SIZE> C_n_pointer; //C_n�Ĵ���
		for (size_t i = 0;i < BLOCK_SIZE;i++)
			m_pointer[i] = plaintext[t * 16 + i] ^ c_pointer[i];
		C_n_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		t++;

		//�����һ�������0���ܵõ�C_n-1
		for (size_t i = 0;i < last_block;i++)
			m_pointer[i] = plaintext[t * 16 + i] ^ C_n_pointer[i];
		for (size_t i = last_block;i < BLOCK_SIZE;i++)
			m_pointer[i] = C_n_pointer[i]; //0 ^ C_n_pointer[i]
		c_pointer = myLBC.lbc_ret_ciphertext(m_pointer);
		//�ȴ�C_n-1
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			ciphertext.push_back(c_pointer[i]);
		//����C_n
		for (size_t i = 0;i < last_block;i++)
			ciphertext.push_back(C_n_pointer[i]);
	}
}

void LBC_CBC::decrypt() {
	int key_bits = key.size() * 32;
	LBC myLBC(key, key_bits);

	array<uint32_t, BLOCK_SIZE> c_pointer;//���ļĴ���
	array<uint32_t, BLOCK_SIZE> pre_c_pointer;//��һ�����ļĴ���
	array<uint32_t, BLOCK_SIZE> mid_pointer;//�м�״̬�Ĵ���

	for (size_t i = 0;i < BLOCK_SIZE;i++)
		pre_c_pointer[i] = IV[i];

	//�̿鴦���߼�
	if (last_block == 0) {	//ǡ�÷��飬����Ҫ����͵��
		for (size_t t = 0;t < block_num;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				c_pointer[i] = ciphertext[t * 16 + i];
			mid_pointer = myLBC.lbc_ret_plaintext(c_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				plaintext.push_back(mid_pointer[i] ^ pre_c_pointer[i]);
			pre_c_pointer = c_pointer;
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
	else if (block_num == 1) {	//�������ֻ��һ���̷���
		cout << "The ciphertext format is incorrect" << endl;
	}
	else {	//�ж�������Ҵ��ڶ̷��飬��������͵��
		size_t t = 0;
		for (t;t < block_num - 2;t++) {
			for (size_t i = 0;i < BLOCK_SIZE;i++)
				c_pointer[i] = ciphertext[t * 16 + i];
			mid_pointer = myLBC.lbc_ret_plaintext(c_pointer);
			for (size_t i = 0;i < BLOCK_SIZE; i++)
				plaintext.push_back(mid_pointer[i] ^ pre_c_pointer[i]);
			pre_c_pointer = c_pointer;
		}

		//����C_n-1�Ľ���
		for (size_t i = 0;i < BLOCK_SIZE;i++)
			c_pointer[i] = ciphertext[t * 16 + i];
		t++;
		mid_pointer = myLBC.lbc_ret_plaintext(c_pointer);
		//��Cn�����0���õ�Pn��C'
		vector<uint32_t> Pn_pointer;
		for (size_t i = 0;i < last_block;i++)
			Pn_pointer.push_back(mid_pointer[i] ^ ciphertext[t * 16 + i]);

		//��ϵõ�����͵�ݺ������
		for (size_t i = 0;i < last_block;i++)
			c_pointer[i] = ciphertext[t * 16 + i];
		for (size_t i = last_block;i < BLOCK_SIZE;i++)
			c_pointer[i] = mid_pointer[i];

		//����C_n�Ľ���
		mid_pointer = myLBC.lbc_ret_plaintext(c_pointer);
		for (size_t i = 0;i < BLOCK_SIZE; i++)
			plaintext.push_back(mid_pointer[i] ^ pre_c_pointer[i]);

		//�������Pn
		for (size_t i = 0;i < Pn_pointer.size(); i++)
			plaintext.push_back(Pn_pointer[i]);
	}
}


vector<uint32_t> LBC_CBC::ret_ciphertext(vector<uint32_t> plaintext_) {
	for (size_t i = 0;i < plaintext_.size();i++)
		plaintext.push_back(plaintext_[i]);
	last_block = plaintext.size() % BLOCK_SIZE;
	block_num = last_block == 0 ? plaintext.size() / BLOCK_SIZE : plaintext.size() / BLOCK_SIZE + 1;
	encrypt();
	return ciphertext;
}

vector<uint32_t> LBC_CBC::ret_plaintext(vector<uint32_t> ciphertext_) {
	for (size_t i = 0;i < ciphertext_.size();i++)
		ciphertext.push_back(ciphertext_[i]);
	last_block = ciphertext.size() % BLOCK_SIZE;
	block_num = last_block == 0 ? ciphertext.size() / BLOCK_SIZE : ciphertext.size() / BLOCK_SIZE + 1;
	decrypt();
	return plaintext;
}