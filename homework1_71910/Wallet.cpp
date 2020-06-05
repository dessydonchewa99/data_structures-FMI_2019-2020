//
//  Wallet.cpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include "Wallet.hpp"
#include <string.h>
using namespace std;

Wallet::Wallet()
{
    strcpy (owner, "");
    id = 0;
    fiatMoney = 0.0;
}

ostream& operator<<(ostream& obj, Wallet& w)
{
    obj << "Wallet:" << endl << w.owner << endl << w.id << endl << w.fiatMoney << endl;
    return obj;
}
