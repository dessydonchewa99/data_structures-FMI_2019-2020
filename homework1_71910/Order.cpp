//
//  Order.cpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include "Order.hpp"
using namespace std;

Order::Order()
{
    type = SELL;
    walletId = 0;
    fmiCoins = 0.0;
}
ostream& operator<<(ostream& obj, Order& o)
{
    obj << "Order:" << endl << o.type << endl << o.walletId << endl << o.fmiCoins << endl;
    return obj;
}
