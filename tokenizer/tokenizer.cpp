#include <tokenizer.h>
#include <error.h>

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    x_ = in_->get();
    for (char x = 65; x < 91; ++x) {
        symbols_allowed_.insert(x);
        symbols_allowed_begin_.insert(x);
    }
    for (char x = 97; x < 123; ++x) {
        symbols_allowed_.insert(x);
        symbols_allowed_begin_.insert(x);
    }
    for (char x = 48; x < 58; ++x) {
        symbols_allowed_.insert(x);
    }
    Next();
}

bool Tokenizer::IsEnd() {
    return is_ended_;
}

Token Tokenizer::GetToken() {
    return cur_token_;
}

void Tokenizer::Next() {
    if (x_ == std::istream::traits_type::eof()) {
        is_ended_ = true;
        return;
    }
    if (isspace(x_)) {
        x_ = in_->get();
        Next();
        return;
    }
    if (x_ == '(') {
        cur_token_ = BracketToken::OPEN;
        x_ = in_->get();
        return;
    }
    if (x_ == ')') {
        cur_token_ = BracketToken::CLOSE;
        x_ = in_->get();
        return;
    }
    if (x_ == '\'') {
        cur_token_ = QuoteToken();
        x_ = in_->get();
        return;
    }
    if (x_ == '.') {
        cur_token_ = DotToken();
        x_ = in_->get();
        return;
    }
    if (isdigit(x_)) {
        std::string num;
        num += x_;
        x_ = in_->get();
        while (x_ != std::istream::traits_type::eof() && isdigit(x_)) {
            num += x_;
            x_ = in_->get();
        }
        cur_token_ = ConstantToken{std::stoi(num)};
        return;
    }
    if (x_ == '+') {
        x_ = in_->get();
        if (x_ == ' ') {
            cur_token_ = SymbolToken{"+"};
            x_ = in_->get();
            return;
        }
        if (x_ == std::istream::traits_type::eof()) {
            cur_token_ = SymbolToken{"+"};
            return;
        }
        if (isdigit(x_)) {
            std::string num;
            num += x_;
            x_ = in_->get();
            while (x_ != std::istream::traits_type::eof() && isdigit(x_)) {
                num += x_;
                x_ = in_->get();
            }
            cur_token_ = ConstantToken{std::stoi(num)};
            return;
        }
        cur_token_ = SymbolToken{"+"};
        return;
    }
    if (x_ == '-') {
        x_ = in_->get();
        if (x_ == ' ') {
            cur_token_ = SymbolToken{"-"};
            x_ = in_->get();
            return;
        }
        if (x_ == std::istream::traits_type::eof()) {
            cur_token_ = SymbolToken{"-"};
            return;
        }
        if (isdigit(x_)) {
            std::string num;
            num += x_;
            x_ = in_->get();
            while (x_ != std::istream::traits_type::eof() && isdigit(x_)) {
                num += x_;
                x_ = in_->get();
            }
            cur_token_ = ConstantToken{std::stoi(num) * (-1)};
            return;
        }
        cur_token_ = SymbolToken{"-"};
        return;
    }
    if (x_ == '#') {
        std::string ans;
        ans += x_;
        x_ = in_->get();
        if (x_ == std::istream::traits_type::eof()) {
            cur_token_ = SymbolToken{ans};
            return;
        }
        if (x_ == ' ') {
            cur_token_ = SymbolToken{ans};
            x_ = in_->get();
            return;
        }
        if (x_ == 't' || x_ == 'f') {
            ans += x_;
            x_ = in_->get();
            if (x_ == std::istream::traits_type::eof()) {
                cur_token_ = ans.back() == 't' ? BooleanToken::TRUE : BooleanToken::FALSE;
                return;
            }
            if (x_ == ' ') {
                cur_token_ = ans.back() == 't' ? BooleanToken::TRUE : BooleanToken::FALSE;
                x_ = in_->get();
                return;
            }
            while (x_ != std::istream::traits_type::eof() &&
                   symbols_allowed_.find(x_) != symbols_allowed_.end()) {
                ans += x_;
                x_ = in_->get();
            }
            cur_token_ = SymbolToken{ans};  // probably add #t(sth incorrect)
            return;
        }
        while (x_ != std::istream::traits_type::eof() &&
               symbols_allowed_.find(x_) != symbols_allowed_.end()) {
            ans += x_;
            x_ = in_->get();
        }
        cur_token_ = SymbolToken{ans};
        return;
    }
    if (symbols_allowed_begin_.find(x_) != symbols_allowed_begin_.end()) {
        std::string ans;
        ans += x_;
        x_ = in_->get();
        while (x_ != std::istream::traits_type::eof() &&
               symbols_allowed_.find(x_) != symbols_allowed_.end()) {
            ans += x_;
            x_ = in_->get();
        }
        cur_token_ = SymbolToken{ans};
        return;
    }
    throw SyntaxError("invalid symbol");
}
