#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

// The stack for the program. Since everything pushed to the stack will be a
// character anyway, I just use a string for the stack
std::string stack;

// The current definitions for every character
std::unordered_map<char, std::vector<std::function<void()>>> instruction_defs;

template <char c>
void push_digit() {
    stack.back() = (stack.back() * 10) + c;
}

template <char c>
void push_char() {
    stack.push_back(c);
}

void output() {
    std::cout << stack.back();
    stack.pop_back();
}

void init_interpreter() {
    instruction_defs = {
        {'#', {push_char<'\0'>}},
        {';', {push_char<';'>}},
        {'0', {push_digit<0>}},
        {'1', {push_digit<1>}},
        {'2', {push_digit<2>}},
        {'3', {push_digit<3>}},
        {'4', {push_digit<4>}},
        {'5', {push_digit<5>}},
        {'6', {push_digit<6>}},
        {'7', {push_digit<7>}},
        {'8', {push_digit<8>}},
        {'9', {push_digit<9>}},
        {'.', {output}}
    };
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "Please provide an Emmental file to interpret\n";
        return 1;
    }

    std::ifstream file{argv[1]};
    if (not file.is_open()) {
        std::cerr << "Unable to open " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    init_interpreter();
    for (auto c: code) {
        for (auto func: instruction_defs[c]) {
            func();
        }
    }
}
