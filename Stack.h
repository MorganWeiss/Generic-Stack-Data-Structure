//
//  Stack.h
//  Stack
//
//  Created by Morgan Weiss on 8/29/18.
//  Copyright © 2018 Morgan Weiss. All rights reserved.
//

#ifndef Stack_h
#define Stack_h

#include <iterator>
#include <memory>

template <class T>
class Stack {
    struct Node {
        T data;
        std::unique_ptr<Node> next = nullptr;
        
        template<typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args&&...>::value>>
        explicit Node(std::unique_ptr<Node>&& next, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
        : data{ std::forward<Args>(args)... }, next{ std::move(next) } {}
        
        // disable if noncopyable<T> for cleaner error msgs
        explicit Node(const T& x, std::unique_ptr<Node>&& p = nullptr)
        : data(x)
        , next(std::move(p)) {}
        
        // disable if nonmovable<T> for cleaner error msgs
        explicit Node(T&& x, std::unique_ptr<Node>&& p = nullptr)
        : data(std::move(x))
        , next(std::move(p)) {}
    };
    std::unique_ptr<Node> front = nullptr;
    
    void do_unchecked_pop() {
        front = std::move(front->next);
    }
    
public:
    // Constructors
    Stack() = default;                                           // empty constructor
    Stack(Stack const &source);                                  // copy constructor
    
    // Rule of 5
    Stack(Stack &&move) noexcept;                                // move constructor
    Stack& operator=(Stack &&move) noexcept;                     // move assignment operator
    ~Stack();
    
    // Overload operators
    Stack& operator=(Stack const &rhs);
    
    // Create an iterator class
    class iterator;
    iterator begin();
    iterator end();
    iterator before_begin();
    
    // Create const iterator class
    class const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator before_begin() const;
    const_iterator cbefore_begin() const;
    
    // Member functions
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);
    
    void swap(Stack& other) noexcept;
    bool empty() const {return front == nullptr;}
    int size() const;
    void push(const T& theData);
    void push(T&& theData);
    T& top();
    const T& top() const;
    void pop();
    
};

template <class T>
class Stack<T>::iterator {
    Node* node = nullptr;
    bool before_begin = false;
    
public:
    friend class Stack<T>;
    
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T * ;
    using reference = T & ;
    
    operator const_iterator() const noexcept { return const_iterator{ node }; }
    iterator(Node* node = nullptr, bool before = false) : node{ node }, before_begin{ before } {}
    
    bool operator!=(iterator other) const noexcept;
    bool operator==(iterator other) const noexcept;
    
    T& operator*() const { return node->data; }
    T& operator->() const { return &node->data; }
    
    iterator& operator++();
    iterator operator++(int);
};

template <class T>
class Stack<T>::const_iterator {
    Node* node = nullptr;
    bool before_begin = false;
    
public:
    friend class Stack<T>;
    
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T * ;
    using reference = const T & ;
    
    const_iterator() = default;
    const_iterator(Node* node, bool before = false) : node{ node }, before_begin{ before } {}
    
    
    bool operator!=(const_iterator other) const noexcept;
    bool operator==(const_iterator other) const noexcept;
    
    const T& operator*() const { return node->data; }
    const T& operator->() const { return &node->data; }
    
    const_iterator& operator++();
    const_iterator operator++(int);
};


template <class T>
Stack<T>::Stack(Stack<T> const& source) {
    try {
        for(auto loop = source.front.get(); loop != nullptr; loop = loop->next.get())
            push(loop->data);
    }
    catch (...) {
        while(front != nullptr)
            do_unchecked_pop();
        throw;
    }
}

template <class T>
Stack<T>::Stack(Stack&& move) noexcept {
    move.swap(*this);
}

template <class T>
Stack<T>& Stack<T>::operator=(Stack&& move) noexcept {
    move.swap(*this);
    return *this;
}

template <class T>
Stack<T>::~Stack() {
    while(front != nullptr) {
        do_unchecked_pop();
    }
}

template <class T>
Stack<T>& Stack<T>::operator=(Stack const& rhs) {
    Stack copy(rhs);
    swap(copy);
    return *this;
}

template <class T>
void Stack<T>::swap(Stack& other) noexcept {
    using std::swap;
    swap(front,other.front);
}

template <class T>
template <typename... Args>
typename Stack<T>::iterator Stack<T>::emplace(const_iterator pos, Args&&... args) {
    if (pos.before_begin) {
        emplace_front(std::forward<Args>(args)...);
        return begin();
    }
    
    if (pos.node->next) {
        pos.node->next = std::make_unique<Node>(std::move(pos.node->next), std::forward<Args>(args)...);  // Creating a new node that has the old next pointer with the new value and assign it to the next pointer of the current node
        return { pos.node->next.get() };
    }
}

// Free function
template <typename T>
void swap(Stack<T>& a, Stack<T>& b) noexcept {
    a.swap(b);
}

template <class T>
int Stack<T>::size() const {
    int size = 0;
    for (auto current = front.get(); current != nullptr; current = current->next.get())
        size++;
    return size;
}

template <class T>
void Stack<T>::push(const T& theData) {
    std::unique_ptr<Node> newNode = std::make_unique<Node>(theData);
    
    if(front) {
        newNode->next = std::move(front);
    }
    front = std::move(newNode);
}

template <class T>
void Stack<T>::push(T&& theData) {
    std::unique_ptr<Node> newNode = std::make_unique<Node>(std::move(theData));
    
    if(front) {
        newNode->next = std::move(front);
    }
    front = std::move(newNode);
}

template <class T>
T& Stack<T>::top() {
    if(!empty()) {
        return front->data;
    }
    else {
        throw std::out_of_range("The stack is empty!");
    }
}

template <class T>
const T& Stack<T>::top() const {
    if(!empty()) {
        return front->data;
    }
    else {
        throw std::out_of_range("The stack is empty!");
    }
}

template <class T>
void Stack<T>::pop() {
    if(empty()) {
        throw std::out_of_range("The stack is empty!");
    }
    do_unchecked_pop();
}

// Iterator Implementaion////////////////////////////////////////////////
template <class T>
typename Stack<T>::iterator& Stack<T>::iterator::operator++() {
    if (before_begin) before_begin = false;
    else node = node->next.get();
    
    return *this;
}

template<typename T>
typename Stack<T>::iterator Stack<T>::iterator::operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
}

template<typename T>
bool Stack<T>::iterator::operator==(iterator other) const noexcept {
    return node == other.node && before_begin == other.before_begin;
}

template<typename T>
bool Stack<T>::iterator::operator!=(iterator other) const noexcept {
    return !(*this == other);
}



template<class T>
typename Stack<T>::iterator Stack<T>::begin() {
    return front.get();
}

template<class T>
typename Stack<T>::iterator Stack<T>::end() {
    return {};
}

template <class T>
typename Stack<T>::iterator Stack<T>::before_begin() {
    return { front.get(), true };
}

// Const Iterator Implementaion////////////////////////////////////////////////
template <class T>
typename Stack<T>::const_iterator& Stack<T>::const_iterator::operator++() {
    if (before_begin) before_begin = false;
    else node = node->next.get();
    
    return *this;
}

template<typename T>
typename Stack<T>::const_iterator Stack<T>::const_iterator::operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
}

template<typename T>
bool Stack<T>::const_iterator::operator==(const_iterator other) const noexcept {
    return node == other.node && before_begin == other.before_begin;
}

template<typename T>
bool Stack<T>::const_iterator::operator!=(const_iterator other) const noexcept {
    return !(*this == other);
}


template <class T>
typename Stack<T>::const_iterator Stack<T>::begin() const {
    return front.get();
}

template <class T>
typename Stack<T>::const_iterator Stack<T>::end() const {
    return {};
}

template <class T>
typename Stack<T>::const_iterator Stack<T>::cbegin() const {
    return begin();
}

template <class T>
typename Stack<T>::const_iterator Stack<T>::cend() const {
    return end();
}

template <class T>
typename Stack<T>::const_iterator Stack<T>::before_begin() const {
    return { front.get(), true };
}

template <class T>
typename Stack<T>::const_iterator Stack<T>::cbefore_begin() const {
    return before_begin();
}


#endif /* Stack_h */
