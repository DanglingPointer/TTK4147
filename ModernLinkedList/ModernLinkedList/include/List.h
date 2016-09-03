#pragma once
#include <memory>
#include <iostream>

template <class TVal> struct Node
{
    typedef std::shared_ptr<Node<TVal>> Ptr_t;

    Node(TVal&& data)
        :Data(std::move(data)), pPrev(), pNext()
    { }
    Node(const TVal& data)
        :Data(data), pPrev(), pNext()
    { }

    TVal	Data;
    Ptr_t	pPrev;
    Ptr_t	pNext;
};

template <class TVal> class LinkedList
{
public:
    typedef typename Node<TVal>::Ptr_t Ptr_t;
    typedef LinkedList<TVal> My_t;

    LinkedList() :m_pHead(), m_pTail(), m_length(0)
    { }
    LinkedList(My_t&& rhs) :m_pHead(std::move(rhs.m_pHead)), m_pTail(std::move(rhs.m_pTail)), m_length(rhs.m_length)
    { }
    LinkedList(const My_t&)             = delete;
    My_t& operator=(const My_t& rhs)    = delete;
    My_t& operator=(My_t&& rhs)
    {
        m_pHead = std::move(rhs.m_pHead);
        m_pTail = std::move(rhs.m_pTail);
        m_length = rhs.m_length;
    }
    std::size_t get_Length() const
    {
        return m_length;
    }
    void Delete()
    {
        m_pHead.reset();
        m_pTail.reset();
        m_length = 0;
    }
    template <class T>
    bool Insert(std::size_t index, T&& data)
    {
        if (index > m_length) return false;

        auto pnew = std::make_shared<Node<TVal>>(std::forward<T>(data));
        if (index == 0) {
            if (m_pHead) {
                m_pHead->pPrev = pnew;
                pnew->pNext = m_pHead;
            }
            else {
                m_pTail = pnew;
            }
            m_pHead = pnew;
        }
        else if (index == m_length) {
            pnew->pPrev = m_pTail;
            m_pTail->pNext = pnew;
            m_pTail = pnew;
        }
        else {
            auto pnext = m_pHead;
            for (decltype(index) i = 0; (i < index) && pnext->pNext; ++i)
                pnext = pnext->pNext;
            auto pprev = pnext->pPrev;

            pprev->pNext = pnew;
            pnext->pPrev = pnew;

            pnew->pPrev = pprev;
            pnew->pNext = pnext;
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
        for (auto pcurrent = m_pHead; pcurrent; pcurrent = pcurrent->pNext) {
            std::cout << pcurrent->Data << " ";
        }
        std::cout << std::endl;
    }
    TVal GetSum() const; // only defined for int
    TVal get_At(std::size_t index) const
    {
        std::size_t i = 0;
        for (auto pcurrent = m_pHead; pcurrent; pcurrent = pcurrent->pNext) {
            if (i++ == index)
                return pcurrent->Data;
        }
        return 0;
    }
    TVal Extract(std::size_t index)
    {
        Ptr_t pcurrent = m_pHead;
        for (std::size_t i = 0; i < index; ++i) {
            if (!pcurrent) return 0;
            pcurrent = pcurrent->pNext;
        }
        if (pcurrent->pPrev) {
            auto pprev = pcurrent->pPrev;
            pprev->pNext = pcurrent->pNext;
        }
        if (pcurrent->pNext) {
            auto pnext = pcurrent->pNext;
            pnext->pPrev = pcurrent->pPrev;
        }
        if (pcurrent == m_pHead)
            m_pHead = pcurrent->pNext;
        if (pcurrent == m_pTail)
            m_pTail = pcurrent->pPrev;

        --m_length;
        return pcurrent->Data;
    }
private:
    Ptr_t m_pHead;
    Ptr_t m_pTail;
    std::size_t m_length;
};

template<>
inline int LinkedList<int>::GetSum() const
{
    int sum = 0;
    for (auto pcurrent = m_pHead; pcurrent; pcurrent = pcurrent->pNext) {
        sum += pcurrent->Data;
    }
    return sum;
}