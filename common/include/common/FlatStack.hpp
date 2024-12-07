#pragma once
#include <vector>
namespace common {
template <class Ty>
class FlatStack {
public:
    using Container = std::vector<Ty>;
    using Iterator = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;
    using ReverseIterator = typename Container::reverse_iterator;
    using ConstReverseIterator = typename Container::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    ConstIterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    ConstIterator end() const { return data_.end(); }
    ReverseIterator rbegin() { return data_.rbegin(); }
    ConstReverseIterator rbegin() const { return data_.rbegin(); }
    ReverseIterator rend() { return data_.rend(); }
    ConstReverseIterator rend() const { return data_.rend(); }
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
