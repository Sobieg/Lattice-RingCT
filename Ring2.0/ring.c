#include <stdio.h>
#include "ring.h"
#include "rand.h"
#include "common.h"
#include "sha256.h"


void LRCT_SampleKey(poly *r, size_t mLen)
{
	uint8_t seed[RINGCT_SYMBYTES] = { 0 };
	size_t i;
	for ( i = 0; i < mLen; i++)
	{
		randombytes(seed, RINGCT_SYMBYTES);
		for (size_t j = 0; j < RINGCT_SYMBYTES; j++)
		{

			r[i].coeffs[j * 8 + 0] = (seed[j] & 0x01);
			r[i].coeffs[j * 8 + 1] = (seed[j] & 0x02)>>1;
			r[i].coeffs[j * 8 + 2] = (seed[j] & 0x04)>>2;
			r[i].coeffs[j * 8 + 3] = (seed[j] & 0x08)>>3;
			r[i].coeffs[j * 8 + 4] = (seed[j] & 0x10)>>4;
			r[i].coeffs[j * 8 + 5] = (seed[j] & 0x20)>>5;
			r[i].coeffs[j * 8 + 6] = (seed[j] & 0x40)>>6;
			r[i].coeffs[j * 8 + 7] = (seed[j] & 0x80)>>7;
		}
		randombytes(seed, RINGCT_SYMBYTES);
		for (size_t j = 0; j < RINGCT_SYMBYTES; j++)
		{

			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 0] = (seed[j] & 0x01);
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 1] = (seed[j] & 0x02)>>1;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 2] = (seed[j] & 0x04)>>2;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 3] = (seed[j] & 0x08)>>3;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 4] = (seed[j] & 0x10)>>4;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 5] = (seed[j] & 0x20)>>5;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 6] = (seed[j] & 0x40)>>6;
			r[i].coeffs[RINGCT_SYMBYTES * 8 + j * 8 + 7] = (seed[j] & 0x80)>>7;
		}
		 
	}

}
void LRCT_Setup(poly *A, poly *H, size_t mLen)
{

	uint8_t seed[RINGCT_SYMBYTES] = { 0 };
	size_t i = 0;

	for ( i = 0; i < mLen; i++)
	{
		randombytes(seed, RINGCT_SYMBYTES); // why do we use seed?
		poly_uniform(A + i, seed);
		poly_serial(A + i);
		randombytes(seed, RINGCT_SYMBYTES);
		poly_uniform(H + i, seed);
		poly_serial(H + i);
	}
}

void LRCT_KeyGen(poly *a, poly *A, poly *S, size_t mLen)
{
	LRCT_MatrixMulPoly(a, A, S,  mLen);
	poly_serial(a);
}

void LRCT_SigGen(poly *c1, poly **t, poly *h, poly *A, poly *H, poly *S, poly *u, size_t mLen, poly *L, uint8_t w, uint8_t pai, unsigned char *msg, int msgLen)
{
	//H2q
	size_t i, j, k;
	poly *H2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *S2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *A2qp = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *tmp2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly  tmp, tmp1;
	poly c,  cpai;
	SHA256_CTX ctx;
	unsigned char bHash[32] = { 0 };
	unsigned char bpoly[RINGCT_POLYBYTES] = { 0 };
	unsigned char bt[RINGCT_POLYBYTES] = { 0 };
	uint8_t coin = 0;
	for ( i = 0; i < (mLen+1); i++)
	{
		poly_init(H2q + i);
		poly_init(S2q + i);
		poly_init(A2qp + i);
		poly_init(tmp2q + i);
	}
	///////////1.
	LRCT_MatrixMulPoly(h, H, S, mLen);//h = HS_{pai}
	LRCT_Lift(H2q, H, h, mLen);//H_{2q}
	poly_copy(S2q, S, mLen);
	poly_setValue(S2q + mLen, 1);//S_{2q}
	///////////2.
	LRCT_Lift(A2qp, A, L + pai, mLen);
	SHA256_Init(&ctx);
	for (i = 0; i < w; i++)
	{
		poly_tobytes(bpoly, L + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
	}
	for ( i = 0; i < mLen+1; i++)
	{
		poly_tobytes(bpoly, H2q + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
	}
	SHA256_Update(&ctx, msg, msgLen);//msg

	LRCT_MatrixMulPoly(&tmp, A2qp, u, mLen + 1);
	poly_tobytes(bpoly, &tmp);
	SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//A2qb*U

	LRCT_MatrixMulPoly(&tmp, H2q, u, mLen + 1);
	poly_tobytes(bpoly, &tmp);
	SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//H2q*U
	SHA256_Final(bHash, &ctx);//C_(pai+1)
	SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bt);
	poly_frombytes(&c, bt);
	poly_serial(&c);
	poly_print(&c);
	/////////////////////////////////////
	for ( i = 0; i < (w-1); i++)
	{
		j = (pai + i+1) % w;
		if (j == 0)
		{
			poly_cofcopy(c1, &c);
		}
		LRCT_Lift(tmp2q, A, L + j, mLen);
		SHA256_Init(&ctx);
		for (k = 0; k < w; k++)
		{
			poly_tobytes(bpoly, L + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
		for (k = 0; k < mLen+1; k++)
		{
			poly_tobytes(bpoly, H2q + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
		SHA256_Update(&ctx, msg, msgLen);//msg
		
		for ( k = 0; k < mLen+1; k++)
		{
			randombytes(bt, RINGCT_POLYBYTES);
			poly_frombytes(t[j] + k, bt);
			poly_serial(t[j] + k);
		}
		LRCT_MatrixMulPoly(&tmp, tmp2q, t[j], mLen + 1);
		poly_constmul(&tmp1, &c, RINGCT_Q);
		poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
		poly_tobytes(bpoly, &tmp);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//

		LRCT_MatrixMulPoly(&tmp, H2q, t[j], mLen + 1);
		poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
		poly_tobytes(bpoly, &tmp);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//H2q*U
		SHA256_Final(bHash, &ctx);//C_(pai+1)
		SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bt);
		poly_frombytes(&c, bt);
		poly_serial(&c);//C_{j+1}
		if (j == (pai-1))
		{
			poly_cofcopy(&cpai, &c);
			break;
		}

	}
	randombytes(&coin, 1);
	LRCT_PolyMultMatrix(tmp2q, &cpai, S2q, mLen + 1);//S2qpai *c_pai
	if (coin&0x01)//b =1
	{
		LRCT_MatrixSubMatrix(t[pai], u, tmp2q, mLen + 1);
	}
	else {
		LRCT_MatrixAddMatrix(t[pai], u, tmp2q, mLen + 1);
	}

	free(H2q);
	free(S2q);
	free(A2qp);
	free(tmp2q);
}
int LRCT_SigVer(const poly *c1, poly **t, poly *A, poly *H, size_t mLen, poly *h, poly *L,
	uint8_t w, unsigned char *msg, int msgLen)
{
	size_t i,k;
	poly *H2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *A2qp = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly c, tmp, tmp1;
	SHA256_CTX ctx;
	unsigned char bHash[32] = { 0 };
	unsigned char bpoly[RINGCT_POLYBYTES] = { 0 };
	for (i = 0; i < (mLen + 1); i++)
	{
		poly_init(H2q + i);
		//poly_init(S2q + i);
		poly_init(A2qp + i);
	}
	LRCT_Lift(H2q, H, h, mLen);
	poly_cofcopy(&c, c1);
	for ( i = 0; i < w; i++)
	{
		LRCT_Lift(A2qp, A, L+i, mLen);
		SHA256_Init(&ctx);
		for (k = 0; k < w; k++)
		{
			poly_tobytes(bpoly, L + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
		for (k = 0; k < mLen+1; k++)
		{
			poly_tobytes(bpoly, H2q + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
		SHA256_Update(&ctx, msg, msgLen);//msg

		poly_constmul(&tmp1, &c, RINGCT_Q);

		LRCT_MatrixMulPoly(&tmp, A2qp, t[i], mLen + 1);
		poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
		poly_tobytes(bpoly, &tmp);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//A2qb*U

		LRCT_MatrixMulPoly(&tmp, H2q, t[i], mLen + 1);
		poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
		poly_serial(&tmp);
		poly_tobytes(bpoly, &tmp);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//H2q*U
		SHA256_Final(bHash, &ctx);//C_(pai+1)
		printf("bHash======================%d:\n", i);
        print_bytes(bHash, 32);
		SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bpoly);
		poly_frombytes(&c, bpoly);
		poly_serial(&c);
	}
	free(H2q);
	free(A2qp);
	if (poly_equal(&c, c1) ==1)
	{
		return 1;
	}
	return 0;
}

void LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen)
{
	LRCT_SampleKey(ck, mLen);
//    LRCT_nttCom(&(iw->cn), A, ck, mLen, bMessage, msglen);
    LRCT_Com(&(iw->cn), A, ck, mLen, bMessage, msglen);
	poly_cofcopy(&(iw->a), a);
}
void LRCT_Spend(IW *iwOA, poly *ckOA, poly *c1, poly **t, poly *h, poly *L, unsigned char* bSignMess, size_t sigMsgLen, IW *iws, size_t iwsLen,
	int PaiInd, poly *skPai, poly *ckPai, unsigned char* bVal, size_t bvalLen, poly *OA, poly *A, poly *H, size_t mLen)
{

	poly *u = (poly *)malloc((mLen+1)*sizeof(poly));
	poly *S = (poly *)malloc((mLen) * sizeof(poly));
	size_t i;
	poly tmp;
	LRCT_Mint(iwOA, ckOA, OA, A, mLen, bVal, bvalLen);

	for ( i = 0; i < iwsLen; i++)
	{
		poly_add(&tmp, &iws[i].a, &iws[i].cn);
		poly_sub(L + i, &tmp, &(iwOA->cn));
	}
	LRCT_SampleKey(u, mLen + 1);
	LRCT_MatrixAddMatrix(S, skPai, ckPai, mLen);
	LRCT_MatrixSubMatrix(S, S, ckOA, mLen);
	LRCT_SigGen(c1, t, h, A, H, S, u, mLen, L, iwsLen, PaiInd, bSignMess, sigMsgLen);

	free(u);
	free(S);
}
int LRCT_Verify(poly *c1, poly **t, poly *h, poly* A, poly *H, size_t mLen,
	unsigned char* bSignMess, size_t sigMsgLen, poly *L, size_t iwsLen)
{
	int result = 0;
	result = LRCT_SigVer(c1, t, A, H, mLen, h, L, iwsLen, bSignMess, sigMsgLen);
	return result;
}
/////multiple
void MIMO_LRCT_Setup(poly *A, poly *H, size_t mLen)
{
	uint8_t seed[RINGCT_SYMBYTES] = { 0 };
	size_t i = 0;

	for (i = 0; i < mLen; i++)
	{
		randombytes(seed, RINGCT_SYMBYTES);
		poly_uniform(A + i, seed);
		poly_serial(A + i);
		randombytes(seed, RINGCT_SYMBYTES);
		poly_uniform(H + i, seed);
		poly_serial(H + i);
	}
}
void MIMO_LRCT_KeyGen(poly *a, poly *A, poly *S, size_t mLen)
{
	LRCT_MatrixMulPoly(a, A, S, mLen);
	poly_serial(a);
}
void MIMO_LRCT_Mint(IW *iw, poly *ck, poly *a, poly *A, size_t mLen, unsigned char* bMessage, size_t msglen)
{
	LRCT_SampleKey(ck, mLen);
//    LRCT_nttCom(&(iw->cn), A, ck, mLen, bMessage, msglen);
    LRCT_Com(&(iw->cn), A, ck, mLen, bMessage, msglen);
	poly_cofcopy(&(iw->a), a);
}
void MIMO_LRCT_Hash(int *pTable, poly *cn, poly *a, poly *ia, int beta)
{
	SHA256_CTX ctx;
	unsigned char bHash[32] = { 0 };
	unsigned char bpoly[RINGCT_POLYBYTES] = { 0 };
	unsigned char bt[576] = { 0 };
	int i;
	int tmpTable[RINGCT_N] = { 0 };
	for ( i = 0; i < RINGCT_N; i++)
	{
		tmpTable[i] = i;
	}
	SHA256_Init(&ctx);
	////H(L)
	for (i = 0; i < beta; i++)
	{
		poly_tobytes(bpoly, cn + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		poly_tobytes(bpoly, a + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		poly_tobytes(bpoly, ia + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
	}///H_1(L||)
	SHA256_Final(bHash, &ctx);//C_(pai)
	SHA256_KDF(bHash, 32, 576, bt);


}
////

void ZKP_OR(poly *ck, int bit, int betaLen)
{}
//////
void MIMO_LRCT_SigGen(poly *c1, poly *tList, poly *hList, poly *SList, int NLen,
	poly *A, poly *H, int mLen,  poly *LList, int wLen, uint8_t pai, unsigned char *msg, int msgLen)
{
	poly *H2q = (poly *)malloc(NLen*(mLen + 1) * sizeof(poly));
	poly *A2qp = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *tmp2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly *u = (poly *)malloc(NLen*(mLen + 1) * sizeof(poly));
	poly *S2q = (poly *)malloc((mLen + 1) * sizeof(poly));
	/////
	SHA256_CTX ctx;
	poly tmp, tmp1, ctmp;
	poly c, cpai;
	unsigned char bHash[32] = { 0 };
	unsigned char bpoly[RINGCT_POLYBYTES] = { 0 };
	unsigned char bt[RINGCT_POLYBYTES] = { 0 };
	uint8_t coin = 0;
	int i = 0;
	int k = 0;
	int j = 0;
	int index = 0;
	//��ʼ����̬����
	for (i = 0; i < (mLen + 1); i++)
	{
		poly_init(A2qp + i);
		poly_init(S2q + i);
		poly_init(tmp2q + i);
	}
	for ( i = 0; i < NLen*(mLen+1); i++)
	{
		poly_init(H2q + i);
		poly_init(u+i);
	}
	/////
	SHA256_Init(&ctx);
	////H(L)
	for ( i = 0; i < wLen*NLen; i++)
	{
		poly_tobytes(bpoly, LList + i);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
	}///H_1(L||)
	///H(L||H2q..)
	for (i = 0; i < NLen; i++)
	{
		LRCT_MatrixMulPoly(hList + i, H, SList + i * mLen, mLen);
		LRCT_Lift(H2q + i * (mLen + 1), A, hList + i, mLen);
		for (k = 0; k < mLen + 1; k++)
		{
			poly_tobytes(bpoly, H2q + i * (mLen + 1) + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
	}
	////H(L||...||mu)
	SHA256_Update(&ctx, msg, msgLen);
	/////u
	for ( i = 0; i < NLen; i++)
	{
		for (k = 0; k < mLen + 1; k++)
		{
			randombytes(bt, RINGCT_POLYBYTES);
			poly_frombytes(u + i * (mLen + 1) + k, bt);
			poly_serial(u + i * (mLen + 1) + k);
		}
	}
	//////H(L||...||mu||(A2qp*U ,H2q*U)...||)
	for (i = 0; i < NLen; i++)
	{
		LRCT_Lift(A2qp, A, LList + i*wLen + (pai - 1), mLen);
		LRCT_MatrixMulPoly(&tmp, A2qp, u + i * (mLen + 1), mLen + 1);

		LRCT_MatrixMulPoly(&tmp1, H2q + i * (mLen + 1), u+ i * (mLen + 1), mLen + 1);
		poly_tobytes(bpoly, &tmp);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		poly_tobytes(bpoly, &tmp1);
		SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
	}
	SHA256_Final(bHash, &ctx);//C_(pai)
	SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bt);
	poly_frombytes(&c, bt);
	poly_serial(&c);
	//////////////////////
	poly_cofcopy(&ctmp, &c);
	for (i = 0; i < (wLen-1) ; i++)
	{
		index = (pai + i ) % (wLen);
		if (index == 0)
		{
			poly_cofcopy(c1, &ctmp);
		}

		SHA256_Init(&ctx);
		////H_1(L||)
		for (j = 0; j < wLen*NLen; j++)
		{
			poly_tobytes(bpoly, LList + j);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}
		for (j = 0; j < NLen; j++)
		{
			for (k = 0; k < mLen + 1; k++)
			{
				poly_tobytes(bpoly, H2q + j * (mLen + 1) + k);
				SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
			}
		}//H_1(L||H2q)
		SHA256_Update(&ctx, msg, msgLen);//H(L||...||mu)

		poly_constmul(&tmp1, &ctmp, RINGCT_Q);//qC_i
		for (j = 0; j < NLen; j++)
		{
		   LRCT_Lift(tmp2q, A, LList + j * wLen + index, mLen);
			for (k = 0; k < mLen + 1; k++)
			{
				randombytes(bt, RINGCT_POLYBYTES);
				poly_frombytes(tList + j * wLen*(mLen + 1) + index * (mLen + 1) + k, bt);
				poly_serial(tList + j * wLen*(mLen + 1) + index * (mLen+1)+ k);
			}
			LRCT_MatrixMulPoly(&tmp, tmp2q, tList + j * wLen*(mLen + 1) + index * (mLen + 1), mLen + 1);
		
			poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
			poly_tobytes(bpoly, &tmp);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//
			////////
			LRCT_MatrixMulPoly(&tmp, H2q + j * (mLen + 1), tList + j * wLen*(mLen + 1) + index * (mLen + 1), mLen + 1);
			poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
			poly_tobytes(bpoly, &tmp);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//H2q*U
		}
		SHA256_Final(bHash, &ctx);//
		SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bt);
		poly_frombytes(&ctmp, bt);
		poly_serial(&ctmp);//C_{index+1}
		if (index == (pai - 2))
		{
			poly_cofcopy(&cpai, &ctmp);
			break;
		}
	}
	for ( i = 0; i < NLen; i++)
	{
		poly_copy(S2q, SList+i*mLen, mLen);
		poly_setValue(S2q + mLen, 1);//S_{2q}
		//////
		randombytes(&coin, 1);
		LRCT_PolyMultMatrix(tmp2q, &cpai, S2q, mLen + 1);//S2qpai *c_pai
		if (coin & 0x01)//b =1
		{
			LRCT_MatrixSubMatrix(tList + i * wLen*(mLen + 1) + (pai-1) * (mLen + 1), u + i * (mLen + 1), tmp2q, mLen + 1);
		}
		else {
			LRCT_MatrixAddMatrix(tList + i * wLen*(mLen + 1) + (pai - 1) * (mLen + 1), u+ i * (mLen + 1), tmp2q, mLen + 1);
		}
	}
	/////
	//��ʼ����̬����
		free(A2qp);
		free(S2q);
		free(tmp2q);
		free(H2q );
		free(u);


}
int MIMO_LRCT_SigVer(poly *c1, poly *tList, poly *hList, int NLen, poly *A, poly *H,
	size_t mLen, poly *LList, int wLen, unsigned char *msg, int msgLen)
{
	size_t i,j, k;
	poly *H2q = (poly *)malloc(NLen*(mLen + 1) * sizeof(poly));
	poly *A2qp = (poly *)malloc((mLen + 1) * sizeof(poly));
	poly ctmp,tmp, tmp1;
	SHA256_CTX ctx;
	unsigned char bHash[32] = { 0 };
	unsigned char bpoly[RINGCT_POLYBYTES] = { 0 };
	/////////
	poly_cofcopy(&ctmp, c1);
	for (i = 0; i < NLen; i++)
	{
		LRCT_Lift(H2q + i * (mLen + 1), A, hList + i, mLen);
	}
	//////
	for (i = 0; i < wLen; i++)
	{
		SHA256_Init(&ctx);
		for (k = 0; k < wLen*NLen; k++)
		{
			poly_tobytes(bpoly, LList + k);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
		}///H_1(L||)
		for (j = 0; j< NLen; j++)
		{
			for (k = 0; k < (mLen + 1); k++)
			{
				poly_tobytes(bpoly, H2q + j * (mLen + 1) + k);
				SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
			}
		}
		SHA256_Update(&ctx, msg, msgLen);//H(L||...||mu)

		poly_constmul(&tmp1, &ctmp, RINGCT_Q);//qC_i
		for ( j = 0; j < NLen; j++)
		{
			LRCT_Lift(A2qp, A, LList + j * wLen + i , mLen);
			LRCT_MatrixMulPoly(&tmp, A2qp, tList + j * wLen*(mLen + 1) + i * (mLen + 1), mLen + 1);
			poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
			poly_tobytes(bpoly, &tmp);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);
			
			LRCT_MatrixMulPoly(&tmp, H2q + j * (mLen + 1), tList + j * wLen*(mLen + 1) + i* (mLen + 1), mLen + 1);
			poly_add(&tmp, &tmp, &tmp1);//(+ qC_i)% Q
			poly_tobytes(bpoly, &tmp);
			SHA256_Update(&ctx, bpoly, RINGCT_POLYBYTES);//H2q*U
		}
		SHA256_Final(bHash, &ctx);//
		SHA256_KDF(bHash, 32, RINGCT_POLYBYTES, bpoly);
		poly_frombytes(&ctmp, bpoly);
		poly_serial(&ctmp);//
	}
	if (poly_equal(&ctmp, c1) == 1)
	{
		return 1;
	}
	return 0;	
}





void LRCT_Lift(poly *LA, poly *A, poly *a, size_t mLen)
{
	size_t i = 0;
	size_t j = 0;
	int16_t tmp = 0;
	for ( i = 0; i < mLen; i++)
	{
		for ( j = 0; j < RINGCT_N; j++)
		{
			LA[i].coeffs[j] = 2 * A[i].coeffs[j];
		}	
	}
	for ( j = 0; j < RINGCT_N; j++)
	{
		LA[mLen].coeffs[j] = coeff_freeze2Q(RINGCT_2Q+ RINGCT_Q - a->coeffs[j] * 2);
	}
}

void LRCT_Com(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen)
{
	poly tmp;
	size_t j;

	LRCT_MatrixMulPoly(&tmp, A, sk, mLen);
	poly_cofcopy(r, &tmp);
	for (j = 0; j < msglen; j++)
	{

		r->coeffs[j * 8 + 0] = (tmp.coeffs[j * 8 + 0] + (bMessage[j]&0x01))%RINGCT_Q;
		r->coeffs[j * 8 + 1] = (tmp.coeffs[j * 8 + 1] + ((bMessage[j] & 0x02) >> 1)) % RINGCT_Q;
		r->coeffs[j * 8 + 2] = (tmp.coeffs[j * 8 + 2] + ((bMessage[j] & 0x04) >> 2)) % RINGCT_Q;
		r->coeffs[j * 8 + 3] = (tmp.coeffs[j * 8 + 3] + ((bMessage[j] & 0x08) >> 3)) % RINGCT_Q;
		r->coeffs[j * 8 + 4] = (tmp.coeffs[j * 8 + 4] + ((bMessage[j] & 0x10) >> 4)) % RINGCT_Q;
		r->coeffs[j * 8 + 5] = (tmp.coeffs[j * 8 + 5] + ((bMessage[j] & 0x20) >> 5)) % RINGCT_Q;
		r->coeffs[j * 8 + 6] = (tmp.coeffs[j * 8 + 6] + ((bMessage[j] & 0x40) >> 6)) % RINGCT_Q;
		r->coeffs[j * 8 + 7] = (tmp.coeffs[j * 8 + 7] + ((bMessage[j] & 0x80) >> 7)) % RINGCT_Q;
	}
void LRCT_nttCom(poly *r, poly *A, poly *sk, size_t mLen, unsigned char *bMessage, size_t msglen)
{
	poly tmp, pMess;
	size_t j;
	poly_init(&pMess);
	LRCT_MatrixMulPoly(&tmp, A, sk, mLen);
	//poly_cofcopy(r, &tmp);
	for (j = 0; j < msglen; j++)
	{

		pMess.coeffs[j * 8 + 0] =  (bMessage[j] & 0x01) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 1] = (((bMessage[j] & 0x02) >> 1)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 2] = (((bMessage[j] & 0x04) >> 2)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 3] = ( ((bMessage[j] & 0x08) >> 3)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 4] = ( ((bMessage[j] & 0x10) >> 4)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 5] = ( ((bMessage[j] & 0x20) >> 5)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 6] = (((bMessage[j] & 0x40) >> 6)) % NEWHOPE_Q;
		pMess.coeffs[j * 8 + 7] = ( ((bMessage[j] & 0x80) >> 7)) % NEWHOPE_Q;
	}
	poly_ntt(&pMess);
	poly_add(r, &tmp, &pMess);
}



//N*M mul M*1  
void LRCT_MatrixMulPoly(poly *r, poly *A, poly *s, size_t mLen)
{
	size_t i;
	poly tmp, tmpA, tmps;
	poly_init(r);
	for ( i = 0; i < mLen; i++)
	{
		poly_cofcopy(&tmpA, A + i);
		poly_cofcopy(&tmps, s + i);
	  // poly_ntt(&tmpA);
		//poly_ntt(&tmps);
		poly_mul_pointwise(&tmp, &tmpA, &tmps);
		poly_add(r, r, &tmp);
	}
	//poly_invntt(r);
}
//M*N  mul N*1
void LRCT_PolyMultMatrix(poly *r, poly *p, poly *A, size_t mLen)
{
	size_t i;
	for ( i = 0; i < mLen; i++)
	{
		poly_mul_pointwise(r+i, A+i, p);
	}
}

void LRCT_MatrixAddMatrix(poly *R, poly *A, poly *B, size_t mLen)
{
	size_t i;
	for ( i = 0; i < mLen; i++)
	{
		poly_add(R + i, A + i, B + i);
	}
}
void LRCT_MatrixSubMatrix(poly *R, poly *A, poly *B, size_t mLen)
{
	size_t i;
	for (i = 0; i < mLen; i++)
	{
		poly_sub(R + i, A + i, B + i);
	}
}

void LRCT_ConstMulMatrix(poly *r, const poly *A, uint16_t cof, size_t mLen)
{
	size_t i, j;
	for (i = 0; i < mLen; i++)
	{
		for ( j = 0; j < NEWHOPE_N; j++)
		{
			r[i].coeffs[j] = cof * A[i].coeffs[j];
		}
		
	}
}
///
void LRCT_MatrixShift(poly *desCK, poly* rCK, size_t mLen, int iNumber)
{
	size_t i;
	for ( i = 0; i < mLen; i++)
	{
		poly_shift(desCK + i, rCK + i, iNumber);
	}
}

void LRCT_GetCK0(poly *CK0, poly * CK, size_t mLen, poly* CKi, int messBitLen)
{
	size_t i;
	poly *tmp = (poly *)malloc((mLen) * sizeof(poly));
	poly *desCK = (poly *)malloc((mLen) * sizeof(poly));
	for (i = 0; i < (mLen); i++)
	{
		poly_init(tmp + i);
		poly_init(desCK + i);
	}

	for ( i = 0; i < messBitLen; i++)
	{
		LRCT_MatrixShift(desCK, CKi + i * mLen, mLen, i + 1);
		LRCT_MatrixAddMatrix(tmp, tmp, desCK, mLen);
	}
	LRCT_MatrixSubMatrix(CK0, CK, tmp, mLen);
	free(tmp);
	free(desCK);
}
