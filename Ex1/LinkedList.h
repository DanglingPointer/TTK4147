//
// Created by student on 9/2/16.
//

#ifndef ASSB_LINKEDLIST_H
#define ASSB_LINKEDLIST_H

#include <cstdlib>
#include <iostream>

struct Node
{
    Node(Node *pprev, Node *pnext, int data):pNext(pnext), pPrev(pprev), data(data)
    {}
    Node *pNext;
    Node *pPrev;
    int data;
};

class LinkedList
{
public:
    LinkedList():m_pHead(nullptr), m_pTail(nullptr), m_length(0)
    {}
    std::size_t get_Length() const
    {
        return m_length;
    }
    void Delete()
    {
        for(Node *pcurrent = m_pHead; pcurrent != nullptr; ){
            Node *pnext = pcurrent->pNext;
            delete pcurrent;
            pcurrent = pnext;
        }
        m_pHead=m_pTail= nullptr;
        m_length=0;
    }
    bool Insert(std::size_t index, int data)
    {
        if (m_length < index)
            return false;

        if (index == 0){
            Node *pnew = new Node(nullptr, m_pHead, data);
            if (m_pHead)
                m_pHead->pPrev = pnew;
            m_pHead = pnew;
            if (m_length == 0)
                m_pTail = m_pHead;
        }
        else if (index == m_length){
            Node *pnew = new Node(m_pTail, nullptr, data);
            if (m_pTail)
                m_pTail->pNext = pnew;
            m_pTail = pnew;
        }
        else {
            Node *pnext = m_pHead;
            for (std::size_t i = 0; i < index; ++i) {
                pnext = pnext->pNext;
            }
            Node *pprev = pnext->pPrev;
            Node *pnew = new Node(pprev, pnext, data);

            pprev->pNext = pnew;
            pnext->pPrev = pnew;
        }
        ++m_length;
        return true;
    }
    void Append(int data)
    {
        Insert(m_length, data);
    }
    void Print() const
    {
        for(Node *pcurrent = m_pHead; pcurrent != nullptr; pcurrent = pcurrent->pNext){
            std::cout << pcurrent->data << " ";
        }
        std::cout << std::endl;
    }
    int GetSum() const
    {
        int sum = 0;
        for(Node *pcurrent = m_pHead; pcurrent != nullptr; pcurrent = pcurrent->pNext){
            sum += pcurrent->data;
        }
        return sum;
    }
    // returns 0 if index out of bounds
    int get_At(std::size_t index) const
    {
        std::size_t i = 0;
        for(Node *pcurrent = m_pHead; pcurrent != nullptr; pcurrent = pcurrent->pNext){
            if (i++ == index)
                return pcurrent->data;
        }
        return 0;
    }
    int Extract(std::size_t index)
    {
        Node *pcurrent = m_pHead;
        for (std::size_t i = 0; i < index; ++i) {
            if(!pcurrent) return 0;
            pcurrent = pcurrent->pNext;
        }
        if(pcurrent->pPrev){
            Node *pprev = pcurrent->pPrev;
            pprev->pNext = pcurrent->pNext;
        }
        if(pcurrent->pNext){
            Node *pnext = pcurrent->pNext;
            pnext->pPrev=pcurrent->pPrev;
        }
        if (pcurrent==m_pHead)
            m_pHead=pcurrent->pNext;
        if(pcurrent==m_pTail)
            m_pTail=pcurrent->pPrev;

        int temp = pcurrent->data;

        delete pcurrent;
        --m_length;
        return temp;
    }
private:
    Node *m_pHead;
    Node *m_pTail;
    std::size_t m_length;
};


#endif //ASSB_LINKEDLIST_H
