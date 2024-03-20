//
// Created by Łukasz on 25.02.2024.
//

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <algorithm>
#include <mutex>

struct List {
    struct Node {
        int _x;
        Node* _next;

        explicit Node(int y) : _x(y), _next(nullptr) {}
    };

    Node* _head;
    std::mutex _mutex;

    List() : _head(nullptr) {}

    void insert(int x) {
        auto node = new Node(x);
        std::lock_guard<std::mutex> lock(_mutex);
        node->_next = _head;
        _head = node;
    }

    int count() const;
};

void thfun(List& list) {
    for (int i = 0; i < 100; ++i) {
        list.insert(i);
    }
}

int main() {
    List list;
    std::vector<std::thread> workers;
    for (int i = 0; i < 10; ++i) {
        workers.push_back(std::thread(&thfun, std::ref(list)));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread& th) {
        th.join();
    });

    int total = list.count();
    std::cout << total;
}

int List::count() const {
    int n = 0;
    auto curr = _head;
    while (curr != nullptr) {
        ++n;
        curr = curr->_next;
    }
    return n;
}

