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

    CThreadPool();
    CThreadPool(size_t thread_cnt);

    CThreadPool             (const CThreadPool&) = delete;
    CThreadPool& operator = (const CThreadPool&) = delete;
    CThreadPool             (CThreadPool&&);
    CThreadPool& operator = (CThreadPool&&);

    ~CThreadPool();

    void push_task(const SFunctor& task);
    void push_task(SFunctor&& task);

//
//    void full_join();

private:
    //TODO: use unique_lock on it (read about locks and others on habr)
    std::mutex              mutex_;
    std::condition_variable cond_var_;

    std::vector<std::thread> thread_vec_;
    std::queue<SFunctor>     func_queue_; 
};

CThreadPool::CThreadPool():
    thread_vec_(),
    func_queue_()
{}
    
CThreadPool::CThreadPool(size_t thread_cnt);

CThreadPool::CThreadPool(CThreadPool&&);
CThreadPool& CThreadPool::operator = (CThreadPool&&);

CThreadPool::~CThreadPool();

void CThreadPool::push_task(const SFunctor& task);
void CThreadPool::push_task(SFunctor&& task);

//} //namespace sgl

#endif //SGL_CTHREADPOOL_H
