#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Empty tokenizer");
    }
    auto token = tokenizer->GetToken();
    if (ConstantToken* x = std::get_if<ConstantToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Number>(x->value);
    }
    if (SymbolToken* x = std::get_if<SymbolToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(x->name);
    }
    if (BracketToken* x = std::get_if<BracketToken>(&token)) {
        tokenizer->Next();
        if (*x == BracketToken::OPEN) {
            return ReadLine(tokenizer);
        }
        return std::make_shared<Symbol>(")");
    }
    if (DotToken* x = std::get_if<DotToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(".");
    }
    if (QuoteToken* x = std::get_if<QuoteToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                                      std::make_shared<Cell>(Read(tokenizer), nullptr));
    }
    if (BooleanToken* x = std::get_if<BooleanToken>(&token)) {
        tokenizer->Next();
        if (*x == BooleanToken::TRUE) {
            return std::make_shared<Symbol>("#t");
        } else {
            return std::make_shared<Symbol>("#f");
        }
    }
    tokenizer->Next();
    return nullptr;
}

std::shared_ptr<Object> ReadLine(Tokenizer* tokenizer) {
    auto read = Read(tokenizer);
    if (Is<Symbol>(read) && As<Symbol>(read)->GetName() == ")") {
        return nullptr;
    }
    if (Is<Symbol>(read) && As<Symbol>(read)->GetName() == ".") {
        auto new_read = Read(tokenizer);
        auto close_brace = Read(tokenizer);
        if (!Is<Symbol>(close_brace) || As<Symbol>(close_brace)->GetName() != ")") {
            throw SyntaxError("There should be )");
        }
        return new_read;
    }
    return std::make_shared<Cell>(read, ReadLine(tokenizer));
}
