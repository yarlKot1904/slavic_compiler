//
// Created by yarlkot on 11.08.24.
//
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <locale>


enum class TokenType {
    _exit,
    bool_lit,
    int_lit,
    semi,
    eq,
    plus,
    star,
    minus,
    slash,
    proc,
    open_paren,
    close_paren,
    open_bracket,
    close_bracket,
    ident,
    var
};

bool isBinOperator(TokenType type);

std::optional<int> binPrec(TokenType type);


struct Token {
    TokenType type;
    std::optional<std::wstring> value;
};

class Tokenizer {
public:
    inline explicit Tokenizer(std::wstring src)
            : m_src(std::move(src)) {
    }

    inline std::vector<Token> tokenize() {
        std::vector<Token> result;
        std::wstring buf;
        std::locale loc("ru_RU.UTF-8");

        while (peak().has_value()) {
            if (std::iswspace(peak().value())) {
                consume();
                continue;
            }

            if (std::iswalpha(peak().value())) {
                buf.push_back(consume());
                while (peak().has_value() && std::iswalnum(peak().value())) {
                    buf.push_back(consume());
                }
                if (buf == L"откланяться") {
                    result.push_back({.type = TokenType::_exit});
                    buf.clear();
                    continue;
                } else if (buf == L"лар") {
                    result.push_back({.type = TokenType::var});
                    buf.clear();
                    continue;
                } else if (buf == L"правда") {
                    result.push_back({.type = TokenType::bool_lit, .value = L"true"});
                    buf.clear();
                    continue;
                } else if (buf == L"кривда") {
                    result.push_back({.type = TokenType::bool_lit, .value=L"false"});
                    buf.clear();
                    continue;
                } else {
                    result.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
                    continue;
                }
                continue;
            }
            if (std::iswdigit(peak().value())) {
                while (peak().has_value() && std::iswdigit(peak().value())) {
                    buf.push_back(consume());
                }
                result.push_back({.type=TokenType::int_lit, .value = buf});
                buf.clear();
                continue;
            }
            switch (peak().value()) {
                case L';': {
                    result.push_back({.type = TokenType::semi});
                    consume();
                    continue;
                }
                case L'(': {
                    result.push_back({.type = TokenType::open_paren});
                    consume();
                    continue;
                }
                case L')': {
                    result.push_back({.type = TokenType::close_paren});
                    consume();
                    continue;
                }
                case L'{': {
                    result.push_back({.type = TokenType::open_bracket});
                    consume();
                    continue;
                }
                case L'}': {
                    result.push_back({.type = TokenType::close_bracket});
                    consume();
                    continue;
                }
                case L'=': {
                    result.push_back({.type = TokenType::eq});
                    consume();
                    continue;
                }
                case L'+': {
                    result.push_back({.type = TokenType::plus});
                    consume();
                    continue;
                }
                case L'-': {
                    result.push_back({.type = TokenType::minus});
                    consume();
                    continue;
                }
                case L'*': {
                    result.push_back({.type = TokenType::star});
                    consume();
                    continue;
                }
                case L'/': {
                    result.push_back({.type = TokenType::slash});
                    consume();
                    continue;
                }
                case L'%': {
                    result.push_back({.type = TokenType::proc});
                    consume();
                    continue;
                }

            }
            std::wcerr << L"Ошибся, молодец при разборе" << std::endl;
            exit(EXIT_FAILURE);
        }

        m_index = 0;

        return result;
    }

private:
    [[nodiscard]] std::optional<wchar_t> peak(int ahead = 0) const;

    wchar_t consume();

    const std::wstring m_src;
    int m_index = 0;
};