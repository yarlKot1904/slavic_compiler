//
// Created by yarlkot on 11.08.24.
//

#pragma once

#include <sstream>
#include <codecvt>
#include <unordered_map>
#include <cassert>
#include <map>
#include <algorithm>

#include "Parser.hpp"

std::wstring ms2ws(const std::string &str);

std::string mws2s(const std::wstring &wstr);

enum class VarType {
    _int,
    _bool
};

class Generator {
public:
    std::stringstream output;

    inline explicit Generator(Node::Prog prog) :
            m_prog(std::move(prog)) {
    };

    VarType getType(const Node::StatementVar *var) {
        auto res = VarType::_int;

        struct StatementVarVisitor {
            Generator *m_gen;
            VarType *res;

            void operator()(const Node::Term *term) {
                if (auto termBool = std::get_if<Node::TermBoolLit *>(&term->var))
                    *res = VarType::_bool;
                else
                    *res = VarType::_int;
            }

            void operator()(const Node::ExprBin *expr) {
                *res = VarType::_int;
            }

        };


        StatementVarVisitor visitor{.m_gen = this, .res = &res};
        std::visit(visitor, var->expr->var);
        return res;
    }

    void genStatement(const Node::Statement *statement) {
        struct StatementVisitor {
            Generator *m_gen;


            void operator()(const Node::StatementExit *statementExit) const {
                m_gen->genExpression(statementExit->expr);
                m_gen->output << "    mov rax, 60\n";
                m_gen->pop("rdi");
                m_gen->output << "    syscall\n";
            }

            void operator()(const Node::StatementVar *statementVar) const {
                auto it = std::find_if(m_gen->m_vars.cbegin(), m_gen->m_vars.cend(), [&](const Var &var) {
                    return var.name == statementVar->ident.value.value();
                });
                if (it != m_gen->m_vars.cend()) {
                    std::cerr << "Ошибся, молодец, не может быть двух ларцов, да с именем одним" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto type = m_gen->getType(statementVar);

                m_gen->m_vars.push_back(
                        Var{.name = statementVar->ident.value.value(), .stack_loc = m_gen->m_stackSize, .type = type, .size = m_gen->getTypeSize(
                                type)});
                m_gen->genExpression(statementVar->expr);
            }

            void operator()(const Node::StatementScope *statementScope) const {
                m_gen->beginScope();
                for (const Node::Statement *stmt: statementScope->stmts) {
                    m_gen->genStatement(stmt);
                }
                m_gen->endScope();
            }
        };

        StatementVisitor visitor{.m_gen = this};
        std::visit(visitor, statement->var);
    }

    void genTerm(const Node::Term *term) {
        struct TermVisitor {
            Generator *m_gen;

            explicit TermVisitor(Generator *gen) : m_gen(gen) {

            }

            void operator()(const Node::TermIntLit *exprIntLit) const {
                m_gen->output << "    mov rax, " << mws2s(exprIntLit->int_lit.value.value()) << "\n";
                m_gen->push("rax");
            }

            void operator()(const Node::TermBoolLit *exprBoolLit) const {
                std::string res;
                if (exprBoolLit->bool_lit.value == L"false")
                    res = "0";
                else
                    res = "1";
                m_gen->output << "    mov ax, " << res << "\n";
                m_gen->push("ax");
            }

            void operator()(const Node::NodeTermIdent *exprIdent) const {
                auto it = std::find_if(m_gen->m_vars.cbegin(), m_gen->m_vars.cend(), [&](const Var &var) {
                    return var.name == exprIdent->ident.value.value();
                });
                if (it == m_gen->m_vars.cend()) {
                    std::cerr << "Негоже это, ларец, да необъявленный искать. Сперва объявить надо." << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::stringstream offset;
                offset << "QWORD [rsp + " << (m_gen->m_stackSize - (*it).stack_loc - 1) * 8 << "]";
                m_gen->push(offset.str());
            }

            void operator()(const Node::TermParen *termParen) const {
                m_gen->genExpression(termParen->expr);
            }
        };

        TermVisitor visitor(this);
        std::visit(visitor, term->var);
    }

    void genBinExpr(const Node::ExprBin *binExpr) {
        struct BinExprVisitor {
            Generator *m_gen;

            explicit BinExprVisitor(Generator *gen) : m_gen(gen) {

            }

            void operator()(const Node::ExprBinAdd *add) const {
                m_gen->genExpression(add->rhs);
                m_gen->genExpression(add->lhs);
                m_gen->pop("rax");
                m_gen->pop("rbx");
                m_gen->output << "    add rax, rbx\n";
                m_gen->push("rax");
            };

            void operator()(const Node::ExprBinSub *sub) const {
                m_gen->genExpression(sub->rhs);
                m_gen->genExpression(sub->lhs);
                m_gen->pop("rax");
                m_gen->pop("rbx");
                m_gen->output << "    sub rax, rbx\n";
                m_gen->push("rax");
            };

            void operator()(const Node::ExprBinMulti *mul) const {
                m_gen->genExpression(mul->rhs);
                m_gen->genExpression(mul->lhs);
                m_gen->pop("rax");
                m_gen->pop("rbx");
                m_gen->output << "    mul rbx\n";
                m_gen->push("rax");
            };

            void operator()(const Node::ExprBinDiv *divi) const {
                m_gen->genExpression(divi->rhs);
                m_gen->genExpression(divi->lhs);
                m_gen->pop("rax");
                m_gen->pop("rbx");
                m_gen->output << "    xor rdx, rdx\n";
                m_gen->output << "    div rbx\n";
                m_gen->push("rax");
            };

            void operator()(const Node::ExprBinProc *proc) const {
                m_gen->genExpression(proc->rhs);
                m_gen->genExpression(proc->lhs);
                m_gen->pop("rax");
                m_gen->pop("rbx");
                m_gen->output << "    xor rdx, rdx\n";
                m_gen->output << "    div rbx\n";
                m_gen->push("rdx");
            };
        };

        BinExprVisitor visitor(this);
        std::visit(visitor, binExpr->var);
    }

    void genExpression(const Node::Expr *expr) {
        struct ExprVisitor {
            Generator *m_gen;

            explicit ExprVisitor(Generator *gen) : m_gen(gen) {

            }

            void operator()(const Node::Term *term) const {
                m_gen->genTerm(term);
            }

            void operator()(const Node::ExprBin *binExpr) const {
                m_gen->genBinExpr((binExpr));

            }
        };
        ExprVisitor visitor(this);
        std::visit(visitor, expr->var);
    }

    [[nodiscard]] std::string genProg() {
        output << "global _start\n"
                  "_start:\n";
        for (const Node::Statement *statement: m_prog.statements) {
            genStatement(statement);
        }


        output << "    mov rax, 60\n";
        output << "    mov rdi, 0\n";
        output << "    syscall";
        return output.str();
    }

private:

    void push(const std::string &reg) {
        output << "    push " << reg << "\n";
        m_stackSize++;
    }

    void pop(const std::string &reg) {
        output << "    pop " << reg << "\n";
        m_stackSize--;
    }

    void beginScope() {
        m_scopes.push_back(m_vars.size());
    }

    void endScope() {
        size_t popC = 0;
        for (int i = m_scopes.back(); i < m_vars.size(); i++) {
            popC += m_vars[i].size;
        }
        size_t popCount = m_vars.size() - m_scopes.back();
        output << "    add rsp, " << popC << "\n";
        m_stackSize -= popCount;
        for (int i = 0; i < popCount; ++i) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }


    struct Var {
        std::wstring name;
        size_t stack_loc;
        VarType type;
        size_t size;
    };


    const Node::Prog m_prog;

    size_t m_stackSize = 0;
    std::vector<Var> m_vars{};
    std::vector<size_t> m_scopes{};

    size_t getTypeSize(VarType type) {
        switch (type) {
            case VarType::_int: {
                return 8;
            }
            case VarType::_bool: {
                return 2;
            }
        }
    }
};


