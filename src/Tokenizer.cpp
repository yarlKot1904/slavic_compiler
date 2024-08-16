//
// Created by yarlkot on 11.08.24.
//

#include "Tokenizer.hpp"


std::optional<wchar_t> Tokenizer::peak(int ahead) const {
    if (m_index + ahead >= m_src.length())
        return {};
    else {
        return m_src[m_index + ahead];
    }
}

wchar_t Tokenizer::consume() {
    return m_src.at(m_index++);
}

bool isBinOperator(TokenType type) {
    switch (type) {
        case TokenType::plus:
        case TokenType::star:
            return true;
        default:
            return false;
    }
}

std::optional<int> binPrec(TokenType type) {
    switch (type) {
        case TokenType::plus:
        case TokenType::minus:
            return 1;
        case TokenType::star:
        case TokenType::slash:
        case TokenType::proc:
            return 2;
        default:
            return {};
    }
}
