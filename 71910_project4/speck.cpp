//
//  speck.cpp
//  darch
//
//  Created by Desislava Doncheva on 28.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <stdint.h>
#include <iostream>
#include "speck.hpp"

using namespace std;


#define ROR(x, r) ((x >> r) | (x << (64 - r)))
#define ROL(x, r) ((x << r) | (x >> (64 - r)))
#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
#define RR(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
#define ROUNDS 32

void encrypt(uint64_t *ct,
             uint64_t const pt[2],
             uint64_t const K[2])
{
    uint64_t y = pt[0], x = pt[1], b = K[0], a = K[1];
    
    for (int i = 0; i < ROUNDS - 1; i++) {
        R(x, y, a);
        R(x, y, b);
    }
    
    ct[0] = y;
    ct[1] = x;
}

void decrypt(uint64_t *ct,
             uint64_t const pt[2],
             uint64_t const K[2])
{
    uint64_t y = pt[0], x = pt[1], b = K[0], a = K[1];
    
    for (int i = (ROUNDS - 1)-1; i >= 0; i--) {
        RR(x, y, b);
        RR(x, y, a);
    }

    ct[0] = y;
    ct[1] = x;
}
