//
//  Transaction.hpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Transaction_hpp
#define Transaction_hpp

#include <stdio.h>
#include <time.h>
#include <iostream>
using namespace std;

struct Transaction
{
    time_t time;
    unsigned senderId;
    unsigned receiverId;
    double fmiCoins;
    
    Transaction();
    friend ostream& operator<<(ostream& obj, Transaction& t);
};

#endif /* Transaction_hpp */
