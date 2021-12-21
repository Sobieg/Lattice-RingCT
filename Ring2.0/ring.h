#ifndef RINGCT_RING_H
#define RINGCT_RING_H


#include "params.h"
#include "poly.h"


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

void LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen);
void LRCT_Spend(IW *iwOA, poly *ckOA, poly *c1, poly **t, poly *h, poly *L, unsigned char* bSignMess, size_t sigMsgLen, IW *iws, size_t iwsLen,
				int PaiInd, poly *skPai, poly *ckPai, unsigned char* bVal, size_t bvalLen, poly *OA, poly *A, poly *H, size_t mLen);
int LRCT_Verify(poly *c1, poly **t, poly *h, poly* A, poly *H, size_t mLen, 
	unsigned char* bSignMess, size_t sigMsgLen, poly *L, size_t iwsLen);
void MIMO_LRCT_Setup(poly *A, poly *H, size_t mLen);
void MIMO_LRCT_KeyGen(poly *a, poly *A, poly *S, size_t mLen);
void MIMO_LRCT_SigGen(poly *c1, poly *tList, poly *hList, poly *SList, int NLen,
	poly *A, poly *H, int mLen, poly *LList, int wLen, uint8_t pai, unsigned char *msg, int msgLen);
int MIMO_LRCT_SigVer(poly *c1, poly *tList, poly *hList, int NLen, poly *A, poly *H,
	size_t mLen, poly *LList, int wLen, unsigned char *msg, int msgLen);
void MIMO_LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen);
void MIMO_LRCT_Hash(int *pTable, poly *cn, poly *a, poly *ia, int beta);


void ZKP_OR(poly *ck , int bit, int betaLen);


void LRCT_Lift(poly *LA, poly *A, poly *a, size_t mLen);
void LRCT_Com(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen);
void LRCT_nttCom(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen);
void LRCT_MatrixMulPoly(poly *r, poly *A, poly *s, size_t mLen);
void LRCT_ConstMulMatrix(poly *r, const poly *A, uint16_t cof, size_t mLen);
void LRCT_PolyMultMatrix(poly *r, poly *p, poly *A,  size_t mLen);
void LRCT_MatrixAddMatrix(poly *R, poly *A, poly *B, size_t mLen);
void LRCT_MatrixSubMatrix(poly *R, poly *A, poly *B, size_t mLen);
void LRCT_SampleKey(poly *r, size_t mLen);
void LRCT_MatrixShift(poly *desCK, poly* rCK, size_t mLen, int iNumber);
void LRCT_GetCK0(poly *CK0, poly * CK, size_t mLen, poly* CKi, int messBitLen);
#endif

