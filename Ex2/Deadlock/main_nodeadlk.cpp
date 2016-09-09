/*
 * Compile with:
 * g++-4.9 -std=gnu++14 -pthread main_nodeadlk.cpp -o NoDeadlock
 * 
 * Run with:
 * ./NoDeadlock
 * 
 */

#include <iostream>
#include <mutex>
#include <memory>
#include <thread>
#include <chrono>

#define FORK_COUNT 3
#define PHIL_COUNT FORK_COUNT

class ForkPair
{
public:
    ForkPair(int leftIndex, int rightIndex, std::mutex *pforks)
    : m_pfirstFork(&pforks[leftIndex]), m_psecondFork(&pforks[rightIndex])
    {
        if (leftIndex > rightIndex)
            std::swap(m_pfirstFork, m_psecondFork);
    }
    void PickUp()
    {
        m_pfirstFork->lock();
        m_psecondFork->lock();
    }
    void PutDown()
    {
        m_pfirstFork->unlock();
        m_psecondFork->unlock();
    }
private:
    std::mutex *m_pfirstFork;
    std::mutex *m_psecondFork;
};

class Philosofer
{
public:
    Philosofer(int index, std::mutex *pforks) : m_index(index), m_pforks(pforks)
    {
    }
    void Run()
    {
        auto leftForkIndex = m_index;
        auto rightForkIndex = (m_index + 1) % PHIL_COUNT;
        ForkPair fp(leftForkIndex, rightForkIndex, m_pforks);

        for (;;) {
            fp.PickUp();
            std::cout << "Philosofer " << m_index << " eating\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            fp.PutDown();
	    std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
private:
    int m_index;
    std::mutex *m_pforks;
};
int main()
{
    std::mutex forks[FORK_COUNT];
    std::unique_ptr<std::thread> threads[PHIL_COUNT];
    std::unique_ptr<Philosofer> phils[PHIL_COUNT];

    for (int i = 0; i < PHIL_COUNT; ++i) {
        phils[i] = std::make_unique<Philosofer>(i, forks);
    }
    for (int i = 0; i < PHIL_COUNT; ++i) {
        threads[i] = std::make_unique<std::thread>([&phils, i]() {
            phils[i]->Run();
        });
    }
    for (int i = 0; i < PHIL_COUNT; ++i) {
        threads[i]->join();
    }

    return 0;
}
