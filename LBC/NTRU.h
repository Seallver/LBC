#pragma once
#include <iostream>
#include <vector>
#include<vector>
#include <chrono>
#include <set>
#include<iostream>
#include <cmath>
#include<bitset>
#include<string>
#include<sstream>
#include"myRandom.h"

using namespace std;

class NTRU {
public:
	// 构造函数，使用默认参数集生成公共参数
	NTRU();
	// 构造函数，使用给定参数集生成公共参数
	NTRU(int N, int p, int q, int Df, int Dg, int Dr);
	// 生成公钥和私钥
	void generate_keys();
	vector<int> get_public_key();
	pair<vector<int>, vector<int>> get_private_key();
	// 加密
	vector<int> encrypt(const vector<int>& plaintext, const vector<int>& public_key);
	string encrypt(const string& plaintext, string public_key);
	// 解密
	vector<int> decrypt(const vector<int>& ciphertext, const pair<vector<int>, vector<int>>& private_key);
	string decrypt(string result, string private_key);

	void print_poly(vector<int> poly);

	vector<int> inverse_qin_p(const vector<int>& a);  //秦九韶大衍求一术求多项式关于p的模逆，通常p=3

private:
	int N;   //多项式系数
	int p;   //系数模量
	int q;   //算数模量
	int Df;     //加密过程多项式的特征参数
	int Dg;
	int Dr;
	vector<int> f;   //私钥
	vector<int> Fp;
	vector<int> Fq;
	vector<int> g;
	vector<int> h;  //公钥 
	vector<int> r;  //加密过程中用于置乱明文的小多项式

	// 一些辅助函数，如多项式生成和多项式运算函数
	vector<int> generate_poly(int N, int a, int b);  //随机生成属于环R（a，b）的N-1次多项式
	void rotateLeft(vector<int>& v, int k);     //多项式循环左移k位，用于求模逆算法中
	vector<int> convolution(vector<int> A, vector<int> B);  //多项式卷积运算

	vector<int> add_polynomials(const vector<int>& a, const vector<int>& b);      //多项式加法 
	vector<int> subtract_polynomials(const vector<int>& a, const vector<int>& b);//多项式减法
	void polynomial_division(vector<int> a, vector<int> b, vector<int>& q, vector<int>& r);    //多项式除法


	vector<int> inverse_qin_q(const vector<int>& a);  //秦九韶大衍求一术求多项式关于q的模逆，通常q为2的指数幂

	void mod_p(vector<int>& a);  //多项式模p，系数调整在{-1，0，1}中
	void mod_2(vector<int>& a);  //多项式模2，系数调整在{0，1}中
	void mod_q(vector<int>& a);   //多项式模q，系数调整在 (-q/2，q/2)中
	void remove_zeros(vector<int>& a); //去掉多项式末尾的0

	vector<vector<int>> convert(const string& str);      //转换函数，将string明文转换为三进制，分为长度为N的组。
	string reverse_convert(vector<vector<int>> split);    //逆转换函数，将分组长度为N的数组，转换为string类型
};