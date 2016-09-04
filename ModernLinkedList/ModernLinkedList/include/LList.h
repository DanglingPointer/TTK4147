#ifndef LLIST_H
#define LLIST_H
// singly-linked list using C++11

#include <memory>
#include <iostream>

template <class TVal> struct Node
{
    typedef std::unique_ptr<Node<TVal>> Ptr_t;

    Node(TVal&& data)
        :Data(std::move(data)), pNext()
    { }
    Node(const TVal& data)
        :Data(data), pNext()
    { }

    TVal	Data;
    Ptr_t	pNext;
};

template <class TVal> class LinkedList
{
public:
    typedef typename Node<TVal>::Ptr_t  Ptr_t;
    typedef LinkedList<TVal>            My_t;

    LinkedList() :m_pHead(), m_length(0)
    { }
    LinkedList(My_t&& rhs) :m_pHead(std::move(rhs.m_pHead)),m_length(rhs.m_length)
    { }
    LinkedList(const My_t&)             = delete;
    My_t& operator=(const My_t& rhs)    = delete;
    My_t& operator=(My_t&& rhs)
    {
        m_pHead = std::move(rhs.m_pHead);
        m_length = rhs.m_length;
    }
    std::size_t get_Length() const
    {
        return m_length;
    }
    void Delete()
    {
        m_pHead.reset();
        m_length = 0;
    }
    template <class T>
    bool Insert(std::size_t index, T&& data)
    {
        if (index > m_length) return false;

        auto pnew = std::make_unique<Node<TVal>>(std::forward<T>(data));
        if (index == 0) {
            std::swap(pnew->pNext, m_pHead);
            m_pHead = std::move(pnew);
        }
        else {
            auto *pprev = m_pHead.get();
            for (std::size_t i = 0; i+1 < index; ++i) {
                pprev = (pprev->pNext).get();
            }
            std::swap(pnew->pNext, pprev->pNext);
            pprev->pNext = std::move(pnew);
        }
        ++m_length;
        return true;
    }
    template <class T>
    void Append(T&& data)
    {
        Insert(m_length, std::forward<T>(data));
    }
    void Print() const
    {
        for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = (pcurrent->pNext).get()) {
            std::cout << pcurrent->Data << " ";
        }
        std::cout << std::endl;
    }
    TVal GetSum() const; // only defined for int
    TVal get_At(std::size_t index) const
    {
        std::size_t i = 0;
        for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = (pcurrent->pNext).get()) {
            if (i++ == index)
                return pcurrent->Data;
        }
        return 0;
    }
    TVal Extract(std::size_t index)
    {
        if (m_length == 1) {
            Ptr_t ptemp;
            std::swap(m_pHead, ptemp);
            m_pHead.reset();
            return ptemp->Data;
        }
        if (index == 0) {
            auto pcurrent = m_pHead.get();
            std::swap(m_pHead, m_pHead->pNext); // pcurrent->pNext points to itself now
            Ptr_t ptemp(std::move(pcurrent->pNext));

            --m_length;
            return ptemp->Data;
        }
        else {
            auto pcurrent = m_pHead->pNext.get();
            auto pprev = m_pHead.get();

            for (std::size_t i = 1; i < index; ++i) {
                if (!pcurrent) return 0;
                pprev = pcurrent;
                pcurrent = (pcurrent->pNext).get();
            }
            std::swap(pprev->pNext, pcurrent->pNext); // pcurrent->pNext points to itself now
            Ptr_t ptemp(std::move(pcurrent->pNext));

            --m_length;
            return ptemp->Data;
        }
    }
private:
    Ptr_t       m_pHead;
    std::size_t m_length;
};

template<>
inline int LinkedList<int>::GetSum() const
{
    int sum = 0;
    for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = (pcurrent->pNext).get()) {
        sum += pcurrent->Data;
    }
    return sum;
}

#endif