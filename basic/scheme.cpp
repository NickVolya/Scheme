#include "scheme.h"

std::shared_ptr<Object> ReadAll(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};

    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Invalid syntax");
    }
    if (tokenizer.DotUsed() && !Is<Cell>(obj)) {
        throw SyntaxError("Invalid syntax");
    }
    return obj;
}

void OutputFirst(std::shared_ptr<Object> tree, std::string& ans) {
    if (tree == nullptr) {
        ans += "()";
        return;
    }
    if (!Is<Cell>(tree)) {
        if (Is<Number>(tree)) {
            auto num = As<Number>(tree);
            ans += std::to_string(num->GetValue());
            return;
        }
        auto symb = As<Symbol>(tree);
        ans += symb->GetName();
        return;
    }
    ans += '(';
    OutputSecond(tree, ans);
    ans += ')';
}

void OutputSecond(std::shared_ptr<Object> tree, std::string& ans) {
    if (tree == nullptr) {
        ans.pop_back();
        return;
    }
    if (!Is<Cell>(tree)) {
        if (Is<Number>(tree)) {
            auto num = As<Number>(tree);
            ans += ". ";
            ans += std::to_string(num->GetValue());
            return;
        }
        auto symb = As<Symbol>(tree);
        ans += ". ";
        ans += symb->GetName();
        return;
    }
    auto pair = As<Cell>(tree);
    OutputFirst(pair->GetFirst(), ans);
    ans += ' ';
    OutputSecond(pair->GetSecond(), ans);
}

class Function {
public:
    virtual ~Function() = default;
    virtual std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) = 0;
    virtual bool IsBoolean() {
        return false;
    }
    virtual bool IsQuote() {
        return false;
    }
};

class QuoteFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr) {
            return ptr;
        }
        if (!Is<Cell>(ptr) || As<Cell>(ptr)->GetSecond() != nullptr) {
            throw SyntaxError("Invalid syntax for quote");
        }
        return As<Cell>(ptr)->GetFirst();
    }
    bool IsQuote() override {
        return true;
    }
};

class IsNumberFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        if (pair->GetFirst() == nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        if (Is<Number>(pair->GetFirst())) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

class EqualFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        return Helper(ptr);
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        std::vector<std::shared_ptr<Object>> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        bool ans = true;
        for (size_t i = 0; i < nums.size() - 1; ++i) {
            if (Is<Number>(nums[i]) && Is<Number>(nums[i + 1])) {
                if (As<Number>(nums[i])->GetValue() != As<Number>(nums[i + 1])->GetValue()) {
                    ans = false;
                    break;
                }
                continue;
            }
            throw RuntimeError("Invalid operands");
        }
        if (ans) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<std::shared_ptr<Object>>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            nums.push_back(ptr);
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class MoreFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        return Helper(ptr);
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        std::vector<std::shared_ptr<Object>> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        bool ans = true;
        for (size_t i = 0; i < nums.size() - 1; ++i) {
            if (Is<Number>(nums[i]) && Is<Number>(nums[i + 1])) {
                if (As<Number>(nums[i])->GetValue() <= As<Number>(nums[i + 1])->GetValue()) {
                    ans = false;
                    break;
                }
                continue;
            }
            throw RuntimeError("Invalid operands");
        }
        if (ans) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<std::shared_ptr<Object>>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            nums.push_back(ptr);
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class LessFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        return Helper(ptr);
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        std::vector<std::shared_ptr<Object>> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        bool ans = true;
        for (size_t i = 0; i < nums.size() - 1; ++i) {
            if (Is<Number>(nums[i]) && Is<Number>(nums[i + 1])) {
                if (As<Number>(nums[i])->GetValue() >= As<Number>(nums[i + 1])->GetValue()) {
                    ans = false;
                    break;
                }
                continue;
            }
            throw RuntimeError("Invalid operands");
        }
        if (ans) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<std::shared_ptr<Object>>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            nums.push_back(ptr);
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class MoreOrEqualFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        return Helper(ptr);
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        std::vector<std::shared_ptr<Object>> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        bool ans = true;
        for (size_t i = 0; i < nums.size() - 1; ++i) {
            if (Is<Number>(nums[i]) && Is<Number>(nums[i + 1])) {
                if (As<Number>(nums[i])->GetValue() < As<Number>(nums[i + 1])->GetValue()) {
                    ans = false;
                    break;
                }
                continue;
            }
            throw RuntimeError("Invalid operands");
        }
        if (ans) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<std::shared_ptr<Object>>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            nums.push_back(ptr);
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class LessOrEqualFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        return Helper(ptr);
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        std::vector<std::shared_ptr<Object>> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        bool ans = true;
        for (size_t i = 0; i < nums.size() - 1; ++i) {
            if (Is<Number>(nums[i]) && Is<Number>(nums[i + 1])) {
                if (As<Number>(nums[i])->GetValue() > As<Number>(nums[i + 1])->GetValue()) {
                    ans = false;
                    break;
                }
                continue;
            }
            throw RuntimeError("Invalid operands");
        }
        if (ans) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<std::shared_ptr<Object>>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            nums.push_back(ptr);
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class SumFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Number>(0);
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = 0;
        for (const auto& value : nums) {
            ans += value;
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to + func");
            }
            auto num = As<Number>(ptr);
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class SubstitutionFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return nullptr;
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            ans -= nums[i];
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to - func");
            }
            auto num = As<Number>(ptr);
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class MultiplicationFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return std::make_shared<Number>(1);
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = 1;
        for (const auto& value : nums) {
            ans *= value;
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to * func");
            }
            auto num = As<Number>(ptr);
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class DivideFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return nullptr;
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            ans /= nums[i];
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to / func");
            }
            auto num = As<Number>(ptr);
            if (num->GetValue() == 0) {
                throw RuntimeError("Divididng by zero");
            }
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class MaxFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return nullptr;
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = std::numeric_limits<int64_t>::min();
        for (const auto& value : nums) {
            ans = std::max(ans, value);
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to * func");
            }
            auto num = As<Number>(ptr);
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class MinFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr || !Is<Number>(helper)) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        auto num = As<Number>(helper);
        return num;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return nullptr;
        }
        std::vector<int64_t> nums;
        FullVector(ptr, nums);
        if (nums.empty()) {
            throw RuntimeError("No operands");
        }
        int64_t ans = std::numeric_limits<int64_t>::max();
        for (const auto& value : nums) {
            ans = std::min(ans, value);
        }
        return std::make_shared<Number>(ans);
    }

    void FullVector(std::shared_ptr<Object> ptr, std::vector<int64_t>& nums) {
        if (ptr == nullptr) {
            return;
        }
        if (!Is<Cell>(ptr)) {
            if (!Is<Number>(ptr)) {
                throw RuntimeError("Invalid argument to * func");
            }
            auto num = As<Number>(ptr);
            nums.push_back(num->GetValue());
            return;
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        FullVector(pair->GetFirst(), nums);
        FullVector(pair->GetSecond(), nums);
    }
};

class AbsFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("No input");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr) {
            throw RuntimeError("Invalid operands");
        }
        if (pair->GetFirst() == nullptr) {
            throw RuntimeError("Invalid operands");
        }
        if (Is<Number>(pair->GetFirst())) {
            return std::make_shared<Number>(std::abs(As<Number>(pair->GetFirst())->GetValue()));
        }
        throw RuntimeError("Invalid operands");
    }
};

class IsBooleanFunction : public Function {
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr || pair->GetFirst() == nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        if (Is<Symbol>(pair->GetFirst())) {
            auto symb = As<Symbol>(pair->GetFirst());
            if (symb->GetName() == "#t" || symb->GetName() == "#f") {
                return std::make_shared<Symbol>("#t");
            }
        }
        return std::make_shared<Symbol>("#f");
    }
};

class NotFunctioon : public Function {
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("No operands for no-func");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr) {
            throw RuntimeError("Invalid operands");
        }
        if (pair->GetFirst() == nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        if (Is<Symbol>(pair->GetFirst())) {
            auto symb = As<Symbol>(pair->GetFirst());
            if (symb->GetName() == "#f") {
                return std::make_shared<Symbol>("#t");
            }
        }
        return std::make_shared<Symbol>("#f");
    }
};

class AndFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        return helper;
    }

    bool IsBoolean() override {
        return true;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#t");
        }
        std::shared_ptr<Object> ans = std::make_shared<Symbol>("#t");
        std::shared_ptr<Object> pair = As<Cell>(ptr);
        while (pair != nullptr) {
            if (!Is<Cell>(pair)) {
                auto first = RealCount(pair);
                if (first.first != nullptr && Is<Cell>(first.first) && !first.second) {
                    throw RuntimeError("Invalid syntax");
                }
                auto value = first.first;
                if (Is<Symbol>(value)) {
                    auto symb = As<Symbol>(value);
                    if (symb->GetName() == "#f") {
                        ans = std::make_shared<Symbol>("#f");
                        return ans;
                    } else {
                        ans = std::make_shared<Symbol>(symb->GetName());
                        return ans;
                    }
                }
                if (Is<Number>(value)) {
                    ans = std::make_shared<Number>(As<Number>(value)->GetValue());
                    return ans;
                }
            }
            auto rpair = As<Cell>(pair);
            auto first = RealCount(rpair->GetFirst());
            if (first.first != nullptr && Is<Cell>(first.first) && !first.second) {
                throw RuntimeError("Invalid syntax");
            }
            auto value = first.first;
            if (value == nullptr) {
                throw RuntimeError("Invalid operands");
            }
            if (Is<Symbol>(value)) {
                auto symb = As<Symbol>(value);
                if (symb->GetName() == "#f") {
                    ans = std::make_shared<Symbol>("#f");
                    return ans;
                } else {
                    ans = std::make_shared<Symbol>(symb->GetName());
                    pair = rpair->GetSecond();
                    continue;
                }
            }
            if (Is<Number>(value)) {
                ans = std::make_shared<Number>(As<Number>(value)->GetValue());
                pair = rpair->GetSecond();
                continue;
            }
            ans = std::make_shared<Cell>(As<Cell>(value)->GetFirst(), As<Cell>(value)->GetSecond());
            pair = rpair->GetSecond();
        }
        return ans;
    }
};

class OrFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        auto helper = Helper(ptr);
        if (helper == nullptr) {
            throw RuntimeError("Sth went wrong, it is not a number");
        }
        return helper;
    }

    bool IsBoolean() override {
        return true;
    }

private:
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        std::shared_ptr<Object> ans = std::make_shared<Symbol>("#f");
        std::shared_ptr<Object> pair = As<Cell>(ptr);
        while (pair != nullptr) {
            if (!Is<Cell>(pair)) {
                auto first = RealCount(pair);
                if (first.first != nullptr && Is<Cell>(first.first) && !first.second) {
                    throw RuntimeError("Invalid syntax");
                }
                auto value = first.first;
                if (Is<Symbol>(value)) {
                    auto symb = As<Symbol>(value);
                    if (symb->GetName() == "#f") {
                        return ans;
                    }
                }
                return value;
            }
            auto rpair = As<Cell>(pair);
            auto first = RealCount(rpair->GetFirst());
            if (first.first != nullptr && Is<Cell>(first.first) && !first.second) {
                throw RuntimeError("Invalid syntax");
            }
            auto value = first.first;
            if (value == nullptr) {
                throw RuntimeError("Invalid operands");
            }
            if (Is<Symbol>(value)) {
                auto symb = As<Symbol>(value);
                if (symb->GetName() == "#f") {
                    pair = rpair->GetSecond();
                    continue;
                }
            }
            return value;
        }
        return ans;
    }
};

class IsPairFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr || pair->GetFirst() == nullptr ||
            !Is<Cell>(pair->GetFirst())) {
            return std::make_shared<Symbol>("#f");
        }
        pair = As<Cell>(pair->GetFirst());
        if (pair->GetFirst() == nullptr) {
            throw RuntimeError("Invalid syntax");
        }
        if (!Is<Cell>(pair->GetFirst()) && pair->GetSecond() != nullptr &&
            !Is<Cell>(pair->GetSecond())) {
            return std::make_shared<Symbol>("#t");
        }
        if (!Is<Cell>(pair->GetFirst()) && pair->GetSecond() != nullptr &&
            Is<Cell>(pair->GetSecond()) && !Is<Cell>(As<Cell>(pair->GetSecond())->GetFirst()) &&
            As<Cell>(pair->GetSecond())->GetSecond() == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

class IsNullFunctioon : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr && pair->GetSecond() == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
};

class IsListFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            return std::make_shared<Symbol>("#f");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() != nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        auto ptr_check = pair->GetFirst();
        if (Helper(ptr_check)) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }

private:
    bool Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return true;
        }
        return Is<Cell>(ptr) && Helper(As<Cell>(ptr)->GetSecond());
    }
};

class ConsFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("Invalid operands");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || Is<Cell>(pair->GetFirst()) ||
            pair->GetSecond() == nullptr || !Is<Cell>(pair->GetSecond()) ||
            As<Cell>(pair->GetSecond())->GetSecond() != nullptr ||
            As<Cell>(pair->GetSecond())->GetFirst() == nullptr ||
            Is<Cell>(As<Cell>(pair->GetSecond())->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        return std::make_shared<Cell>(pair->GetFirst(), As<Cell>(pair->GetSecond())->GetFirst());
    }
};

class CarFunction : public Function {
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("Invalid operands");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || !Is<Cell>(pair->GetFirst()) ||
            pair->GetSecond() != nullptr) {
            throw RuntimeError("Invalid operands");
        }
        pair = As<Cell>(pair->GetFirst());
        return pair->GetFirst();
    }
};

class CdrFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("Invalid operands");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetFirst() == nullptr || !Is<Cell>(pair->GetFirst()) ||
            pair->GetSecond() != nullptr) {
            throw RuntimeError("Invalid operands");
        }
        pair = As<Cell>(pair->GetFirst());
        return pair->GetSecond();
    }
};

class ListFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (Helper(ptr)) {
            return ptr;
        }
        throw RuntimeError("Invalid operands");
    }

private:
    bool Helper(std::shared_ptr<Object> ptr) {
        if (ptr == nullptr) {
            return true;
        }
        return Is<Cell>(ptr) && Helper(As<Cell>(ptr)->GetSecond());
    }
};

class ListRefFunction : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("Invalid operands");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() == nullptr || !Is<Cell>(pair->GetSecond()) ||
            As<Cell>(pair->GetSecond())->GetSecond() != nullptr ||
            As<Cell>(pair->GetSecond())->GetFirst() == nullptr ||
            !Is<Number>(As<Cell>(pair->GetSecond())->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        auto ind = As<Number>(As<Cell>(pair->GetSecond())->GetFirst())->GetValue();
        if (!Is<Cell>(pair->GetFirst()) || ind < 0) {
            throw RuntimeError("Invalid operands");
        }
        auto list = As<Cell>(pair->GetFirst());
        while (ind > 0) {
            if (list->GetSecond() == nullptr || !Is<Cell>(list->GetSecond())) {
                throw RuntimeError("Invalid operands");
            }
            list = As<Cell>(list->GetSecond());
            --ind;
        }
        return list->GetFirst();
    }
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> Do(std::shared_ptr<Object> ptr) override {
        if (ptr == nullptr || !Is<Cell>(ptr)) {
            throw RuntimeError("Invalid operands");
        }
        auto pair = As<Cell>(ptr);
        if (pair->GetSecond() == nullptr || !Is<Cell>(pair->GetSecond()) ||
            As<Cell>(pair->GetSecond())->GetSecond() != nullptr ||
            As<Cell>(pair->GetSecond())->GetFirst() == nullptr ||
            !Is<Number>(As<Cell>(pair->GetSecond())->GetFirst())) {
            throw RuntimeError("Invalid operands");
        }
        auto ind = As<Number>(As<Cell>(pair->GetSecond())->GetFirst())->GetValue();
        if (!Is<Cell>(pair->GetFirst()) || ind < 0) {
            throw RuntimeError("Invalid operands");
        }
        --ind;
        auto list = As<Cell>(pair->GetFirst());
        while (ind > 0) {
            if (list->GetSecond() == nullptr || !Is<Cell>(list->GetSecond())) {
                throw RuntimeError("Invalid operands");
            }
            list = As<Cell>(list->GetSecond());
            --ind;
        }
        return list->GetSecond();
    }
};

std::unique_ptr<Function> FunctionCreator(const std::string& func_string) {
    if (func_string == "quote") {
        return std::make_unique<QuoteFunction>();
    } else if (func_string == "number?") {
        return std::make_unique<IsNumberFunction>();
    } else if (func_string == "=") {
        return std::make_unique<EqualFunction>();
    } else if (func_string == ">") {
        return std::make_unique<MoreFunction>();
    } else if (func_string == "<") {
        return std::make_unique<LessFunction>();
    } else if (func_string == ">=") {
        return std::make_unique<MoreOrEqualFunction>();
    } else if (func_string == "<=") {
        return std::make_unique<LessOrEqualFunction>();
    } else if (func_string == "+") {
        return std::make_unique<SumFunction>();
    } else if (func_string == "-") {
        return std::make_unique<SubstitutionFunction>();
    } else if (func_string == "*") {
        return std::make_unique<MultiplicationFunction>();
    } else if (func_string == "/") {
        return std::make_unique<DivideFunction>();
    } else if (func_string == "max") {
        return std::make_unique<MaxFunction>();
    } else if (func_string == "min") {
        return std::make_unique<MinFunction>();
    } else if (func_string == "abs") {
        return std::make_unique<AbsFunction>();
    } else if (func_string == "boolean?") {
        return std::make_unique<IsBooleanFunction>();
    } else if (func_string == "not") {
        return std::make_unique<NotFunctioon>();
    } else if (func_string == "and") {
        return std::make_unique<AndFunction>();
    } else if (func_string == "or") {
        return std::make_unique<OrFunction>();
    } else if (func_string == "pair?") {
        return std::make_unique<IsPairFunction>();
    } else if (func_string == "null?") {
        return std::make_unique<IsNullFunctioon>();
    } else if (func_string == "list?") {
        return std::make_unique<IsListFunction>();
    } else if (func_string == "cons") {
        return std::make_unique<ConsFunction>();
    } else if (func_string == "car") {
        return std::make_unique<CarFunction>();
    } else if (func_string == "cdr") {
        return std::make_unique<CdrFunction>();
    } else if (func_string == "list") {
        return std::make_unique<ListFunction>();
    } else if (func_string == "list-ref") {
        return std::make_unique<ListRefFunction>();
    } else if (func_string == "list-tail") {
        return std::make_unique<ListTail>();
    }
    return nullptr;
}

std::shared_ptr<Object> Count(std::shared_ptr<Object> tree) {
    if (tree == nullptr) {
        throw RuntimeError("Invalid syntax");
    }
    if (!Is<Cell>(tree)) {
        return tree;
    }
    auto pair = As<Cell>(tree);
    auto first_arg = RealCount(pair->GetFirst()).first;
    if (first_arg == nullptr || !Is<Symbol>(first_arg) ||
        FunctionCreator(As<Symbol>(first_arg)->GetName()) == nullptr) {
        throw RuntimeError("Invalid operands, there should be a function first");
    }
    auto symb = As<Symbol>(first_arg);
    auto func = FunctionCreator(symb->GetName());
    if (func->IsBoolean()) {
        return func->Do(pair->GetSecond());
    }
    return func->Do(RealCount(pair->GetSecond(), func->IsQuote()).first);
}

std::pair<std::shared_ptr<Object>, bool> RealCount(std::shared_ptr<Object> tree, bool isquote) {
    if (tree == nullptr || !Is<Cell>(tree)) {
        return std::make_pair(tree, false);
    }
    auto first = RealCount(As<Cell>(tree)->GetFirst());
    if (first.first != nullptr && Is<Cell>(first.first) && !isquote && !first.second) {
        throw RuntimeError("Invalid syntax");
    }
    auto pair = std::make_shared<Cell>(first.first, As<Cell>(tree)->GetSecond());
    if (pair->GetFirst() != nullptr && Is<Symbol>(pair->GetFirst()) &&
        FunctionCreator(As<Symbol>(pair->GetFirst())->GetName()) != nullptr) {
        auto symb = As<Symbol>(pair->GetFirst());
        auto func = FunctionCreator(symb->GetName());
        if (func->IsBoolean()) {
            return std::make_pair(func->Do(pair->GetSecond()), false);
        }
        return std::make_pair(func->Do(RealCount(pair->GetSecond(), func->IsQuote()).first),
                              func->IsQuote());
    }
    return std::make_pair(
        std::make_shared<Cell>(pair->GetFirst(), RealCount(pair->GetSecond()).first), false);
}

std::string Interpreter::Run(const std::string& string) {
    auto tree = ReadAll(string);
    auto count = Count(tree);
    std::string ans;
    OutputFirst(count, ans);
    return ans;
}
