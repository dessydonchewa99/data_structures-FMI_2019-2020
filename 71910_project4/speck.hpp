//
//  speck.hpp
//  darch
//
//  Created by Desislava Doncheva on 28.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef speck_hpp
#define speck_hpp

#include <stdio.h>
#include <stdint.h>

void encrypt(uint64_t *ct,
             uint64_t const pt[2],
             uint64_t const K[2]);

void decrypt(uint64_t *ct,
             uint64_t const pt[2],
             uint64_t const K[2]);

#endif /* speck_hpp */
