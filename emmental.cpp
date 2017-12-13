#include <functional>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <vector>

// The stack for the program. Since everything pushed to the stack will be a
// character anyway, I just use a string for the stack
std::string stack;

// The queue for the Emmental program
std::queue<char> queue;

// The current definitions for every character
std::unordered_map<char, std::vector<void(*)()>> instruction_defs;

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

void redefine() {
    auto to_define = stack.back();
    stack.pop_back();
    auto index = stack.find_last_of(';');
    auto new_def = stack.substr(index + 1);
    stack.erase(index);
    std::vector<void(*)()> function;
    for (auto c: new_def) {
        auto &instruction = instruction_defs[c];
        function.insert(function.end(), instruction.begin(), instruction.end());
    }
    instruction_defs[to_define] = function;
}

void execute() {
    bool tail_end_recursion;
    do {
        auto to_execute = instruction_defs[stack.back()];
        stack.pop_back();

        tail_end_recursion = false;
        for (auto it = to_execute.begin(); it != to_execute.end(); ++it) {
            if (*it == execute and it + 1 == to_execute.end()) {
                tail_end_recursion = true;
                break;
            } else {
                (*it)();
            }
        }
    } while (tail_end_recursion);
}

void enqueue() {
    queue.push(stack.back());
}

void dequeue() {
    stack.push_back(queue.front());
    queue.pop();
}

void dup() {
    stack.push_back(stack.back());
}

void plus() {
    auto c1 = stack.back();
    stack.pop_back();
    auto c2 = stack.back();
    stack.pop_back();
    stack.push_back(c1 + c2);
}

void minus() {
    auto c1 = stack.back();
    stack.pop_back();
    auto c2 = stack.back();
    stack.pop_back();
    stack.push_back(c2 - c1);
}

void discrete_log() {
    unsigned char c = stack.back();
    stack.pop_back();
    char log_result = 0;
    if (c == 0) {
        log_result = 8;
    } else {
        while (c >>= 1) {
            log_result++;
        }
    }
    stack.push_back(log_result);
}

void input() {
    stack.push_back(std::cin.get());
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
        {'.', {output}},
        {'!', {redefine}},
        {'?', {execute}},
        {'^', {enqueue}},
        {'v', {dequeue}},
        {':', {dup}},
        {'+', {plus}},
        {'-', {minus}},
        {'~', {discrete_log}},
        {',', {input}},
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
