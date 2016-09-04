#ifndef DLLIST_H
#define DLLIST_H
// Doubly-linked list using C++11

#include <memory>
#include <iostream>

template <class TVal> struct Node
{
    typedef std::shared_ptr<Node<TVal>> SPtr_t;
    typedef std::weak_ptr<Node<TVal>>   WPtr_t;

    Node(TVal&& data)
        :Data(std::move(data)), pPrev(), pNext()
    { }
    Node(const TVal& data)
        :Data(data), pPrev(), pNext()
    { }

    TVal	Data;
    WPtr_t	pPrev;
    SPtr_t	pNext;
};

template <class TVal> class DLinkedList
{
public:
    typedef typename Node<TVal>::SPtr_t SPtr_t;
    typedef typename Node<TVal>::WPtr_t WPtr_t;
    typedef DLinkedList<TVal>           My_t;

    DLinkedList() :m_pHead(), m_pTail(), m_length(0)
    { }
    DLinkedList(My_t&& rhs) :m_pHead(std::move(rhs.m_pHead)), m_pTail(std::move(rhs.m_pTail)), m_length(rhs.m_length)
    { }
    DLinkedList(const My_t&)            = delete;
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
            m_pHead = std::move(pnew);
        }
        else if (index == m_length) {
            pnew->pPrev = m_pTail;
            m_pTail->pNext = pnew;
            m_pTail = std::move(pnew);
        }
        else {
            SPtr_t pnext = m_pHead;
            for (decltype(index) i = 0; (i < index) && pnext->pNext; ++i)
                pnext = pnext->pNext;
            WPtr_t pprev = pnext->pPrev;
            
            pnew->pPrev = pprev;
            pnew->pNext = pnext;

            pnext->pPrev = pnew;
            (pprev.lock())->pNext = std::move(pnew);
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
        for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = pcurrent->pNext.get()) {
            std::cout << pcurrent->Data << " ";
        }
        std::cout << std::endl;
    }
    TVal GetSum() const; // only defined for int
    TVal get_At(std::size_t index) const
    {
        std::size_t i = 0;
        for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = pcurrent->pNext.get()) {
            if (i++ == index)
                return pcurrent->Data;
        }
        return 0;
    }
    TVal Extract(std::size_t index)
    {
        SPtr_t pcurrent = m_pHead;
        for (std::size_t i = 0; i < index; ++i) {
            if (!pcurrent) return 0;
            pcurrent = pcurrent->pNext;
        }
        SPtr_t pprev = std::move(pcurrent->pPrev.lock());
        SPtr_t pnext = std::move(pcurrent->pNext);

        if (pprev) {
            pprev->pNext = pnext;
        }
        if (pnext) {
            pnext->pPrev = std::move(pcurrent->pPrev);
        }
        if (pcurrent == m_pHead) {
            m_pHead = std::move(pnext);
        }
        if (pcurrent == m_pTail) {
            m_pTail = std::move(pprev);
        }
        --m_length;
        return pcurrent->Data;
    }
private:
    SPtr_t      m_pHead;
    SPtr_t      m_pTail;
    std::size_t m_length;
};

template<>
inline int DLinkedList<int>::GetSum() const
{
    int sum = 0;
    for (auto pcurrent = m_pHead.get(); pcurrent; pcurrent = pcurrent->pNext.get()) {
        sum += pcurrent->Data;
    }
    return sum;
}

#endif