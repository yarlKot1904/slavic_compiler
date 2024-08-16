//
// Created by yarlkot on 11.08.24.
//

#include "Parser.hpp"

std::optional<Token> Parser::peak(int ahead) const {
    if(m_index + ahead >= m_tokens.size()){
        return {};
    } else{
        return m_tokens[m_index + ahead];
    }
}

Token Parser::consume() {
    return m_tokens[m_index++];
}
