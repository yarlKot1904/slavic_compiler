#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include "Tokenizer.hpp"
#include "Generator.hpp"

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    if (argc < 2) {
        std::cerr << "Ошибся, молодец" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::wstringstream stream;

    {
        std::wifstream input(argv[1], std::ios::in);
        input.imbue(std::locale("ru_RU.UTF-8"));
        stream << input.rdbuf();
        input.close();
    }

    std::wstring context_string = stream.str();

    std::wcout << context_string << std::endl;
    Tokenizer tokenizer = Tokenizer(context_string);
    auto tokens = tokenizer.tokenize();
    Parser parser = Parser(std::move(tokens));
    std::optional<Node::Prog> prog = parser.parseProgram();
    if(!prog.has_value()){
        std::cerr << "Ошибся, молодец. Древо без корня вышло" << std::endl;
    }
    Generator generator = Generator(prog.value());
    std::string asmString = generator.genProg();
    std::wcout << std::wstring(asmString.begin(), asmString.end()) << std::endl;
    std::ofstream asmWriter("../build/out.asm");
    asmWriter << asmString;
    asmWriter.close();

    system("nasm -f elf64 -o ../build/out.o ../build/out.asm && ld -o ../build/out ../build/out.o");

    std::wcout << L"Готова твоя программа" << std::endl;

    exit(EXIT_SUCCESS);
}