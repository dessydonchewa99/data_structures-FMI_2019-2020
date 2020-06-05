//
//  Order.hpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Order_hpp
#define Order_hpp

#include <stdio.h>
#include <iostream>
using namespace std;

struct Order
{
    enum Type { SELL, BUY } type;
    unsigned walletId;
    double fmiCoins;
    
    Order();
    friend ostream& operator<<(ostream& obj, Order& o);
};
#endif /* Order_hpp */
