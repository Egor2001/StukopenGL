#ifndef SGL_CTHREADPOOL_H
#define SGL_CTHREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>

//namespace sgl {

//TODO: rewrite with templates
class CThreadPool
{
public:
    using SFunctor = std::function<void()>;

    explicit CThreadPool(size_t thread_cnt);

    CThreadPool             (const CThreadPool&) = delete;
    CThreadPool& operator = (const CThreadPool&) = delete;
    CThreadPool             (CThreadPool&&);
    CThreadPool& operator = (CThreadPool&&);

    ~CThreadPool();

    void push_task(const SFunctor& task);
    void push_task(SFunctor&& task);

    //TODO: consider to implement full join inside the destructor
    void full_join();

private:
    //TODO: use unique_lock on it (read about locks and others on habr)
    std::mutex              mutex_;
    std::condition_variable cond_var_;

    std::vector<std::thread> thread_vec_;
    std::queue<SFunctor>     func_queue_; 
};
    
CThreadPool::CThreadPool(size_t thread_cnt):
    mutex_(),
    cond_var_(),
    thread_vec_(thread_cnt),
    func_queue_()
{
    auto thread_func = [this]//() -> void
    {
        SFunctor work_func;

        //TODO: to implement the condition to stop working
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_var_.wait(lock, [this]/*() -> bool*/
                                     { return !func_queue_.empty(); });

                work_func = std::move(func_queue_.front());
                func_queue_.pop();
            }

            work_func();
        }
    };
    
    for (auto& thrd : thread_vec_)
    {
        thrd = std::thread(std::move(thread_func));
        //TODO: consider another way to keep threads on 
        thrd.detach();
    }
}

CThreadPool::CThreadPool(CThreadPool&& move_pool):
    mutex_     (std::move(move_pool.mutex_)),
    cond_var_  (std::move(move_pool.cond_var_)),
    thread_vec_(std::move(move_pool.thread_vec_)),
    func_queue_(std::move(move_pool.func_queue_))
{}

CThreadPool& CThreadPool::operator = (CThreadPool&& move_pool)
{
    std::swap(mutex_,      move_pool.mutex_);
    std::swap(cond_var_,   move_pool.cond_var_);
    std::swap(thread_vec_, move_pool.thread_vec_);
    std::swap(func_queue_, move_pool.func_queue_);

    return *this;
}

CThreadPool::~CThreadPool()
{
    //TODO: consider not to wait in destructor
    //full_join();
    thread_vec_.clear();
}

void CThreadPool::push_task(const SFunctor& task)
{
    func_queue_.push_back_(task);
}

void CThreadPool::push_task(SFunctor&& task)
{
    func_queue_.push_back_(std::move(task));
}

void CThreadPool::full_join()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var.wait(lock, [this]/*() -> bool*/{ return func_queue_.empty(); });
}

//} //namespace sgl

#endif //SGL_CTHREADPOOL_H
