//
// Created by yarlkot on 11.08.24.
//

#pragma once

#define AI ArenaAllocator::Instance()

#include <vector>
#include <variant>
#include <utility>

#include "Tokenizer.hpp"
#include "arena.hpp"

namespace Node {
    struct TermIntLit {
        Token int_lit;
    };

    struct TermBoolLit {
        Token bool_lit;
    };

    struct NodeTermIdent {
        Token ident;
    };


    struct Expr;

    struct ExprBinAdd {
        Expr *lhs;
        Expr *rhs;
    };

    struct ExprBinSub {
        Expr *lhs;
        Expr *rhs;
    };

    struct ExprBinMulti {
        Expr *lhs;
        Expr *rhs;
    };

    struct ExprBinDiv {
        Expr *lhs;
        Expr *rhs;
    };

    struct ExprBinProc {
        Expr *lhs;
        Expr *rhs;
    };

    struct TermParen {
        Expr *expr;
    };

    struct ExprBin {
        std::variant<ExprBinAdd *, ExprBinSub *, ExprBinMulti *, ExprBinDiv *, ExprBinProc *> var;
    };

    struct Term {
        std::variant<TermIntLit *, TermBoolLit *, NodeTermIdent *, TermParen *> var;
    };
    struct Expr {
        std::variant<Term *, ExprBin *> var;
    };

    struct StatementExit {
        Expr *expr;
    };

    struct StatementVar {
        Token ident;
        Expr *expr;
    };
    struct Statement;
    struct StatementScope {
        std::vector<Statement *> stmts;
    };

    struct Statement {
        std::variant<StatementExit *, StatementVar *, StatementScope *> var;
    };

    struct Prog {
        std::vector<Statement *> statements;
    };
}


class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
            : m_tokens(tokens) {

    }

    std::optional<Node::ExprBin *> parseBinExpr() {
        if (auto lhs = parseExpr()) {
            auto binExpr = AI.alloc<Node::ExprBin>();

        } else return {};

    }

    std::optional<Node::Term *> parseTerm() {
        std::wcout << "log " << std::to_underlying(peak().value().type) << std::endl;
        if (auto intLit = try_consume(TokenType::int_lit)) {
            auto termIntLit = AI.alloc<Node::TermIntLit>();
            termIntLit->int_lit = intLit.value();
            auto term = AI.alloc<Node::Term>();
            term->var = termIntLit;
            return term;
        } else if (auto ident = try_consume(TokenType::ident)) {
            auto termIntLit = AI.alloc<Node::NodeTermIdent>();
            termIntLit->ident = ident.value();
            auto term = AI.alloc<Node::Term>();
            term->var = termIntLit;
            return term;
        } else if (auto lit = try_consume(TokenType::bool_lit)) {
            auto termBoolLit = AI.alloc<Node::TermBoolLit>();
            termBoolLit->bool_lit = lit.value();
            auto term = AI.alloc<Node::Term>();
            term->var = termBoolLit;
            return term;
        } else if (auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parseExpr();
            if (!expr.has_value()) {
                std::cerr << "В скобах надобно выражение держать." << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Скобы надобно закрывать.");
            auto termParen = AI.alloc<Node::TermParen>();
            termParen->expr = expr.value();
            auto term = AI.alloc<Node::Term>();
            term->var = termParen;
            return term;
        }
        return {};
    }

    std::optional<Node::Expr *> parseExpr(int minPrec = 0) {
        auto termLhs = parseTerm();

        if (!termLhs.has_value()) {
            return {};
        }
        auto exprLhs = AI.alloc<Node::Expr>();
        exprLhs->var = termLhs.value();
        while (true) {
            std::optional<Token> curToken = peak();
            std::optional<int> prec;
            if (curToken.has_value()) {
                prec = binPrec(curToken->type);
                if (!prec.has_value() || prec < minPrec) {
                    break;
                }

            } else {
                break;
            }
            Token op = consume();

            int nextMinPrec = prec.value() + 1;
            auto exprRhs = parseExpr(nextMinPrec);
            if (!exprRhs.has_value()) {
                std::cerr << "Не запарсить строченьку." << std::endl;
                exit(EXIT_FAILURE);
            }

            auto expr = AI.alloc<Node::ExprBin>();
            auto exprLhs2 = AI.alloc<Node::Expr>();

            switch (op.type) {
                case TokenType::plus: {
                    auto add = AI.alloc<Node::ExprBinAdd>();
                    exprLhs2->var = exprLhs->var;
                    add->lhs = exprLhs2;
                    add->rhs = exprRhs.value();
                    expr->var = add;
                    break;
                }
                case TokenType::minus: {
                    auto sub = AI.alloc<Node::ExprBinSub>();
                    exprLhs2->var = exprLhs->var;
                    sub->lhs = exprLhs2;
                    sub->rhs = exprRhs.value();
                    expr->var = sub;
                    break;
                }

                case TokenType::star: {
                    auto mul = AI.alloc<Node::ExprBinMulti>();
                    exprLhs2->var = exprLhs->var;
                    mul->lhs = exprLhs2;
                    mul->rhs = exprRhs.value();
                    expr->var = mul;
                    break;
                }

                case TokenType::slash: {
                    auto div = AI.alloc<Node::ExprBinDiv>();
                    exprLhs2->var = exprLhs->var;
                    div->lhs = exprLhs2;
                    div->rhs = exprRhs.value();
                    expr->var = div;
                    break;
                }

                case TokenType::proc: {
                    auto proc = AI.alloc<Node::ExprBinProc>();
                    exprLhs2->var = exprLhs->var;
                    proc->lhs = exprLhs2;
                    proc->rhs = exprRhs.value();
                    expr->var = proc;
                    break;
                }
                default: {
                    std::cerr << "Не достает оператора богатырского" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            exprLhs->var = expr;

        }
        return exprLhs;
    }

    std::optional<Node::Statement *> parseStatement() {
        if (!peak().has_value())
            return {};
        auto statementExit = AI.alloc<Node::StatementExit>();

        if (peak().value().type == TokenType::_exit && peak(3).has_value() &&
            peak(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            if (auto node_expr = parseExpr()) {
                statementExit->expr = node_expr.value();
            } else {
                std::cerr << "Ошибся, молодец, выражение негоже составлено" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Ошибся, молодец, выражение негоже составлено. Скобы не достает.");
            try_consume(TokenType::semi,
                        "\"Ошибся, молодец, выражение негоже составлено. Не достает точеньки да с запятой.");
            auto nodeStatement = AI.alloc<Node::Statement>();
            nodeStatement->var = statementExit;
            return nodeStatement;
        } else if (peak().value().type == TokenType::var) {
            consume();
            auto ident = try_consume(TokenType::ident, "Ошибся, молодец, негоже ларец без имени оставлять.");

            auto statementVar = AI.alloc<Node::StatementVar>();
            statementVar->ident = ident;
            try_consume(TokenType::eq, "Ошибся, молодец, негоже ларец не наполнять при созидании.");
            if (auto expr = parseExpr()) {
                statementVar->expr = expr.value();
            } else {
                std::cerr << "Ошибся, молодец, выражение негоже составлено" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi,
                        "Ошибся, молодец, выражение негоже составлено. Не достает точеньки да с запятой.");
            auto nodeStatement = AI.alloc<Node::Statement>();
            nodeStatement->var = statementVar;
            return nodeStatement;
        } else if (auto openBracket = try_consume(TokenType::open_bracket)) {
            auto scope = AI.alloc<Node::StatementScope>();
            while (auto stmt = parseStatement()) {
                scope->stmts.push_back(stmt.value());
            }
            try_consume(TokenType::close_bracket, "Ожидалась скоба фигурная.");
            std::wcout << L"Получилось" << std::endl;
            auto stmt = AI.alloc<Node::Statement>();
            stmt->var = scope;
            return stmt;
        } else {
            return {};
        }
    }

    std::optional<Node::Prog> parseProgram() {
        Node::Prog prog;
        while (peak().has_value()) {
            if (auto stmt = parseStatement()) {
                prog.statements.push_back(stmt.value());
            } else {
                std::cerr << "Ошибся, молодец, выражение негоже составлено" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    const std::vector<Token> m_tokens;

    [[nodiscard]] std::optional<Token> peak(int ahead = 0) const;

    Token consume();

    Token try_consume(TokenType type, const std::string errMsg) {
        if (peak().has_value() && peak().value().type == type) {
            return consume();
        } else {
            std::cerr << errMsg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::optional<Token> try_consume(TokenType type) {
        if (peak().has_value() && peak().value().type == type) {
            return consume();
        } else {
            return {};
        }
    }

    int m_index = 0;
};


