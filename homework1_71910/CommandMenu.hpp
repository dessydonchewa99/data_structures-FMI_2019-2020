//
//  CommandMenu.hpp
//  homework1
//
//  Created by Desislava Doncheva on 3.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef CommandMenu_hpp
#define CommandMenu_hpp

#include <stdio.h>
#include "TList.hpp"

const int MAX_COMMAND_ARG = 25;
const double MONEY_COIN_RATE = 375.0;
const unsigned SYSTEM_WALLET_ID = 4294967295;

struct WalletInfo
{
    unsigned walletid;
    char walletName[256];
    double Coins;
    unsigned numberTrans;
    time_t firstTime;
    time_t lastTime;
};

class CommandMenu
{
private:
    TList<Wallet>* wl;
    TList<Transaction>* tl;
    TList<Order>* ol;
    char ComplitedFileOrders[256];
    
    void add_wallet(double money, char* name);
    void make_order(Order::Type typeOrder, double coins, unsigned wid);
    void transfer(unsigned swid, unsigned rwid, double coins);
    void wallet_info(unsigned wid);
    void attract_investors();
    void list_wallets();
    void list_transactions();
    void list_orders();
    void quit();
    void help();
    
    unsigned getMaxWalletId();
    double accountBalance(unsigned wid);
    unsigned numberTransactions(unsigned wid);
    time_t firstTimeTransaction(unsigned wid);
    time_t lastTimeTransaction(unsigned wid);
    double getWalletBalance(unsigned wid);
    bool setWalletBalance(unsigned wid, double money);
    void logOrder(ofstream* logFileFs, unsigned walletid1, unsigned walletid2, double coins);
    
public:
    CommandMenu(TList<Wallet>* wl, TList<Transaction>* tl, TList<Order>* ol, char* ComplitedFileOrders);
    bool processCommand (char* commandline);
    
    ~CommandMenu();
    
};

#endif /* CommandMenu_hpp */
