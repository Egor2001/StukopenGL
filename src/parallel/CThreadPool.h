#ifndef SGL_CTHREADPOOL_H
#define SGL_CTHREADPOOL_H

//general
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

//testing
#include <cstdio>
#include <cstdint>

//namespace sgl {

//namespace parallel {

//TODO: rewrite with templates
class CThreadPool
{
public:
    using SFunctor = std::function<void(size_t)>;

    explicit CThreadPool(size_t thread_cnt);

    CThreadPool             (const CThreadPool&) = delete;
    CThreadPool& operator = (const CThreadPool&) = delete;

    ~CThreadPool();

    [[nodiscard]]
    size_t thread_cnt() const noexcept
    {
        return thread_vec_.size(); 
    }

    void push_task(const SFunctor& task);
    void push_task(SFunctor&& task);

private:
    //init flag to false
    bool terminate_flag_; 

    std::mutex              mutex_;
    std::condition_variable cond_var_;

    std::vector<std::thread> thread_vec_;
    std::queue<SFunctor>     func_queue_; 
};
    
CThreadPool::CThreadPool(size_t thread_cnt):
    terminate_flag_(false),
    mutex_(),
    cond_var_(),
    thread_vec_(),
    func_queue_()
{
    auto thread_func = [this](size_t thread_index)/* -> void*/
    {
        //will cause bad_call exception in case of not being initialized
        SFunctor work_func;

        //quit loop via break on terminate flag inside a critical section
        while (true)
        {
            //anonymous block is used for RAII within unique_lock
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_var_.wait(lock, [this]() -> bool
                { 
                    return !func_queue_.empty() ||
                            terminate_flag_; 
                });

                //quit the loop on terminate flag after processing all tasks 
                if (terminate_flag_ && func_queue_.empty())
                    break;

                work_func = std::move(func_queue_.front());
                func_queue_.pop();
            }

            work_func(thread_index);
        }
    };

    thread_vec_.reserve(thread_cnt);
    for (size_t i = 0; i < thread_cnt; ++i)
        thread_vec_.emplace_back(std::move(thread_func), i);
}

CThreadPool::~CThreadPool()
{
    //anonymous block is used for RAII within unique_lock
    {
        std::unique_lock<std::mutex> lock(mutex_);
        terminate_flag_ = true;
    }

    cond_var_.notify_all();

    for (auto& thrd : thread_vec_)
        thrd.join();
}

void CThreadPool::push_task(const SFunctor& task)
{
    //anonymous block is used for RAII within unique_lock
    {
        std::unique_lock<std::mutex> lock(mutex_);
        func_queue_.push(task);
    }

    cond_var_.notify_one();
}

void CThreadPool::push_task(SFunctor&& task)
{
    //anonymous block is used for RAII within unique_lock
    {
        std::unique_lock<std::mutex> lock(mutex_);
        func_queue_.push(std::move(task));
    }

    cond_var_.notify_one();
}

int test_CThreadPool()
{
    const size_t ARR_SIZE = 32;
    const size_t POOL_SIZE = 8;

    uint8_t var[ARR_SIZE] = {};

    //anonymous block for RAII within thread pool
    {
        CThreadPool thread_pool(POOL_SIZE);

        for (size_t i = 0; i < ARR_SIZE; ++i)
            thread_pool.push_task([&var, i](size_t n){ ++var[i]; ++var[n]; });
    }
    
    for (size_t i = 0; i < ARR_SIZE; ++i)
        printf("%u ", var[i]);

    return 0;
}

//} //namespace parallel 

//} //namespace sgl

#endif //SGL_CTHREADPOOL_H
