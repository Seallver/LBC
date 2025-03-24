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
	// ���캯����ʹ��Ĭ�ϲ��������ɹ�������
	NTRU();
	// ���캯����ʹ�ø������������ɹ�������
	NTRU(int N, int p, int q, int Df, int Dg, int Dr);
	// ���ɹ�Կ��˽Կ
	void generate_keys();
	vector<int> get_public_key();
	pair<vector<int>, vector<int>> get_private_key();
	// ����
	vector<int> encrypt(const vector<int>& plaintext, const vector<int>& public_key);
	string encrypt(const string& plaintext, string public_key);
	// ����
	vector<int> decrypt(const vector<int>& ciphertext, const pair<vector<int>, vector<int>>& private_key);
	string decrypt(string result, string private_key);

	void print_poly(vector<int> poly);

	vector<int> inverse_qin_p(const vector<int>& a);  //�ؾ��ش�����һ�������ʽ����p��ģ�棬ͨ��p=3

private:
	int N;   //����ʽϵ��
	int p;   //ϵ��ģ��
	int q;   //����ģ��
	int Df;     //���ܹ��̶���ʽ����������
	int Dg;
	int Dr;
	vector<int> f;   //˽Կ
	vector<int> Fp;
	vector<int> Fq;
	vector<int> g;
	vector<int> h;  //��Կ 
	vector<int> r;  //���ܹ����������������ĵ�С����ʽ

	// һЩ���������������ʽ���ɺͶ���ʽ���㺯��
	vector<int> generate_poly(int N, int a, int b);  //����������ڻ�R��a��b����N-1�ζ���ʽ
	void rotateLeft(vector<int>& v, int k);     //����ʽѭ������kλ��������ģ���㷨��
	vector<int> convolution(vector<int> A, vector<int> B);  //����ʽ�������

	vector<int> add_polynomials(const vector<int>& a, const vector<int>& b);      //����ʽ�ӷ� 
	vector<int> subtract_polynomials(const vector<int>& a, const vector<int>& b);//����ʽ����
	void polynomial_division(vector<int> a, vector<int> b, vector<int>& q, vector<int>& r);    //����ʽ����


	vector<int> inverse_qin_q(const vector<int>& a);  //�ؾ��ش�����һ�������ʽ����q��ģ�棬ͨ��qΪ2��ָ����

	void mod_p(vector<int>& a);  //����ʽģp��ϵ��������{-1��0��1}��
	void mod_2(vector<int>& a);  //����ʽģ2��ϵ��������{0��1}��
	void mod_q(vector<int>& a);   //����ʽģq��ϵ�������� (-q/2��q/2)��
	void remove_zeros(vector<int>& a); //ȥ������ʽĩβ��0

	vector<vector<int>> convert(const string& str);      //ת����������string����ת��Ϊ�����ƣ���Ϊ����ΪN���顣
	string reverse_convert(vector<vector<int>> split);    //��ת�������������鳤��ΪN�����飬ת��Ϊstring����
};