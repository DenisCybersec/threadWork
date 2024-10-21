#include<iostream>
#include<thread>
void helloWorld()
{
    std::cout << "Hello World" << std::endl;
}
int main()
{
    std::thread th(helloWorld);
    if(th.joinable())
    {
        th.join();
    }
}