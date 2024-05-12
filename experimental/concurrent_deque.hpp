#pragma once

#include <memory>
#include <atomic>
#include <cstdint>

namespace ext {
template <class E, class Allocator = ext::allocator<E>>
class concurrent_deque final {
  public:
    // types
    using allocator_type         = Allocator;
    using size_type              = std::size;
    using iterator               = /* implementation-defined */;
    using const_iterator         = /* implementation-defined */;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // construct/copy/destroy
    concurrent_deque() : concurrent_deque(Allocator()) {}
    explicit concurrent_deque(const Allocator &alloc);

    concurrent_deque(const concurrent_deque &) = delete;
    concurrent_deque &operator=(const concurrent_deque &) = delete;

    ~concurrent_deque();

    allocator_type get_allocator() const noexcept;

    // iterators
    iterator               begin() noexcept;
    const_iterator         begin() const noexcept;
    iterator               end() noexcept;
    const_iterator         end() const noexcept;
    reverse_iterator       rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator       rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    const_iterator         cbegin() const noexcept;
    const_iterator         cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // capacity
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] size_type size() const noexcept;
    [[nodiscard]] size_type max_size() const noexcept;

    // element access
    [[nodiscard]] std::shared_ptr<E> front() const;
    [[nodiscard]] std::shared_ptr<E> back() const;

    // modifiers
    template<class... Args> std::shared_ptr<E> emplace_front(Args&&... args);
    template<class... Args> std::shared_ptr<E> emplace_back(Args&&... args);

    void push_front(const E& element);
    void push_front(E&& element);
    void push_back(const T& element);
    void push_back(T&& element);

    [[nodiscard]] std::shared_ptr<E> pop_front();
    [[nodiscard]] std::shared_ptr<E> pop_back();

    void clear() noexcept;
};

// class concurrent_deque final {
// public:
//     concurrent_deque(const Allocator &alloc = Allocator());

//     concurrent_deque(const concurrent_deque &) = delete;
//     concurrent_deque &operator=(const concurrent_deque &) = delete;

//     ~concurrent_deque();

//     std::shared_ptr<E> push_back(const E &element);
//     std::shared_ptr<E> push_back(E &&element);
//     std::shared_ptr<E> push_front(const E &element);
//     std::shared_ptr<E> push_front(E &&element);

//     std::shared_ptr<E> pop_back();
//     std::shared_ptr<E> pop_front();

//     std::shared_ptr<E> peek_back();
//     std::shared_ptr<E> peek_front();

//     bool empty() const noexcept;
//     std::size_t size() const noexcept;

//     iterator begin() noexcept;
//     const_iterator begin() const noexcept;
//     const_iterator cbegin() const noexcept;


// private:
//     static inline constexpr int HOPS = 2;
//     using AllocatorTraitsElement = std::allocator_traits<Allocator>;

//     struct Node final {
//         std::atomic<Node *> m_prev;
//         std::atomic<Node *> m_next;
//         std::shared_ptr<E> m_element;

//         template <class... Args>
//         Node(Allocator &alloc, Args&&... args)
//         : m_prev(nullptr)
//         , m_next(nullptr) {
//             auto element = AllocatorTraitsElement::allocate(alloc, 1);
//             try {
//                 AllocatorTraitsElement::construct(alloc, element, std::forward<Args>(args)...);
//                 m_element = std::shared_ptr(element, [&alloc] (E *element) {
//                     AllocatorTraitsElement::destroy(alloc, element);
//                     AllocatorTraitsElement::deallocate(alloc, element, 1);
//                 });
//             } catch(...) {
//                 AllocatorTraitsElement::deallocate(alloc, element, 1);
//                 throw;
//             }
//         }

//         Node(const Node &) = delete;
//         Node &operator=(const Node &) = delete;

//         ~Node() = default;
//     };

//     using AllocatorTraitsNode = typename AllocatorTraitsElement::rebind_traits<Node>;
//     using AllocatorNode = typename AllocatorTraitsElement::rebind_alloc<Node>;

//     Allocator m_alloc;
//     AllocatorNode m_allocNode;
//     std::atomic<Node *> m_head;
//     std::atomic<Node *> m_tail;

//     template<class... Args>
//     Node *newNode(Args&&... args) {
//         Node *node = AllocatorTraitsNode::allocate(m_allocNode, 1);
//         try {
//             AllocatorTraitsNode::construct(m_allocNode, node, m_alloc, std::forward<Args>(args)...);
//             return node;
//         } catch(...) {
//             AllocatorTraitsNode::deallocate(m_allocNode, node, 1);
//             throw;
//         }
//     }

//     [[nodiscard]] std::shared_ptr<E> destroyNode(Node *node) {
//         auto element = node->m_element;
//         AllocatorTraitsNode::destroy(m_allocNode, node);
//         return element;
//     }

//     template<class... Args>
//     [[nodiscard]] std::shared_ptr<E> pushFront(Args&&... args) {
//         Node *node = newNode(std::forward<Args>(args)...);

//         for (;;)
//             for (Node *h = m_head, *p = h, *q = nullptr;;) {
//                 if ((q = p->m_prev)) != nullptr &&
//                     (q = (p = q)->m_prev) != nullptr)
//                     // Check for head updates every other hop.
//                     // If p == q, we are sure to follow head instead.
//                     p = (h != (h = m_head)) ? h : q;
//                 else if (p->m_next == p) // PREV_TERMINATOR
//                     break;
//                 else {
//                     // p is first node
//                     node->m_next = p;
//                     Node *pPrev = nullptr;
//                     if (p->m_prev.compare_exchange_strong(pPrev, node)) {
//                         // Successful CAS is the linearization point
//                         // for e to become an element of this deque,
//                         // and for newNode to become "live".
//                         if (p != h) // hop two nodes at a time; failure is OK
//                             m_head.compare_exchange_weak(h, node);
//                         return node->m_element;
//                     }
//                     // Lost CAS race to another thread; re-read next
//                 }
//             }
//     }

//     template<class... Args>
//     [[nodiscard]] std::shared_ptr<E> pushBack(Args&&... args) {
//         Node *node = newNode(std::forward<Args>(args)...);
//         for (;;)
//             for (Node *t = m_tail, *p = t, *q = nullptr;;) {
//                 if ((q = p->m_next) != nullptr &&
//                     (q = (p = q)->m_next) != nullptr)
//                     // Check for tail updates every other hop.
//                     // If p == q, we are sure to follow tail instead.
//                     p = (t != (t = m_tail)) ? t : q;
//                 else if (p->m_prev == p) // NEXT_TERMINATOR
//                     break;
//                 else {
//                     // p is last node
//                     node->m_prev = p;
//                     Node *pNext = nullptr;
//                     if (p->m_next.compare_exchange_strong(pNext, node)) {
//                         // Successful CAS is the linearization point
//                         // for e to become an element of this deque,
//                         // and for newNode to become "live".
//                         if (p != t) // hop two nodes at a time; failure is OK
//                             m_tail.compare_exchange_weak(t, node);
//                         return node->m_element;
//                     }
//                     // Lost CAS race to another thread; re-read next
//                 }
//             }
//     }

    // /**
    //  * Unlinks non-null node x.
    //  */
    // [[nodiscard]] std::shared_ptr<E> unlink(Node *x) {
    //     Node *prev = x->m_prev;
    //     Node *next = x->m_next;
    //     if (nullptr == prev) {
    //         return unlinkFirst(x, next);
    //     } else if (nullptr ==  next) {
    //         return unlinkLast(x, prev);
    //     } else {
    //         // Unlink interior node.
    //         //
    //         // This is the common case, since a series of polls at the
    //         // same end will be "interior" removes, except perhaps for
    //         // the first one, since end nodes cannot be unlinked.
    //         //
    //         // At any time, all active nodes are mutually reachable by
    //         // following a sequence of either next or prev pointers.
    //         //
    //         // Our strategy is to find the unique active predecessor
    //         // and successor of x.  Try to fix up their links so that
    //         // they point to each other, leaving x unreachable from
    //         // active nodes.  If successful, and if x has no live
    //         // predecessor/successor, we additionally try to gc-unlink,
    //         // leaving active nodes unreachable from x, by rechecking
    //         // that the status of predecessor and successor are
    //         // unchanged and ensuring that x is not reachable from
    //         // tail/head, before setting x's prev/next links to their
    //         // logical approximate replacements, self/TERMINATOR.
    //         Node *activePred, *activeSucc;
    //         bool isFirst, isLast;
    //         int hops = 1;

    //         // Find active predecessor
    //         for (Node *p = prev; ; ++hops) {
    //             if (p->m_element != nullptr) {
    //                 activePred = p;
    //                 isFirst = false;
    //                 break;
    //             }
    //             Node *q = p->m_prev;
    //             if (nullptr == q) {
    //                 if (p->m_next == p)
    //                     return destroyNode(x);
    //                 activePred = p;
    //                 isFirst = true;
    //                 break;
    //             }
    //             else if (p == q)
    //                 return destroyNode(x);
    //             else
    //                 p = q;
    //         }

    //         // Find active successor
    //         for (Node *p = next; ; ++hops) {
    //             if (p->m_element != nullptr) {
    //                 activeSucc = p;
    //                 isLast = false;
    //                 break;
    //             }
    //             Node *q = p->m_next;
    //             if (nullptr == q) {
    //                 if (p->m_prev == p)
    //                     return destroyNode(x);
    //                 activeSucc = p;
    //                 isLast = true;
    //                 break;
    //             }
    //             else if (p == q)
    //                 return destroyNode(x);
    //             else
    //                 p = q;
    //         }

    //         // TODO: better HOP heuristics
    //         if (hops < HOPS
    //             // always squeeze out interior deleted nodes
    //             && (isFirst | isLast))
    //             return destroyNode(x);

    //         // Squeeze out deleted nodes between activePred and
    //         // activeSucc, including x.
    //         skipDeletedSuccessors(activePred);
    //         skipDeletedPredecessors(activeSucc);

    //         // Try to gc-unlink, if possible
    //         if ((isFirst | isLast) &&

    //             // Recheck expected state of predecessor and successor
    //             (activePred->m_next == activeSucc) &&
    //             (activeSucc->m_prev == activePred) &&
    //             (isFirst ? activePred->m_prev == nullptr : activePred->m_element != nullptr) &&
    //             (isLast  ? activeSucc.next == null : activeSucc.item != null)) {

    //             updateHead(); // Ensure x is not reachable from head
    //             updateTail(); // Ensure x is not reachable from tail

    //             // Finally, actually gc-unlink
    //             PREV.setRelease(x, isFirst ? prevTerminator() : x);
    //             NEXT.setRelease(x, isLast  ? nextTerminator() : x);
    //         }
    //     }
    // }

    // @SuppressWarnings("unchecked")
    // Node<E> prevTerminator() {
    //     return (Node<E>) PREV_TERMINATOR;
    // }

    // @SuppressWarnings("unchecked")
    // Node<E> nextTerminator() {
    //     return (Node<E>) NEXT_TERMINATOR;
    // }


// };
}
