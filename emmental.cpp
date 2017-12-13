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

// Function template that multiplies the top value of stack by 10, and adds
// a number to it. Used to implement the 0 through 9 instructions
template <char c>
void push_digit() {
    stack.back() = (stack.back() * 10) + c;
}

// Pushes a character to the stack. Used to implement the ; and # instructions
template <char c>
void push_char() {
    stack.push_back(c);
}

// Outputs the top character from the stack, and pops it
// Used to implement the . instruction
void output() {
    std::cout << stack.back();
    stack.pop_back();
}

// Pops a symbol from the stack, and redefines it to be the sequence of
// instructions currently on stack, terminated by a ;
// Used to implement the ! command
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

// Pops a symbol from the stack and executes it with its current meaning
// Used to implement the ? command
void execute() {
    bool tail_end_recursion;
    // This function is written somewhat weirdly so as to prevent tail calls
    // from growing the stack. This is very important because recursion is the
    // only way to have a loop in Emmental, and the program would quickly
    // overflow the stack without elimination of tail calls
    do {
        auto to_execute = instruction_defs[stack.back()];
        stack.pop_back();

        tail_end_recursion = false;
        for (auto it = to_execute.begin(); it != to_execute.end(); ++it) {
            // If the next function is an execute instruction, and it's the
            // last function in our list, we just go back to the beginning
            // of this function to prevent growing the call stack
            if (*it == execute and it + 1 == to_execute.end()) {
                tail_end_recursion = true;
                break;
            // Otherwise, we call the function normally
            } else {
                (*it)();
            }
        }
    } while (tail_end_recursion);
}

// Pushes the top symbol of the stack to the queue.
// Used to implement the ^ instruction
void enqueue() {
    queue.push(stack.back());
}

// Dequeues the front symbol of the queue and pushes it to the stack.
// Used to implement the v instruction
void dequeue() {
    stack.push_back(queue.front());
    queue.pop();
}

// Duplicates the top symbol of the stack. Used to implement the : instruction
void dup() {
    stack.push_back(stack.back());
}

// Pop two symbols, adds them, and pushes the result to the stack
// Used to implement the + instruction
void plus() {
    auto c1 = stack.back();
    stack.pop_back();
    auto c2 = stack.back();
    stack.pop_back();
    stack.push_back(c1 + c2);
}

// Pops two symbols, subtracts the first from the second, then pushes the result
// Used to implement the - instruction
void minus() {
    auto c1 = stack.back();
    stack.pop_back();
    auto c2 = stack.back();
    stack.pop_back();
    stack.push_back(c2 - c1);
}

// Pops a symbol from the stack, and pops the base 2 log of that symbol, with 0
// being a special case that pushes 8
// Used to implement the ~ instruction
void discrete_log() {
    unsigned char c = stack.back();
    stack.pop_back();
    char log_result = 0;
    if (c == 0) {
        log_result = 8;
    } else {
        // If the symbol is not 1, this finds the index of the highest set bit
        // by right shifting until c == 0, which is the equivalent of log base
        // 2 of c
        while (c >>= 1) {
            log_result++;
        }
    }
    stack.push_back(log_result);
}

// Pushes a character of input to the stack. Used to implement ,
void input() {
    stack.push_back(std::cin.get());
}

// Sets up the instructions with their initial definitions
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
