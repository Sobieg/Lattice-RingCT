cmake_minimum_required(VERSION 3.21)
project(Lattice_RingCT_v2_0)

set(CMAKE_CXX_STANDARD 14)

#include_directories(optimized)
#include_directories(Ring2.0)

add_executable(Lattice_RingCT_v2_0_optimized
        optimized/api.h
        optimized/cpapke.c
        optimized/cpapke.h
        optimized/fips202.h
        optimized/kem.c
        optimized/ntt.c
        optimized/ntt.h
        optimized/params.h
        optimized/poly.c
        optimized/poly.h
        optimized/precomp.c
        optimized/reduce.c
        optimized/reduce.h
        optimized/verify.c
        optimized/verify.h
        )

add_executable(Lattice_RingCT_v2_0_classical
        Ring2.0/aes.c
        Ring2.0/aes.h
        Ring2.0/aes_c.c
        Ring2.0/aes_local.h
        Ring2.0/api.h
        Ring2.0/common.c
        Ring2.0/common.h
        Ring2.0/cryptoConfig.h
        Ring2.0/fips202.h
        Ring2.0/ntt.c
        Ring2.0/ntt.h
        Ring2.0/oqs.h
        Ring2.0/params.h
        Ring2.0/poly.c
        Ring2.0/poly.h
        Ring2.0/precomp.c
        Ring2.0/rand.c
        Ring2.0/rand.h
        Ring2.0/rand_nist.c
        Ring2.0/reduce.c
        Ring2.0/reduce.h
        Ring2.0/ring.c
        Ring2.0/ring.h
        Ring2.0/Ring2.0.c
        Ring2.0/ring_test.c
        Ring2.0/ring_test.h
        Ring2.0/sha256.c
        Ring2.0/sha256.h
        Ring2.0/sha3.c
        Ring2.0/sha3.h
#        Ring2.0/stdafx.cpp
        Ring2.0/targetver.h
        Ring2.0/verify.c
        Ring2.0/verify.h)
