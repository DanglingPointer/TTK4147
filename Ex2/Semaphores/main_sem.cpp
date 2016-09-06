/*
 * Compile with:
 * g++-4.9 -std=gnu++14 -pthread main_sem.cpp -o Semaphores 
 * 
 * Run with:
 * ./Semaphores
 * 
 */
#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <memory>

class Semaphore
{
public:
    explicit Semaphore(int maxcount) : m_count(maxcount), m_mutex(), m_cv()
    {
    }
    Semaphore(const Semaphore&) = delete;
    int get_Count() const noexcept
    {
        return m_count;
    }
    void Signal()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        ++m_count;
        m_cv.notify_one();
    }
    void Wait()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_cv.wait(lk, [this]() {
            return !(m_count == 0);
        });
        --m_count;
    }
private:
    int m_count;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};
void foo(Semaphore* sem, int id)
{
    using namespace std::chrono_literals;
    std::cout << "Thread " << id << " is waiting" << std::endl;
    sem->Wait();
    std::cout << "Thread " << id << " is doing work" << std::endl;
    std::this_thread::sleep_for(2s);
    std::cout << "Thread " << id << " is finished" << std::endl;
    sem->Signal();
}
int main(int argc, char** argv)
{
    Semaphore sem(3);

    std::unique_ptr<std::thread> threads[5];
    
    int i = 1;
    for (auto& ptr : threads)
        ptr = std::make_unique<std::thread>(foo, &sem, i++);

    for(auto& ptr : threads)
        ptr->join();
    return 0;
}