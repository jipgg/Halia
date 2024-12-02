#pragma once
#include <vector>
namespace common {
template <class Ty>
class Flat_stack {
public:
    using Container = std::vector<Ty>;
    using Iterator = typename Container::iterator;
    using Const_iterator = typename Container::const_iterator;
    using Reverse_iterator = typename Container::reverse_iterator;
    using Const_reverse_iterator = typename Container::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    Const_iterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    Const_iterator end() const { return data_.end(); }
    Reverse_iterator rbegin() { return data_.rbegin(); }
    Const_reverse_iterator rbegin() const { return data_.rbegin(); }
    Reverse_iterator rend() { return data_.rend(); }
    Const_reverse_iterator rend() const { return data_.rend(); }
    Ty& at(size_t idx) {return data_.at(idx);}
    const Ty& at(size_t idx) const {return data_.at(idx);}
    Ty& operator[](size_t idx) {return data_[idx];}
    Ty& emplace(Ty&& v) {return data_.emplace_back(std::forward<Ty&&>(v));}
    void push(const Ty& v) {data_.push_back(v);}
    void pop() {data_.pop_back();}
    Ty& top() {return data_.back();}
    size_t size() const {data_.size();}
    void reserve(size_t amount) {data_.reserve(amount);}
    bool empty() const {return data_.empty();}
    size_t capacity() const {return data_.capacity();}
private:
    std::vector<Ty> data_;
};
}
