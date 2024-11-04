#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <unordered_set>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

enum class BooleanToken { TRUE, FALSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    Token cur_token_{};
    std::istream* in_;
    bool is_ended_ = false;
    std::unordered_set<char> symbols_allowed_ = {'<', '=', '>', '*', '/', '#',
                                                 '?', '!', '-', '[', ']'};
    std::unordered_set<char> symbols_allowed_begin_ = {'<', '=', '>', '*', '/', '#', '[', ']'};
    char x_{};
};