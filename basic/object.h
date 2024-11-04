#pragma once

#include <memory>
#include <string>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int n) : number_(n){};
    int GetValue() const {
        return number_;
    };

private:
    int number_{};
};

class Symbol : public Object {
public:
    Symbol(std::string symbol) : symbol_(symbol){};
    const std::string& GetName() const {
        return symbol_;
    };

private:
    std::string symbol_{};
};

class OpenParen : public Object {};

class CloseParen : public Object {};

class Dot : public Object {};

class Quote : public Object {};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> ptr) : left_son_(ptr){};
    Cell(std::shared_ptr<Object> ptr_l, std::shared_ptr<Object> ptr_r)
        : left_son_(ptr_l), right_son_(ptr_r){};
    std::shared_ptr<Object> GetFirst() const {
        return left_son_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return right_son_;
    };

private:
    std::shared_ptr<Object> left_son_{};
    std::shared_ptr<Object> right_son_{};
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::static_pointer_cast<T>(obj);
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (std::dynamic_pointer_cast<T>(obj) == nullptr) {
        return false;
    }
    return true;
};
