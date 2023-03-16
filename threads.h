#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <iostream>
#include <functional>
#include <future>

enum class operator_value
{
    operator_success,
    operator_failure
};


class task
{
public:
    task(){};
    template <typename F, typename... Args>
    auto add_task(F&& f, Args&& ...args)
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        svc = std::function([task_ptr]{
            (*task_ptr)();
        });
        return task_ptr->get_future();
    }
    void process()
    {
        svc();
    }
private:
    std::function<void()> svc;
};


// template <typename F, typename... args>
class thread_pool
{
public:
    thread_pool()
    {
        thread_num_ = 5;
        stop = false;
    };
    thread_pool(int&  num)
    {
        thread_num_ = num;
        stop = false;
    }
    void vreat_threads(int& num)
    {
        for(auto i = 0; i < thread_num_; i++)
        {
            thread_array.push_back(std::thread([&]{sing_thread();}));
        }
    }
    void sing_thread()
    {
        // optimize the lock scope
        while(!stop)
        {
            std::unique_lock<std::mutex>lock(mutex_);
            while(task_queue_.empty())
            {
                semaphore.wait(lock, [&]{return task_queue_.empty();});
            }
            auto cur_task = std::move(task_queue_.front());
            task_queue_.pop();
            cur_task.process();
            semaphore.notify_one();
        }
    }
    auto thread_stop()
    {
        stop = true;
        semaphore.notify_all();
        for(auto& thread:thread_array)
        {
            thread.join();
        }
        return operator_value::operator_success;
    }
    auto thread_start()
    {
        vreat_threads(thread_num_);
    }
    auto push_task(task&& cur_task )
    {
        std::unique_lock<std::mutex>lock(mutex_);
        task_queue_.push(std::move(cur_task));
        semaphore.notify_one();
        return operator_value::operator_success;
    }

private:
    std::atomic<bool> stop;
    std::mutex mutex_;
    std::condition_variable semaphore;
    int thread_num_;
    std::queue<task>task_queue_;
    std::vector<std::thread>thread_array;
};