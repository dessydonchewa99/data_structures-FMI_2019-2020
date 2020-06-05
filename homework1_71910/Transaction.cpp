//
//  Transaction.cpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include "Transaction.hpp"

Transaction::Transaction()
{
    time = 0;
    senderId = 0;
    receiverId = 0;
    fmiCoins = 0.0;
}

ostream& operator<<(ostream& obj, Transaction& t)
{
    obj << "Transaction:" << endl << t.time << endl << t.senderId << endl << t.receiverId << endl << t.fmiCoins << endl;
    return obj;
}
