#include<iostream>
#include<vector>
#include<thread>
#include<condition_variable>
#include<chrono>
std::mutex mtx;
void threadStart(int id,bool& wait,std::condition_variable& cond)
{
    std::unique_lock<std::mutex> tempMutex(mtx);
    std::cout << "Thread started " << std::endl;
    cond.wait(tempMutex,[&wait]{
        return wait;
    });
    std::cout << "Thread with id " << id << " ended" << std::endl;
}
void threadExecute(bool& change,std::condition_variable& cond)
{
    std::unique_lock<std::mutex> tempMutex(mtx);
    change = true;
    cond.notify_one();
    std::cout << "Notify made" << std::endl;
}
int main()
{
    size_t threadAmount = 20;
    std::vector<bool*> boolVars(threadAmount);
    for(size_t i = 0;i<threadAmount;i++)
    {
        boolVars.at(i) = new bool(false);
    }
    std::vector<std::thread> threads(threadAmount);
    std::vector<std::unique_ptr<std::condition_variable>> condVars(threadAmount);
    for(size_t i = 0;i<threadAmount;i++)
    {
        condVars.at(i) = std::make_unique<std::condition_variable>();
    }
    for(size_t i = 0;i<threadAmount;i++)
    {
        threads.at(i) = std::thread(threadStart,i,std::ref(*boolVars.at(i)),std::ref(*condVars.at(i)));
    }
    std::cout << "Threads created and wait" << std::endl;
    for(size_t i = 0;i<threadAmount;i++)
    {
        threadExecute(*boolVars.at(i),*condVars.at(i));
        threads.at(i).join();
    }
}