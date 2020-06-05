//
//  Wallet.hpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Wallet_hpp
#define Wallet_hpp

#include <stdio.h>
#include <iostream>
using namespace std;

struct Wallet
{
    char owner[256];
    unsigned id;
    double fiatMoney;
    
    Wallet();
    friend ostream& operator<<(ostream& obj, Wallet& w);
};

#endif /* Wallet_hpp */
