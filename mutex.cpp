#include<thread>
#include<iostream>
#include<vector>
#include<mutex>
#include<cmath>

std::mutex transerMutex;
typedef unsigned long long ull; // Imagine our client store tons of money
class BankClient
{
    public:
        virtual void depositMoney(ull amount) = 0;
        virtual void withdrawMoney(ull amount) = 0;
    private:
        ull moneyAmount_;
};
class GoodBankClient : public BankClient // 'Good' implementation with mutex
{
    public:
        GoodBankClient(ull moneyAmount) : moneyAmount_(moneyAmount) {}
        void depositMoney(ull amount)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for(size_t i = 0;i<amount;i++) // some 300 IQ coding, so difference would be more noticeable
            {
                moneyAmount_ += 1;
            }
        }
        void withdrawMoney(ull amount)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(amount < moneyAmount_)
            {
                for(size_t i = 0;i<amount;i++) // some 300 IQ coding, so difference would be more noticeable
                {
                    moneyAmount_ -= 1;
                }
            }
            else
            {
                std::cout << "No money, you are broke" << std::endl;
            }
        }
        ull getMoney(){return moneyAmount_;}   
    private:
        ull moneyAmount_;
        std::mutex mutex_;
};
class BadBankClient : public BankClient // Bad implementation without std::mutex, with data race
{
    public:
        BadBankClient(ull moneyAmount) : moneyAmount_(moneyAmount) {}
        void depositMoney(ull amount)
        {
            for(size_t i = 0;i<amount;i++) // some 300 IQ coding, so difference would be more noticeable
            {
                moneyAmount_ += 1;
            }
        }
        void withdrawMoney(ull amount)
        {
            if(amount < moneyAmount_)
            {
                for(size_t i = 0;i<amount;i++) // some 300 IQ coding, so difference would be more noticeable
                {
                    moneyAmount_ -= 1;
                }
            }
            else
            {
                std::cout << "No money, you are broke" << std::endl;
            }
        }
        ull getMoney(){return moneyAmount_;}
    private:
        ull moneyAmount_;
};
void transactions(BankClient& client) // in the end add 1 to balance
{
    client.depositMoney(10000);
    client.withdrawMoney(9999);
}
int main()
{
    int transactionsAmount = 100; // amount of transaction, the more amount, the easire to see diffrence between bad and good clients
    ull goodClientStart = 100;
    ull badClientStart = 0;
    GoodBankClient ElonMusk(goodClientStart); // good client, expected amount in the end goodClientStart + transactionsAmount
    BadBankClient JeffBezos(badClientStart); // bad client, expected amount in the end badClientStart + transactionsAmount
    std::vector<std::thread> v(transactionsAmount);
    std::vector<std::thread> j(transactionsAmount);
    // Good client transactions
    for(size_t i = 0;i<transactionsAmount;i++)
    {
        v.at(i) = std::thread(transactions,std::ref(ElonMusk));
    }
    for(auto &i:v)
    {
        i.join();
    }
    // Bad client transactions
    for(size_t i = 0;i<transactionsAmount;i++)
    {
        j.at(i) = std::thread(transactions,std::ref(JeffBezos));
    }
    for(auto &i:j)
    {
        i.join();
    }
    ull goodClientMoney = ElonMusk.getMoney();
    if(goodClientMoney == goodClientStart + transactionsAmount)
    {
        std::cout << "Good client have expected amount of money: " << goodClientMoney << std::endl;
    }
    else
    {
        std::cout << "What have you done to my code???!!!!" << std::endl;
    }
    ull badClientMoney = JeffBezos.getMoney();
    if(badClientMoney == badClientStart + transactionsAmount)
    {
        std::cout << "You are very lucky, if you read this. Bad client has expected amount of money " << badClientMoney << std::endl;
    }
    else
    {
        std::cout << "Bad client have problem(s) with transactions. Bad client has " << JeffBezos.getMoney() << std::endl;
    }
}