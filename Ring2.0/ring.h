#ifndef RING_H
#define RING_H


#include "params.h"
#include "poly.h"

/**
*function: setup

*/

typedef struct {
	poly a;
	poly cn;
} IW;

void LRCT_Setup(poly *A, poly *H, size_t mLen);
void LRCT_KeyGen(poly *a, poly *A, poly *S, size_t mLen);
void LRCT_SigGen(poly *c1, poly **t, poly *h, poly *A, poly *H,
				 poly *S, poly *u, size_t mLen, poly *L, uint8_t w,
				 uint8_t pai, unsigned char *msg, int msgLen);
int LRCT_SigVer(const poly *c1, poly **t, poly *A, poly *H, size_t mLen, poly *h, poly *L,
				uint8_t w, unsigned char *msg, int msgLen);
/////����������׷���
/*
����˵����
*/
void LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen);
void LRCT_Spend(IW *iwOA, poly *ckOA, poly *c1, poly **t, poly *h, poly *L, unsigned char* bSignMess, size_t sigMsgLen, IW *iws, size_t iwsLen,
				int PaiInd, poly *skPai, poly *ckPai, unsigned char* bVal, size_t bvalLen, poly *OA, poly *A, poly *H, size_t mLen);
int LRCT_Verify(poly *c1, poly **t, poly *h, poly* A, poly *H, size_t mLen, 
	unsigned char* bSignMess, size_t sigMsgLen, poly *L, size_t iwsLen);

//
/*
����������ǩ������
*/

/*
����������ϵͳ��ʼ����������������
�������������A,H,����mLen
*/
void MIMO_LRCT_Setup(poly *A, poly *H, size_t mLen);
/*
������������Կ������
���룺����A,˽ԿS, ��������mLen
������û���Կ
*/
void MIMO_LRCT_KeyGen(poly *a, poly *A, poly *S, size_t mLen);
/*
����������ǩ������
���룺˽Կ�б�SList, ����NLen, ��������A, H, ������mLen, ��Կ�б�LList, ����wLen, ������λ��pai, ǩ����Ϣmsg,��Ϣ����msgLen
���������ʽc1, t�б�tList, h�б�hList.
*/
void MIMO_LRCT_SigGen(poly *c1, poly *tList, poly *hList, poly *SList, int NLen,
	poly *A, poly *H, int mLen, poly *LList, int wLen, uint8_t pai, unsigned char *msg, int msgLen);
/*
����������ǩ����֤
���룺ǩ����c1�� t�б�tList, h�б�hList, ���׸�������������A,H, ������ mLen, ��Կ�б�LList, �б���wLen, ǩ����Ϣ����Ϣ���ȣ�
*/
int MIMO_LRCT_SigVer(poly *c1, poly *tList, poly *hList, int NLen, poly *A, poly *H,
	size_t mLen, poly *LList, int wLen, unsigned char *msg, int msgLen);
/*

*/
void MIMO_LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen);
/// 
void MIMO_LRCT_Hash(int *pTable, poly *cn, poly *a, poly *ia, int beta);

//////
void ZKP_OR(poly *ck , int bit, int betaLen);
////////////

void LRCT_Lift(poly *LA, poly *A, poly *a, size_t mLen);
/*
������������ŵ��Ϣm�� r = A*sk + m 
���룺��������A, ˽Կsk, �������� mLen,  ������Ϣm,��Ϣ����bMessage
�������ŵr(����ʽN*1)
*/
void LRCT_Com(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen);
void LRCT_nttCom(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen);

/*
��������������A (N*M)* ����s��M*1��
���룺����A, ����s,��������mLen
���������r = A*s (N*1)
*/
void LRCT_MatrixMulPoly(poly *r, poly *A, poly *s, size_t mLen);
/*
��������:���� * ����
���룺����cof, ����A, ����������mLen��
���������r
*/
void LRCT_ConstMulMatrix(poly *r, const poly *A, uint16_t cof, size_t mLen);
/*
��������������A (M��N��)* ����ʽp��N��1�У�
���룺����ʽp,����A����������mLen
���������ʽr(M��1��)
*/
void LRCT_PolyMultMatrix(poly *r, poly *p, poly *A,  size_t mLen);
/*
��������������ӷ���A+B��
���룺����A,����B,�����СmLen
���������R=��A+B��
**/
void LRCT_MatrixAddMatrix(poly *R, poly *A, poly *B, size_t mLen);
/*
�������������������A-B��
���룺����A,����B,�����СmLen
���������R =��A-B��
**/
void LRCT_MatrixSubMatrix(poly *R, poly *A, poly *B, size_t mLen);

///////////////////*
/*
������������Կ��ȡ����
���룺��Կ����mLen
���������ʽ����r(n*mLen)
*/
/////////////////
void LRCT_SampleKey(poly *r, size_t mLen);

void LRCT_MatrixShift(poly *desCK, poly* rCK, size_t mLen, int iNumber);

void LRCT_GetCK0(poly *CK0, poly * CK, size_t mLen, poly* CKi, int messBitLen);
#endif

