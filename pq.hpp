#pragma once

#include <set>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <tuple>

// Template-Klasse f�r eine Priority-Queue mit Update-Funktion.
template <typename T, typename Priority, typename Compare = std::less<Priority>>
class PriorityQueue {
public:
    using Element = std::pair<Priority, T>;

private:
    struct ElementCompare {
        Compare comp;
        bool operator()(const Element& lhs, const Element& rhs) const {
            return comp(lhs.first, rhs.first) || (!comp(rhs.first, lhs.first) && lhs.second < rhs.second);
        }
    };

    std::set<Element, ElementCompare> queue;
    std::unordered_map<T, typename std::set<Element>::iterator> elementMap;
    ElementCompare elementCompare;

public:
    // Konstruktor
    PriorityQueue(Compare comp = Compare()) : elementCompare{ comp } {}
    PriorityQueue(const uint32_t num_nodes, Compare comp = Compare()) : elementCompare{ comp } {
        for (uint32_t i = 0; i < num_nodes; i++) {
            push(i+1, 0);
        }
    }

    // F�gt ein Element mit einer Priorit�t hinzu.
    void push(const T& item, const Priority& priority) {
        auto it = elementMap.find(item);
        if (it != elementMap.end()) {
            // Element existiert bereits: Aktualisieren.
            update(item, priority);
        }
        else {
            auto inserted = queue.emplace(priority, item);
            if (inserted.second) {
                elementMap[item] = inserted.first;
            }
        }
    }

    // Aktualisiert die Priorit�t eines vorhandenen Elements.
    void update(const T& item, const Priority& newPriority) {
        auto it = elementMap.find(item);
        if (it != elementMap.end()) {
            // Element existiert, alte Instanz entfernen.
            queue.erase(it->second);
            elementMap.erase(it);
        }
        // Neue Priorit�t eintragen.
        push(item, newPriority);
    }

    void increment(const T& item) {
        auto it = elementMap.find(item);
        if (it == elementMap.end()) {
            throw std::runtime_error("Element not found in the priority queue");
        }

        // Die alte Priorit�t ermitteln und um 1 erh�hen
        Priority currentPriority = it->second->first;
        update(item, currentPriority + 1);
    }

    void decrement(const T& item) {
        auto it = elementMap.find(item);
        if (it == elementMap.end()) {
            throw std::runtime_error("Element not found in the priority queue");
        }

        // Die alte Priorit�t ermitteln und um 1 erh�hen
        Priority currentPriority = it->second->first;
        update(item, currentPriority - 1);
    }

    // Entfernt und gibt das Element mit der h�chsten Priorit�t zur�ck.
    T pop() {
        if (queue.empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        auto topElement = *queue.rbegin();
        queue.erase(--queue.end());
        elementMap.erase(topElement.second);
        return topElement.second;
    }

    // Gibt das Element mit der h�chsten Priorit�t zur�ck, ohne es zu entfernen.
    const T& top() const {
        if (queue.empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return queue.rbegin()->second;
    }

    // Gibt das Element mit der h�chsten Priorit�t, sowie dessen Priorit�t zur�ck, ohne es zu entfernen.
    const std::tuple<T, Priority> topPair() const {
        if (queue.empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return std::make_tuple(queue.rbegin()->second, queue.rbegin()->first);
    }

    // �berpr�ft, ob die Priority-Queue leer ist.
    bool empty() const {
        return queue.empty();
    }

    // Gibt die Anzahl der Elemente zur�ck.
    size_t size() const {
        return queue.size();
    }
};
