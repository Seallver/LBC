#include"LBC.h"

#define KEY_BITS 128
#define KEY_NUM 4
#define ROUND_NUM 4
#define BLOCK_NUM 4


LBC::LBC() {
	for (size_t i = 0;i < 16;i++)
		ctx[i] = 0;
	for (size_t i = 0;i < KEY_NUM;i++)
		for (size_t j = 0;j < BLOCK_NUM;j++)
			key[i][j] = 0;
}

LBC::LBC(vector<uint32_t> key_list, int bits) {
	for (size_t i = 0;i < 16;i++)
		ctx[i] = 0;

	size_t len = key_list.size();
	if (len != bits / 32) {
		cout << "key or bits error\n";
		return;
	}

	switch (bits) {
	case 128: {
		for (size_t i = 0;i < BLOCK_NUM;i++)
			for (size_t j = 0;j < KEY_NUM;j++)
				key[j][i] = key_list[i];
		break;
	}
	case 256: {
		for (size_t i = 0;i < BLOCK_NUM;i++) {
			key[0][i] = key_list[i];
			key[2][i] = key_list[i];
			key[1][i] = key_list[BLOCK_NUM + i];
			key[3][i] = key_list[BLOCK_NUM + i];
		}
		break;
	}
	case 384: {
		for (size_t i = 0;i < BLOCK_NUM;i++) {
			key[0][i] = key_list[i];
			key[1][i] = key_list[BLOCK_NUM + i];
			key[2][i] = key_list[2 * BLOCK_NUM + i];
			key[3][i] = key[0][i] ^ key[1][i] ^ key[2][i];
		}
		break;
	}
	case 512: {
		for (size_t i = 0;i < BLOCK_NUM;i++)
			for (size_t j = 0;j < KEY_NUM;j++)
				key[j][i] = key_list[j * BLOCK_NUM + i];
		break;
	}
	default:
		cout << "key bits error: " << dec << bits << endl;
		return;
	}
}

void LBC::encrypt_aRound(array<uint32_t, 4> key_SM3, array<uint32_t, 4> key_SM4_1, array<uint32_t, 4> key_SM4_2) {
	array<uint32_t, 4> B = { ctx[8],ctx[9],ctx[10],ctx[11] },
		C = { ctx[12],ctx[13],ctx[14],ctx[15] };
	array<uint32_t, 8> A = { ctx[0],ctx[1],ctx[2],ctx[3],ctx[4],ctx[5],ctx[6],ctx[7] };

	array<uint32_t, 12> SM3_in;
	for (size_t i = 0;i < 4;i++) {
		SM3_in[i] = B[i];
		SM3_in[i + 4] = key_SM3[i];
		SM3_in[i + 8] = C[i];
	}

	mySM3.reset(SM3_in);

	array<uint32_t, 8> SM3_out = mySM3.retHash();

	mySM4.reset(key_SM4_1, C);

	array<uint32_t, 4> SM4_out_1 = mySM4.ret_ciphertext();

	mySM4.reset(key_SM4_2, B);

	array<uint32_t, 4> SM4_out_2 = mySM4.ret_ciphertext();

	for (size_t i = 0;i < 4;i++) {
		ctx[i] = SM4_out_1[i];
		ctx[i + 4] = SM4_out_2[i];
		ctx[i + 8] = A[i] ^ SM3_out[i];
		ctx[i + 12] = A[4 + i] ^ SM3_out[4 + i];
	}
}

array<uint32_t, 16> LBC::lbc_ret_ciphertext(array<uint32_t, 16> plaintext) {
	for (size_t i = 0;i < 16;i++)
		ctx[i] = plaintext[i];

	int round_flag = 0;

	while (round_flag < ROUND_NUM) {
		switch (round_flag) {
		case 0:
			encrypt_aRound(key[2], key[0], key[1]);
			break;
		case 1:
			encrypt_aRound(key[0], key[2], key[3]);
			break;
		case 2:
			encrypt_aRound(key[3], key[1], key[2]);
			break;
		case 3:
			encrypt_aRound(key[1], key[3], key[0]);
			break;
		default:
			break;
		}
		round_flag++;
	}

	return ctx;
}

void LBC::decrypt_aRound(array<uint32_t, 4> key_SM3, array<uint32_t, 4> key_SM4_1, array<uint32_t, 4> key_SM4_2) {
	array<uint32_t, 4> SM4_out_1, SM4_out_2, B, C;

	for (size_t i = 0;i < 4;i++) {
		SM4_out_1[i] = ctx[i];
		SM4_out_2[i] = ctx[4 + i];
	}

	mySM4.reset(key_SM4_1, SM4_out_1);

	C = mySM4.ret_plaintext();

	mySM4.reset(key_SM4_2, SM4_out_2);

	B = mySM4.ret_plaintext();

	array<uint32_t, 8> SM3_out;

	array<uint32_t, 12> SM3_in;

	for (size_t i = 0;i < 4;i++) {
		SM3_in[i] = B[i];
		SM3_in[4 + i] = key_SM3[i];
		SM3_in[8 + i] = C[i];
	}

	mySM3.reset(SM3_in);

	SM3_out = mySM3.retHash();

	array<uint32_t, 8> A;

	for (size_t i = 0;i < 8;i++)
		A[i] = SM3_out[i] ^ ctx[i + 8];

	for (size_t i = 0;i < 4;i++) {
		ctx[i] = A[i];
		ctx[4 + i] = A[4 + i];
		ctx[8 + i] = B[i];
		ctx[12 + i] = C[i];
	}
}

array<uint32_t, 16> LBC::lbc_ret_plaintext(array<uint32_t, 16> ciphertext) {
	for (size_t i = 0;i < 16;i++)
		ctx[i] = ciphertext[i];

	int round_flag = ROUND_NUM;

	while (round_flag > 0) {
		switch (round_flag) {
		case 1:
			decrypt_aRound(key[2], key[0], key[1]);
			break;
		case 2:
			decrypt_aRound(key[0], key[2], key[3]);
			break;
		case 3:
			decrypt_aRound(key[3], key[1], key[2]);
			break;
		case 4:
			decrypt_aRound(key[1], key[3], key[0]);
			break;
		default:
			break;
		}
		round_flag--;
	}

	return ctx;
}