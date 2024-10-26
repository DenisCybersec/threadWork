#include<iostream>
#include<thread>
#include<atomic>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<functional>
class ThreadPool
{
    public:
        ThreadPool(size_t thread_num = std::thread::hardware_concurrency()) : thread_num_(thread_num){
            for(size_t i = 0;i<thread_num_;i++)
            {
                vctr_.emplace_back([this]
                {
                    while(true)
                    {
                        std::function<void()> fc;
                        {
                            std::unique_lock<std::mutex> lock(mtx_);
                            cv_.wait(lock,[this]
                            {
                                return ! queue_.empty() || stop_;
                            });
                            if (stop_ && queue_.empty()) {
                                return;
                            }

                            fc = std::move(queue_.front());
                            queue_.pop();
                        }
                        fc();
                    }
                });
            }
        };
        void enqueue(std::function<void()> fc)
        {
            {
                std::unique_lock<std::mutex> lock(mtx_);
                queue_.emplace(fc);
            }
            cv_.notify_one();
        }
        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(mtx_);
                stop_ = true;
            }
                cv_.notify_all(); // Notify all threads to wake up and exit
                for(std::thread &worker : vctr_) {
                    if (worker.joinable()){
                        worker.join(); // Wait for all threads to finish
                    }
                }
            
}
    private:
        bool stop_ = false;
        size_t thread_num_;
        std::mutex mtx_;
        std::condition_variable cv_;
        std::vector<std::thread> vctr_;
        std::queue<std::function<void()>> queue_;
};