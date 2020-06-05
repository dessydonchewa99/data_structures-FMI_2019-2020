//
//  CommandMenu.cpp
//  homework1
//
//  Created by Desislava Doncheva on 3.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <ctime>
#include <fstream>
#include <stdlib.h>
#include "CommandMenu.hpp"
using namespace std;

CommandMenu::CommandMenu(TList<Wallet>* wl, TList<Transaction>* tl, TList<Order>* ol, char* ComplitedFileOrders)
{
    this->wl = wl;
    this->tl = tl;
    this->ol = ol;
    strcpy(this->ComplitedFileOrders, ComplitedFileOrders);
    
}
bool CommandMenu::processCommand (char* commandline)
{
    int i;
    bool validCommand = false;
    bool QuitCommand = false;
    char coppycommline[256];
    char arguments[MAX_COMMAND_ARG][256];
    
    strcpy(coppycommline, commandline);
    //cout << "You typed: " << commandline << endl;

    for(i = 0; i<MAX_COMMAND_ARG; i++)
    {
        strcpy(arguments[i], "");
    }
    
    // split command line to arguments
    char* token = strtok(coppycommline, " ");

    i = 0;
    while (token!=nullptr)
    {
        //printf("%s\n", token);
        strcpy(arguments[i++], token);
        token = strtok(nullptr, " ");
        if(i==MAX_COMMAND_ARG)
        {
            break;
        }
    }
    //commands
    if(strcmp(arguments[0], "add-wallet") == 0)
    {
        validCommand = true;
        double argMoney = 0;
        char argName[256] = "";
        argMoney = atof(arguments[1]);
        strcpy(argName, arguments[2]);
        add_wallet(argMoney, argName);
    }
    if(strcmp(arguments[0], "make-order") == 0)
    {
        validCommand = true;
        char argTypeStr[256];
        Order::Type argType;
        double argCoins;
        unsigned argWID;
        
        strcpy(argTypeStr, arguments[1]);
        for(i = 0; i < strlen(argTypeStr); i++)
        {
            argTypeStr[i] = toupper(argTypeStr[i]);
        }
        if(strcmp(argTypeStr, "SELL")==0 || strcmp(argTypeStr, "BUY")==0)
        {
            if(strcmp(argTypeStr, "SELL")==0)
            {
                argType = Order::SELL;
            }
            else
            {
                argType = Order::BUY;
            }
            argCoins = atof(arguments[2]);
            argWID = (unsigned int)atoll(arguments[3]);
            make_order(argType, argCoins, argWID);
        }
        else
        {
            cout << "Invalid Order type argument(SELL/BUY)" << endl;
        }
    }
    if(strcmp(arguments[0], "transfer") == 0)
    {
        validCommand = true;
        unsigned argSWID, argRWID;
        double argCoins;
        argSWID = (unsigned int)atoll(arguments[1]);
        argRWID = (unsigned int)atoll(arguments[2]);
        argCoins = atof(arguments[3]);
        transfer(argSWID, argRWID, argCoins);
    }
    if(strcmp(arguments[0], "wallet-info") == 0)
    {
        validCommand = true;
        unsigned argWID;
        argWID = (unsigned int)atoll(arguments[1]);
        wallet_info(argWID);
    }
    if(strcmp(arguments[0], "attract-investors") == 0)
    {
        validCommand = true;
        attract_investors();
    }
    
    //lists commands
    if(strcmp(arguments[0], "list-wallets") == 0)
    {
        validCommand = true;
        list_wallets();
    }
    if(strcmp(arguments[0], "list-transactions") == 0)
    {
        validCommand = true;
        list_transactions();
    }
    if(strcmp(arguments[0], "list-orders") == 0)
    {
        validCommand = true;
        list_orders();
    }
    //help command
    if(strcmp(arguments[0], "help") == 0 || strcmp(arguments[0], "?") == 0)
    {
        validCommand = true;
        help();
    }
    //quit command
    if(strcmp(arguments[0], "quit") == 0)
    {
        validCommand = true;
        quit();
        QuitCommand = true;
    }
    if(validCommand == false)
    {
        cout << "Invalid command, use 'help'" << endl;
    }
    return QuitCommand;
}

void CommandMenu::add_wallet(double money, char* name)
{
    if(money<=0)
    {
        cout << "Amount of Money must be > 0.00" << endl;
        return;
    }
    if(strcmp(name, "")==0)
    {
        cout << "Name of account's holder is missing" << endl;
        return;
    }
    Wallet newWallet;
    strcpy(newWallet.owner, name);
    newWallet.fiatMoney = money;
    newWallet.id = getMaxWalletId()+1;
    wl->add(newWallet);
    
    Transaction initialTransaction;
    time(&initialTransaction.time);
    initialTransaction.senderId = SYSTEM_WALLET_ID;
    initialTransaction.receiverId = newWallet.id;
    initialTransaction.fmiCoins = money/MONEY_COIN_RATE;
    tl->add(initialTransaction);
    
}
void CommandMenu::make_order(Order::Type typeOrder, double coins, unsigned wid)
{
    if(coins <= 0)
    {
        cout << "Invalid order amount, must be > 0" << endl;
    }
    if(wid <= 0)
    {
        cout << "Invalid WID!" << endl;
        return;
    }
    bool foundWid = false;
    unsigned i;
    Wallet Wtemp;
    for(i = 0; i < wl->getSize(); i++)
    {
        Wtemp = wl->get(i);
        if(Wtemp.id == wid)
        {
            foundWid = true;
            break;
        }
    }
    if(foundWid == false)
    {
        cout << "Didn't find WID!" << endl;
        return;
    }
    
    //otwarqm fajla
    //-------------
    unsigned counterLines = 0;
    ofstream outOrder;
    char NewComplitedFileOrders[256];
    time_t timenow;
    time(&timenow);
    sprintf(NewComplitedFileOrders, "%s_%ld_%d.txt", ComplitedFileOrders, timenow, wid);
    outOrder.open(NewComplitedFileOrders, ios::binary | ios::out);
    

    //-------------
    double sumCoins = 0;
    if(typeOrder == Order::SELL)
    {
        double accCoins = 0.0;
        accCoins = accountBalance(wid);
        if(accCoins < coins)
        {
            cout << "Not enough money in WID(" << fixed << accCoins << ") for SELL order!" << endl;
            return;
        }
        
        i = 0;
        bool isDeleted = false;
        Order Otemp;
        while(i < ol->getSize())
        {
            isDeleted = false;
            Otemp = ol->get(i);
            if(Otemp.type == Order::BUY)
            {
                if(coins < Otemp.fmiCoins)
                {
                    transfer(wid, Otemp.walletId, coins);
                    logOrder(&outOrder, wid, Otemp.walletId, coins);
                    sumCoins = sumCoins + coins;
                    counterLines++;
                    Otemp.fmiCoins = Otemp.fmiCoins - coins;
                    ol->update(i, Otemp);
                    coins = 0;
                    break;
                }
                if(coins == Otemp.fmiCoins)
                {
                    transfer(wid, Otemp.walletId, coins);
                    logOrder(&outOrder, wid, Otemp.walletId, coins);
                    sumCoins = sumCoins + coins;
                    counterLines++;
                    ol->del(i);
                    coins = 0;
                    isDeleted = true;
                    break;
                }
                if(coins > Otemp.fmiCoins)
                {
                    transfer(wid, Otemp.walletId, Otemp.fmiCoins);
                    logOrder(&outOrder, wid, Otemp.walletId, Otemp.fmiCoins);
                    sumCoins = sumCoins + Otemp.fmiCoins;
                    counterLines++;
                    ol->del(i);
                    isDeleted = true;
                    coins = coins - Otemp.fmiCoins;
                }
            }
            
            if(isDeleted == false)
            {
                i++;
            }
        }
        if(coins > 0)
        {
            Order ourRemainingOrder;
            ourRemainingOrder.type = Order::SELL;
            ourRemainingOrder.walletId = wid;
            ourRemainingOrder.fmiCoins = coins;
            ol->add(ourRemainingOrder);
        }
    }
    
    if(typeOrder == Order::BUY)
    {
        double accCoins = 0.0;
        accCoins = getWalletBalance(wid)/MONEY_COIN_RATE;
        if(accCoins < coins)
        {
            cout << "Not enough money in WID(" << fixed << accCoins << ") for BUY order!" << endl;
            return;
        }
        
        i = 0;
        bool isDeleted = false;
        Order Otemp;
        double successfulBuy = 0.0;
        while(i < ol->getSize())
        {
            isDeleted = false;
            Otemp = ol->get(i);
            if(Otemp.type == Order::SELL)
            {
                if(coins < Otemp.fmiCoins)
                {
                    transfer(Otemp.walletId, wid, coins);
                    logOrder(&outOrder, Otemp.walletId, wid, coins);
                    sumCoins = sumCoins + coins;
                    counterLines++;
                    successfulBuy = successfulBuy + coins;
                    Otemp.fmiCoins = Otemp.fmiCoins - coins;
                    ol->update(i, Otemp);
                    coins = 0;
                    break;
                }
                if(coins == Otemp.fmiCoins)
                {
                    transfer(Otemp.walletId, wid, coins);
                    logOrder(&outOrder, Otemp.walletId, wid, coins);
                    sumCoins = sumCoins + coins;
                    counterLines++;
                    successfulBuy = successfulBuy + coins;
                    ol->del(i);
                    coins = 0;
                    isDeleted = true;
                    break;
                }
                if(coins > Otemp.fmiCoins)
                {
                    transfer(Otemp.walletId, wid, Otemp.fmiCoins);
                    logOrder(&outOrder, Otemp.walletId, wid, Otemp.fmiCoins);
                    sumCoins = sumCoins + Otemp.fmiCoins;
                    counterLines++;
                    successfulBuy = successfulBuy + Otemp.fmiCoins;
                    ol->del(i);
                    isDeleted = true;
                    coins = coins - Otemp.fmiCoins;
                }
            }
            
            if(isDeleted == false)
            {
                i++;
            }
        }
        if(coins > 0)
        {
            Order ourRemainingOrder;
            ourRemainingOrder.type = Order::BUY;
            ourRemainingOrder.walletId = wid;
            ourRemainingOrder.fmiCoins = coins;
            ol->add(ourRemainingOrder);
        }
        if(successfulBuy > 0)
        {
            double wBalance = 0.0;
            wBalance = getWalletBalance(wid) - (successfulBuy * MONEY_COIN_RATE);
            setWalletBalance(wid, wBalance);
        }
    }
    //ztwariqm  fajla
    //----------------
    outOrder << "Total transfer number: " << counterLines << endl;
    outOrder << "Total FmiCoins transfered: " << fixed << sumCoins << " ("<< fixed << (sumCoins * MONEY_COIN_RATE) <<" lv)" << endl;
    if(outOrder.is_open())
    {
        outOrder.close();
    }
    //----------------
}
void CommandMenu::transfer(unsigned swid, unsigned rwid, double coins)
{
    unsigned i;
    if(swid <= 0)
    {
        cout << "Invalid SWID!" << endl;
        return;
    }
    if(rwid <= 0)
    {
        cout << "Invalid RWID!" << endl;
        return;
    }
    if(coins <= 0)
    {
        cout << "Invalid transfer amount, must be >0" << endl;
        return;
    }
    Wallet Wtemp;
    bool foundSwid = false;
    bool foundRwid = false;
    for(i = 0; i < wl->getSize(); i++)
    {
        Wtemp = wl->get(i);
        if(Wtemp.id == swid)
        {
            foundSwid = true;
        }
        if(Wtemp.id == rwid)
        {
            foundRwid = true;
        }
    }
    if(foundSwid == false)
    {
        cout << "Didn't find SWID ("<< swid <<")" << endl;
        return;
    }
    if(foundRwid == false)
    {
        cout << "Didn't find RWID ("<< rwid <<")" << endl;
        return;
    }
    if(swid == rwid)
    {
        cout << "SWID and RWID are same!" << endl;
        return;
    }
    double senderMoney = 0.0;
    senderMoney = accountBalance(swid);
    if(senderMoney < coins)
    {
        cout << "Not enough money in SWID(" << fixed << senderMoney << ")" << endl;
        return;
    }
    Transaction newTransaction;
    time(&newTransaction.time);
    newTransaction.senderId = swid;
    newTransaction.receiverId = rwid;
    newTransaction.fmiCoins = coins;
    tl->add(newTransaction);
    
    double swBalance = 0, rwBalance = 0;
    
    /*
    wBalance = getWalletBalance(swid) - (coins * MONEY_COIN_RATE);
    setWalletBalance(swid, swBalance);
    rwBalance = getWalletBalance(rwid) + (coins * MONEY_COIN_RATE);
    setWalletBalance(rwid, rwBalance);
     */
    
    //cout << "Transfer succesful!" << endl;
    
}
void CommandMenu::wallet_info(unsigned wid)
{
    unsigned i = 0;
    if(wid <= 0)
    {
        cout << "Invalid WID!" << endl;
        return;
    }
    bool foundWid = false;
    Wallet Wtemp;
    for(i = 0; i < wl->getSize(); i++)
    {
        Wtemp = wl->get(i);
        if(Wtemp.id == wid)
        {
            foundWid = true;
            break;
        }
    }
    if(foundWid == false)
    {
        cout << "Didn't find WID!" << endl;
        return;
    }
    cout << "Wallet " << wid <<":" << endl;
    cout << "owner: " << Wtemp.owner << endl;
    cout << "id: " << Wtemp.id << endl;
    cout << "fiatMoney: " << fixed << Wtemp.fiatMoney << endl;
    
    double Coinstemp = 0.0;
    Coinstemp = accountBalance(wid);
    
    cout << "FmiCoins: " << Coinstemp << endl;
    
}
void CommandMenu::attract_investors()
{
    unsigned i = 0, j = 0;
    WalletInfo* winfo = new(nothrow) WalletInfo[wl->getSize()];
    Wallet wtemp;
    WalletInfo witemp1, witemp2;
    if(winfo != nullptr)
    {
        for(i = 0; i < wl->getSize(); i++)
        {
            wtemp = wl->get(i);
            winfo[i].walletid = wtemp.id;
            strcpy(winfo[i].walletName, wtemp.owner);
            winfo[i].Coins = accountBalance(wtemp.id);
            winfo[i].numberTrans = numberTransactions(wtemp.id);
            winfo[i].firstTime = firstTimeTransaction(wtemp.id);
            winfo[i].lastTime = lastTimeTransaction(wtemp.id);
        }
        for(i = 0 ; i < wl->getSize()-1; i++)
        {
            for(j = i+1; j < wl->getSize(); j++)
            {
                witemp1 = winfo[i];
                witemp2 = winfo[j];
                if(witemp1.Coins < witemp2.Coins)
                {
                    winfo[i] = witemp2;
                    winfo[j] = witemp1;
                }
            }
        }
        for(i = 0 ; i < wl->getSize(); i++)
        {
            if(i==10)
            {
                break;
            }
            cout << "Attract" << i+1 << ":" << endl;
            cout << "WID: " << winfo[i].walletid << endl;
            cout << "Name: " << winfo[i].walletName << endl;
            cout << "FmiCoins: " << fixed << winfo[i].Coins << endl;
            cout << "Number Transactions: " << winfo[i].numberTrans << endl;
            cout << "First time: " << winfo[i].firstTime << endl;
            cout << "Last time: " << winfo[i].lastTime << endl;
            cout << endl;
        }
        delete [] winfo;
    }

}
void CommandMenu::list_wallets()
{
    cout << *wl;
}
void CommandMenu::list_transactions()
{
    cout << *tl;
}
void CommandMenu::list_orders()
{
    cout << *ol;
}
void CommandMenu::quit()
{
    cout << "Goodbye!" << endl;
}
void CommandMenu::help()
{
    cout << "add-wallet Money Name" << endl;
    cout << "make-order SELL/BUY Coins WID" << endl;
    cout << "transfer SWID RWID Coins" << endl;
    cout << "wallet-info WID" << endl;
    cout << "attract-investors" << endl;
    cout << "list-wallets" << endl;
    cout << "list-transactions" << endl;
    cout << "list-orders" << endl;
    cout << "quit" << endl;
    cout << "help" << endl;

}

unsigned CommandMenu::getMaxWalletId()
{
    unsigned maxId = 0;
    if(wl!=nullptr)
    {
        Wallet w;
        for(unsigned i = 0; i < wl->getSize(); i++)
        {
            w = wl->get(i);
            if(w.id > maxId and w.id != SYSTEM_WALLET_ID)
            {
                maxId = w.id;
            }
                
        }
    }
    return maxId;
}

double CommandMenu::accountBalance(unsigned wid)
{
    Transaction Ttemp;
    double Coinstemp = 0.0;
    unsigned i = 0;
    for(i = 0 ; i < tl->getSize(); i++)
    {
        Ttemp = tl->get(i);
        if(Ttemp.receiverId == wid)
        {
            Coinstemp = Coinstemp + Ttemp.fmiCoins;
        }
        if(Ttemp.senderId == wid)
        {
            Coinstemp = Coinstemp - Ttemp.fmiCoins;
        }
    }
    return Coinstemp;
}
unsigned CommandMenu::numberTransactions(unsigned wid)
{
    Transaction Ttemp;
    unsigned i = 0, number = 0;
    
    for(i = 0; i < tl->getSize(); i++)
    {
        Ttemp = tl->get(i);
        if(Ttemp.receiverId == wid or Ttemp.senderId == wid)
        {
            number++;
        }
    }
    return number;
}
time_t CommandMenu::firstTimeTransaction(unsigned wid)
{
    Transaction Ttemp;
    unsigned i = 0;
    time_t firstTime = 0;
    
    for(i = 0; i < tl->getSize(); i++)
    {
        Ttemp = tl->get(i);
        if(Ttemp.receiverId == wid or Ttemp.senderId == wid)
        {
            firstTime = Ttemp.time;
            break;
        }
    }
    return firstTime;
}
time_t CommandMenu::lastTimeTransaction(unsigned wid)
{
    Transaction Ttemp;
    unsigned i = 0;
    time_t lastTime = 0;
    
    for(i = 0; i < tl->getSize(); i++)
    {
        Ttemp = tl->get(i);
        if(Ttemp.receiverId == wid or Ttemp.senderId == wid)
        {
            lastTime = Ttemp.time;
        }
    }
    return lastTime;
}

double CommandMenu::getWalletBalance(unsigned wid)
{
    unsigned i;
    Wallet Wtemp;
    for(i = 0; i < wl->getSize(); i++)
    {
        Wtemp = wl->get(i);
        if(Wtemp.id == wid)
        {
            return Wtemp.fiatMoney;
        }
    }
    return 0;
}
bool CommandMenu::setWalletBalance(unsigned wid, double money)
{
    unsigned i;
    Wallet Wtemp;
    for(i = 0; i < wl->getSize(); i++)
    {
        Wtemp = wl->get(i);
        if(Wtemp.id == wid)
        {
            Wtemp.fiatMoney = money;
            wl->update(i, Wtemp);
            return true;
        }
    }
    return false;
    
    
}

void CommandMenu::logOrder(ofstream* logFileFs, unsigned walletid1, unsigned walletid2, double coins)
{
    unsigned i;
    Wallet wtemp;
    char name1[256]="", name2[256]="";
    for(i = 0; i < wl->getSize(); i++)
    {
        wtemp = wl->get(i);
        if(wtemp.id == walletid1)
        {
            strcpy(name1, wtemp.owner);
        }
        if(wtemp.id == walletid2)
        {
            strcpy(name2, wtemp.owner);
        }
    }
    *logFileFs << name1 << " => " << name2 << " " << fixed << coins << endl;
}

CommandMenu::~CommandMenu()
{
    
}
