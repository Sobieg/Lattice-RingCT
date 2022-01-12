#include <string.h>
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "verify.h"

/*************************************************
* Name:        crypto_kem_keypair
*
* Description: Generates public and private key
*              for CCA secure NewHope key encapsulation
*              mechanism
*
* Arguments:   - unsigned char *pk: pointer to output public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*              - unsigned char *sk: pointer to output private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
//printfAPI STATUS crypto_kem_keypair(unsigned char *pk, unsigned char *sk) {
	size_t i;

	cpapke_keypair(pk, sk);
	sk += RINGCT_CPAPKE_SECRETKEYBYTES;

	for (i = 0; i < RINGCT_CPAPKE_PUBLICKEYBYTES; i++) /* Append the public key for re-encryption */
		sk[i] = pk[i];
	sk += RINGCT_CPAPKE_PUBLICKEYBYTES;

	SHA3_shake256(sk, RINGCT_SYMBYTES, pk, RINGCT_CPAPKE_PUBLICKEYBYTES); /* Append the hash of the public key */
	sk += RINGCT_SYMBYTES;

	randombytes(sk, RINGCT_SYMBYTES); /* Append the value s for pseudo-random output on reject */

	return SUCCESS;
}

/*************************************************
* Name:        crypto_kem_enc
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - unsigned char *ct:       pointer to output cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *pk: pointer to input public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
API STATUS crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk) {
	unsigned char k_coins_d[3 * RINGCT_SYMBYTES]; /* Will contain key, coins, qrom-hash */
	unsigned char buf[2 * RINGCT_SYMBYTES];
	int i;

	randombytes(buf, RINGCT_SYMBYTES);

	SHA3_shake256(buf, RINGCT_SYMBYTES, buf, RINGCT_SYMBYTES);                                /* Don't release system RNG output */
	SHA3_shake256(buf + RINGCT_SYMBYTES, RINGCT_SYMBYTES, pk, RINGCT_CCAKEM_PUBLICKEYBYTES); /* Multitarget countermeasure for coins + contributory KEM */
	SHA3_shake256(k_coins_d, 3 * RINGCT_SYMBYTES, buf, 2 * RINGCT_SYMBYTES);

	cpapke_enc(ct, buf, pk, k_coins_d + RINGCT_SYMBYTES); /* coins are in k_coins_d+RINGCT_SYMBYTES */

	for (i = 0; i < RINGCT_SYMBYTES; i++)
		ct[i + RINGCT_CPAPKE_CIPHERTEXTBYTES] = k_coins_d[i + 2 * RINGCT_SYMBYTES]; /* copy Targhi-Unruh hash into ct */

	SHA3_shake256(k_coins_d + RINGCT_SYMBYTES, RINGCT_SYMBYTES, ct, RINGCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c) */
	SHA3_shake256(ss, RINGCT_SYMBYTES, k_coins_d, 2 * RINGCT_SYMBYTES);                              /* hash concatenation of pre-k and h(c) to ss */
	return SUCCESS;
}

/*************************************************
* Name:        crypto_kem_dec
*
* Description: Generates shared secret for given
*              cipher text and private key
*
* Arguments:   - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *ct: pointer to input cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - const unsigned char *sk: pointer to input private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 for sucess or -1 for failure
*
* On failure, ss will contain a randomized value.
**************************************************/
API STATUS crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk) {
	int i, fail;
	unsigned char ct_cmp[RINGCT_CCAKEM_CIPHERTEXTBYTES];
	unsigned char buf[2 * RINGCT_SYMBYTES];
	unsigned char k_coins_d[3 * RINGCT_SYMBYTES]; /* Will contain key, coins, qrom-hash */
	const unsigned char *pk = sk + RINGCT_CPAPKE_SECRETKEYBYTES;

	cpapke_dec(buf, ct, sk);

	for (i = 0; i < RINGCT_SYMBYTES; i++) /* Use hash of pk stored in sk */
		buf[RINGCT_SYMBYTES + i] = sk[RINGCT_CCAKEM_SECRETKEYBYTES - 2 * RINGCT_SYMBYTES + i];
	SHA3_shake256(k_coins_d, 3 * RINGCT_SYMBYTES, buf, 2 * RINGCT_SYMBYTES);

	cpapke_enc(ct_cmp, buf, pk, k_coins_d + RINGCT_SYMBYTES); /* coins are in k_coins_d+RINGCT_SYMBYTES */

	for (i = 0; i < RINGCT_SYMBYTES; i++)
		ct_cmp[i + RINGCT_CPAPKE_CIPHERTEXTBYTES] = k_coins_d[i + 2 * RINGCT_SYMBYTES];

	fail = verify(ct, ct_cmp, RINGCT_CCAKEM_CIPHERTEXTBYTES);

	SHA3_shake256(k_coins_d + RINGCT_SYMBYTES, RINGCT_SYMBYTES, ct, RINGCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c)  */
	cmov(k_coins_d, sk + RINGCT_CCAKEM_SECRETKEYBYTES - RINGCT_SYMBYTES, RINGCT_SYMBYTES, fail);        /* Overwrite pre-k with z on re-encryption failure */
	SHA3_shake256(ss, RINGCT_SYMBYTES, k_coins_d, 2 * RINGCT_SYMBYTES);                              /* hash concatenation of pre-k and h(c) to k */

	return (STATUS) -fail;
}
