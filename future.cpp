#include<iostream>
#include<vector>
#include<thread>
#include<chrono>
#include<future>
typedef unsigned long long ull;
void calculate(ull start, ull end, std::promise<ull>& prom)
{
    ull summ = 0;
    for(;start<end;start++)
    {
        summ+=start;
    }
    prom.set_value(summ);
}
ull soloCalculate(ull start, ull end)
{
    ull summ = 0;
    for(;start<end;start++)
    {
        summ+=start;
    }
    return summ;
}
int main()
{
    int threadAmount = std::thread::hardware_concurrency();
    std::vector<std::promise<ull>> promises(threadAmount);
    ull begin = 0;
    ull end = 1234567890;
    std::vector<ull> breakPoints(threadAmount + 1);
    size_t delta = end - begin;
    size_t step = delta/threadAmount;
    for(size_t i = 0; i < threadAmount;i++)
    {
        breakPoints.at(i) = begin + step*i;
    }
    breakPoints.at(threadAmount) = end;
    std::vector<std::future<ull>> futures(threadAmount);
    for(size_t i = 0;i<futures.size();i++)
    {
        futures.at(i) = promises.at(i).get_future();
    }
    std::vector<std::thread> threads(threadAmount);
    for(size_t i = 0;i<threadAmount;i++)
    {
        threads.at(i) = std::thread(calculate,breakPoints.at(i),breakPoints.at(i+1),std::ref(promises.at(i)));
    }
    std::chrono::time_point multithreadCalculationStart = std::chrono::system_clock::now();
    for(auto &i:threads)
    {
        if(i.joinable())
        {
            i.join();
        }
    }
    std::chrono::time_point multithreadCalculationEnd = std::chrono::system_clock::now();
    std::chrono::duration diff = multithreadCalculationEnd - multithreadCalculationStart;
    std::cout << "Time for multithread calc " << diff.count() << std::endl;
    for(auto &i: futures)
    {
        std::cout << i.get() << std::endl;
    }
    std::chrono::time_point solohreadCalculationStart = std::chrono::system_clock::now();
    std::thread th(soloCalculate,begin,end);
    th.join();
    std::chrono::time_point solohreadCalculationEnd = std::chrono::system_clock::now();
    std::chrono::duration soloDiff = solohreadCalculationEnd - solohreadCalculationStart;
    std::cout << "Time for solothread calc " << soloDiff.count() << std::endl;
    std::cout << "Multithread calculation is " <<  double(soloDiff.count())/double(diff.count()) << " times faster" << std::endl;
}