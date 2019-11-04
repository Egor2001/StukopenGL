#ifndef SGL_CLATCH_H
#define SGL_CLATCH_H

//general
#include <atomic>

//testing
#include <cstdio>
#include <thread>

//namespace sgl {

//namespace parallel { 

//implements concurrency TS latch interface
struct CLatch
{
public:
    explicit CLatch(ptrdiff_t value);

    CLatch             (const CLatch&) = delete;
    CLatch& operator = (const CLatch&) = delete;

    ~CLatch();

    void count_down_and_wait();
    void count_down(ptrdiff_t n = 1);
    bool is_ready() const noexcept;
    void wait() const;

private:
    std::atomic_ptrdiff_t atomic_value_;
};

CLatch::CLatch(ptrdiff_t value):
    atomic_value_(value)
{}

CLatch::~CLatch()
{
    atomic_value_.store(0);
}

void CLatch::count_down_and_wait()
{
    //TODO: to implement it more effectively, if possible
    count_down();
    wait();
}

void CLatch::count_down(ptrdiff_t n)
{
    //TODO: concider to use instead:
    atomic_value_.fetch_sub(n);

    //TODO: to set proper memory order for load and store
    /*
    ptrdiff_t new_val = 
        std::max(ptrdiff_t(0), 
                 atomic_value_.load(std::memory_order_acquire) - n);
    atomic_value_.store(new_val, std::memory_order_release);
    */
}

bool CLatch::is_ready() const noexcept
{
    //TODO: to set proper memory order for load
    ptrdiff_t cur_val = atomic_value_.load(/*std::memory_order_acquire*/); 
    return (cur_val == 0);
}

void CLatch::wait() const
{
    //TODO: to implement less CPU - expensive wait
    while (!is_ready())
        ;
}

int test_CLatch()
{
    const size_t THREAD_CNT = 17;
    std::thread thread_arr[THREAD_CNT];
    size_t data_arr[THREAD_CNT];
    size_t temp_arr[THREAD_CNT];

    CLatch latch{ THREAD_CNT };
    for (size_t i = 0; i < THREAD_CNT; ++i)
    {
        data_arr[i] = 0;
        thread_arr[i] = 
            std::thread([&latch, i, &data_arr, &temp_arr] 
            { 
                temp_arr[i] = (data_arr[i] += i); 
                latch.count_down_and_wait();
                data_arr[i] += temp_arr[THREAD_CNT - (i + 1)]; 
            });
    }
    latch.wait();

    for (size_t i = 0; i < THREAD_CNT; ++i)
        thread_arr[i].join();

    for (size_t i = 0; i < THREAD_CNT; ++i)
        printf("data[%lu] = %lu\n", i, data_arr[i]);

    return 0;
}

//} //namespace parallel 

//} //namespace sgl

#endif //SGL_CLATCH_H
