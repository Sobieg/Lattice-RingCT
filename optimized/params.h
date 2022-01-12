#ifndef PARAMS_H
#define PARAMS_H

#define RINGCT_Q 12289
#define RINGCT_K 8 /* used in noise sampling */
#define RINGCT_N 512/**/

#define RINGCT_SYMBYTES 32 /* size of shared key, seeds/coins, and hashes */

#define RINGCT_POLYBYTES ((14 * RINGCT_N) / 8)
#define RINGCT_POLYCOMPRESSEDBYTES ((3 * RINGCT_N) / 8)

#define RINGCT_CPAPKE_PUBLICKEYBYTES (RINGCT_POLYBYTES + RINGCT_SYMBYTES)
#define RINGCT_CPAPKE_SECRETKEYBYTES (RINGCT_POLYBYTES)
#define RINGCT_CPAPKE_CIPHERTEXTBYTES (RINGCT_POLYBYTES + RINGCT_POLYCOMPRESSEDBYTES)

#define RINGCT_CPAKEM_PUBLICKEYBYTES RINGCT_CPAPKE_PUBLICKEYBYTES
#define RINGCT_CPAKEM_SECRETKEYBYTES RINGCT_CPAPKE_SECRETKEYBYTES
#define RINGCT_CPAKEM_CIPHERTEXTBYTES RINGCT_CPAPKE_CIPHERTEXTBYTES

#define RINGCT_CCAKEM_PUBLICKEYBYTES RINGCT_CPAPKE_PUBLICKEYBYTES
#define RINGCT_CCAKEM_SECRETKEYBYTES (RINGCT_CPAPKE_SECRETKEYBYTES + RINGCT_CPAPKE_PUBLICKEYBYTES + 2 * RINGCT_SYMBYTES)
#define RINGCT_CCAKEM_CIPHERTEXTBYTES (RINGCT_CPAPKE_CIPHERTEXTBYTES + RINGCT_SYMBYTES) /* Second part is for Targhi-Unruh */

#endif
