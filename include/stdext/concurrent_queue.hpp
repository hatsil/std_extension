#pragma once

#include "memory.hpp"

#include <utility>
#include <limits>
#include <cstdint>
#include <atomic>

namespace stdext {
template <class E, class Allocator = stdext::allocator<E>>
class concurrent_queue {
public:
using allocator_type = Allocator;
using size_type = std::size_t;

private:
    struct Node {
        std::shared_ptr<E> m_element;
        std::atomic<std::shared_ptr<Node>> m_next;

        Node(std::shared_ptr<E> element) noexcept
            : m_element(element)
            , m_next(nullptr) {}
    };

    using AllocatorNode = typename std::allocator_traits<Allocator>::rebind_alloc<Node>;
    using AllocatorTraitsNode = typename std::allocator_traits<Allocator>::rebind_traits<Node>;

    template <class... Args>
    std::shared_ptr<E> newElement(Args &&...args) {
        return ::stdext::make_shared<E>(m_alloc, std::forward<Args>(args)...);
    }

    std::shared_ptr<Node> newNode(std::shared_ptr<E> element) {
        return ::stdext::make_shared<Node>(m_alloc, element);
    }

    void push(std::shared_ptr<Node> node) noexcept {
        for (;;) {
            std::shared_ptr<Node> tail = nullptr;
            if (m_tail.compare_exchange_weak(tail, node)) {
                m_head = node;
                ++m_count;
                break;
            } else {
                std::shared_ptr<Node> next = nullptr;
                if (tail->m_next.compare_exchange_weak(next, node)) {
                    std::shared_ptr<Node> head = nullptr;
                    m_mhead.compare_exchange_strong(head, node);
                    ++m_count;
                    break;
                }
            }
        }
    }

    Allocator m_alloc;
    std::atomic<size_type> m_count;
    std::atomic<std::shared_ptr<Node>> m_head;
    std::atomic<std::shared_ptr<Node>> m_tail;

public:
    concurrent_queue() : concurrent_queue(Allocator()) {}
    concurrent_queue(const Allocator &alloc)
        : m_alloc(alloc)
        , m_count(size_type(0))
        , m_head(nullptr)
        , m_tail(nullptr) {}

    concurrent_queue(const concurrent_queue &) = delete;
    concurrent_queue &operator=(const concurrent_queue &) = delete;

    concurrent_queue(concurrent_queue &&) = default;
    concurrent_queue &operator=(concurrent_queue &&) = default;

    ~concurrent_queue() {
        while (nullptr != pop());
    }

    std::shared_ptr<E> front() const noexcept {
        std::shared_ptr<Node> head = m_head.load();
        if (nullptr == head) {
            return nullptr;
        }
        return head->m_element;
    }

    std::shared_ptr<E> back() const noexcept {
        if (nullptr == front()) {
            return nullptr;
        }
        std::shared_ptr<Node> tail = m_tail.load();
        if (nullptr == tail) {
            return nullptr;
        }
        return tail->m_element;
    }

    bool empty() const noexcept {
        return size_type(0) == size();
    }

    size_type size() const noexcept {
        return m_count.load();
    }


    std::shared_ptr<E> pop() noexcept {
        std::shared_ptr<Node> head = m_head.load();
        if (nullptr == head) {
            return nullptr;
        } else {
            while (nullptr != head && !m_head.compare_exchange_weak(head, head->m_next.load()));
            if (nullptr == head) {
                return nullptr;
            } else {
                m_tail.compare_exchange_strong(head, head->m_next.load());
                --m_count;
                return head->m_element;
            }
        }
    }

    std::shared_ptr<E> push(const E &element) {
        std::shared_ptr<E> elementPtr = newElement(element);
        std::shared_ptr<Node> node = newNode(elementPtr);
        push(node);
        return elementPtr;
    }

    std::shared_ptr<E> push(E &&element) {
        std::shared_ptr<E> elementPtr = newElement(std::move(element));
        std::shared_ptr<Node> node = newNode(elementPtr);
        push(node);
        return elementPtr;
    }

    template<class... Args>
    std::shared_ptr<E> emplace(Args &&...args) {
        std::shared_ptr<E> elementPtr = newElement(std::forward<Args>(args)...);
        std::shared_ptr<Node> node = newNode(elementPtr);
        push(node);
        return elementPtr;
    }

};
}
