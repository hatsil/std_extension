#pragma once

#include <atomic>
#include <memory>
#include <cstdint>

namespace ext {
template <class T, class Allocator = std::allocator<T>>
class concurrent_list final {
public:
    concurrent_list();
    explicit concurrent_list(const Allocator &alloc);

    concurrent_list(const concurrent_list &) = delete;
    concurrent_list &operator=(const concurrent_list &) = delete;

    ~concurrent_list();

    void push_back(const T &value);
    void push_back(T &&value);
    void push_front(const T &value);
    void push_front(T &&value);

    T &back();
    const T &back() const;
    T &front();
    const T &front() const;

    void pop_back();
    void pop_front();

    std::size_t size() const noexcept;
    bool empty() const noexcept;
    void clear();

private:
    struct Node final {
        Node(const T &value);
        Node(T &&value);

        Node() = delete;
        Node(const Node &) = delete;
        Node &operator=(const Node &) = delete;

        ~Node() = default;

        T m_value;
        Node *m_next;
        Node *m_prev;
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<NodeAllocator>;

    template <class U = T>
    Node *makeNode(U &&value);

    void pushBack(Node *newNode) noexcept;
    void pushFront(Node *newNode) noexcept;

    NodeAllocator m_alloc;
    std::atomic<Node *> m_head;
    std::atomic<Node *> m_tail;
    std::atomic<std::size_t> m_size;
};

template <class T, class Allocator>
concurrent_list<T, Allocator>::concurrent_list(): concurrent_list(Allocator()) {}

template <class T, class Allocator>
concurrent_list<T, Allocator>::concurrent_list(const Allocator &alloc): m_alloc(alloc), m_head(nullptr), m_tail(nullptr), m_size(0) {}

template <class T, class Allocator>
template <class U>
concurrent_list<T, Allocator>::Node *concurrent_list<T, Allocator>::makeNode(U &&value) {
    Node *newNode = AllocTraits::allocate(m_alloc, 1);
    try {
        AllocTraits::construct(m_alloc, newNode, std::forward<U>(value));
        return newNode;
    } catch(...) {
        AllocTraits::deallocate(m_alloc, newNode, 1);
        throw;
    }
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::pushBack(Node *newNode) noexcept {
    newNode->m_prev = m_tail.load();
    while(!m_tail.compare_exchange_weak(newNode->m_prev, newNode));

    if (nullptr == newNode->m_prev) {
        Node *head = nullptr;
        if (!m_head.compare_exchange_strong(head, newNode)) {
            head->m_next = newNode;
        }
    } else {
        newNode->m_prev->m_next = newNode;
    }
    ++m_size;
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::pushFront(Node *newNode) noexcept {
    newNode->m_next = m_head.load();
    while(!m_head.compare_exchange_weak(newNode->m_next, newNode));

    if (nullptr == newNode->m_next) {
        Node *tail = nullptr;
        if (!m_tail.compare_exchange_strong(tail, newNode)) {
            tail->m_prev = newNode;
        }
    } else {
        newNode->m_next->m_prev = newNode;
    }
    ++m_size;
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::push_back(const T &value) {
    pushBack(makeNode(value));
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::push_back(T &&value) {
    pushBack(makeNode(std::move(value)));
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::push_front(const T &value) {
    pushFront(makeNode(value));
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::push_front(T &&value) {
    pushFront(makeNode(std::move(value)));
}

template <class T, class Allocator>
T &concurrent_list<T, Allocator>::back() {
    return m_tail.load()->m_value;
}

template <class T, class Allocator>
const T &concurrent_list<T, Allocator>::back() const {
    return m_tail.load()->m_value;
}

template <class T, class Allocator>
T &concurrent_list<T, Allocator>::front() {
    return m_head.load()->m_value;
}

template <class T, class Allocator>
const T &concurrent_list<T, Allocator>::front() const {
    return m_head.load()->m_value;
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::pop_back() {
    Node *tail = m_tail.load();
    while(nullptr != tail && !m_tail.compare_exchange_weak(tail, tail->m_next));
    if (nullptr == tail) {
        return;
    }
    --m_size;
    if (nullptr != tail->m_next) {
        tail->m_next->m_prev = nullptr;
    } else {
        Node *head = nullptr;
        m_head.compare_exchange_strong(head, nullptr);
    }
    AllocTraits::destroy(m_alloc, tail);
    AllocTraits::deallocate(m_alloc, tail, 1);
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::pop_front() {
    Node *head = m_head.load();
    while(nullptr != head && !m_head.compare_exchange_weak(head, head->m_prev));
    if (nullptr == head) {
        return;
    }
    --m_size;
    if (nullptr != head->m_prev) {
        head->m_prev->m_next = nullptr;
    } else {
        Node *tail = nullptr;
        m_tail.compare_exchange_strong(tail, nullptr);
    }
    AllocTraits::destroy(m_alloc, head);
    AllocTraits::deallocate(m_alloc, head, 1);
}

template <class T, class Allocator>
std::size_t concurrent_list<T, Allocator>::size() const noexcept {
    return m_size.load();
}

template <class T, class Allocator>
bool concurrent_list<T, Allocator>::empty() const noexcept {
    return size() == 0;
}

template <class T, class Allocator>
void concurrent_list<T, Allocator>::clear() {
    while (size() != 0) {
        pop_front();
    }
}
}