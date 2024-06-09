#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>
#include <format>
#include <optional>
#include <deque>
#include <unordered_map>
#include <span>
#include <cstdio>
#include <algorithm>
#include <iterator>

#include "raylib.h"
#include "types.hh"
#include "magic_enum.hpp"

// Virtual machine and its types' methods should be pure.

using ParseError = std::runtime_error;
using VmError = std::runtime_error;

enum class Opcode : int
{
    PUSH,
    ADD,
    CALL,
};

class Command
{
public:
    Opcode code;
    Any value;
    
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd) {
        os << magic_enum::enum_name<Opcode>(cmd.code) << " " << cmd.value;
        return os;
    }
};

using HookParams = std::span<Any>;
using HookFunc = std::function<Any(const HookParams&)>;
struct Hook {
    int argc; // -1 for infinite
    HookFunc func;
    std::string name;
    
    Any Call(const HookParams& params) const {
        if (argc != -1 && params.size() != argc) {
            throw VmError(std::format("Function {} expects {} args but {} were given.",
                                      name, argc, params.size()));
        }
        return func(params);
    }
};

class Externals
{
public:
    std::unordered_map<std::string, Hook> hooks;
    
    Externals() {
        // add standard library
        Register("print", Print, -1);
        Register("println", PrintLn, -1);
        Register("throw", Throw, -1);
    }
    
    void Register(const std::string& name, HookFunc func, int paramCount = -1) {
        hooks[name] = Hook{ paramCount, func };
    }
    
    Hook GetHook(const std::string& name) const {
        try {
            return hooks.at(name);
        } catch (std::range_error& ex) {
            throw("No external hook found with name: {}", name);
        }
    }
    
private:
    static Any Throw(const HookParams& args) {
        try {
            Any base;
            for (const Any& a: args) {
                base = base + a;
            }
            throw VmError(base.ToString());
        } catch (OperationError& ex) {
            std::stringstream ss;
            ss << "Error: ";
            for (const Any& a: args) {
                ss << a << " ";
            }
            throw VmError(ss.str());
        }
    }
    
    static void _BasePrint(const HookParams& args) {
        if (args.size() == 1) {
            std::cout << args[0];
        } else {
            std::string formatted = FormatWithVector(args[0].ToString(),
                                                     std::span(args.begin() + 1, args.end()));
            std::cout << formatted;
        }
    }
    
    static Any Print(const HookParams& args) {
        _BasePrint(args);
        return {};
    }
    
    static Any PrintLn(const HookParams& args) {
        _BasePrint(args);
        std::cout << "\n";
        return {};
    }
    
    static std::string FormatWithVector(const std::string& format,
                                        const std::span<Any>& args) {
        std::stringstream ss;
        size_t argIndex = 0;

        for (char ch : format) {
            if (ch == '{' && argIndex < args.size()) {
                const Any& arg = args[argIndex++];
                ss << arg;
            } else if (ch != '}') {
                ss << ch;
            }
        }
        return ss.str();
    }
};

// TODO: make all methods static
class VirtualMachine
{
public:
    VirtualMachine(const std::string& name = "")
        : name(name)
    {
    }
    
    [[nodiscard]] VirtualMachine Execute(const Command& cmd) const {
        VirtualMachine m = *this;
        
        switch (cmd.code) {
            case Opcode::PUSH: {
                m = m.PushStack(cmd.value);
                break;
            }
            case Opcode::ADD: {
                Any base;
                while (!m.IsStackEmpty()) {
                    Any value;
                    m = m.PopStack(&value);
                    base = base + value;
                }
                m = m.PushStack(base);
                break;
            }
            case Opcode::CALL: {
                const std::string& func = cmd.value.Extract<std::string>();
                Hook hook = externals.GetHook(func);
                // eat rest stack
                std::vector<Any> args; args.reserve(8);
                while (!m.IsStackEmpty()) {
                    Any arg;
                    m = m.PopStack(&arg);
                    args.emplace_back(arg);
                }
                Any result = hook.Call(args);
                if (!result.IsUndefined()) {
                    m = m.PushStack(result);
                }
                break;
            }
            default: {
                throw VmError(std::format("Opcode {} not implemented.",
                                          magic_enum::enum_name<Opcode>(cmd.code)));
            }
        }
        
        m.history.emplace_front(cmd);
        return m;
    }
    
    bool IsStackEmpty() const {
        return stack.empty();
    }
    
    [[nodiscard]] VirtualMachine PopStack(Any* outValue) const {
        if (IsStackEmpty()) {
            throw VmError("Stack underflow");
        }
        VirtualMachine m = *this;
        Any value = m.stack.back();
        m.stack.pop_back();
        *outValue = value;
        return m;
    }
    
    [[nodiscard]] VirtualMachine PushStack(const Any& value) const {
        VirtualMachine m = *this;
        m.stack.emplace_back(value);
        return m;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const VirtualMachine& vm) {
        os << "Virtual machine";
        if (!vm.name.empty()) {
            os << " - " << vm.name;
        }
        os << "\nStack:" << '\n';
        if (vm.stack.empty()) {
            os << "(empty)" << '\n';
        } else {
            for (const Any& val: vm.stack) {
                os << "- " << val << '\n';
            }
        }
        os << "\nHistory:" << '\n';
        if (vm.history.empty()) {
            os << "(empty)" << '\n';
        } else {
            for (const Command& cmd: vm.history) {
                os << "- " << cmd << '\n';
            }
        }
        return os;
    }
    
private:
    std::string name;
    std::deque<Command> history{};
    std::vector<Any> stack{};
    Externals externals{};
};

constexpr bool StringsEqualInsensitive(const std::string_view& str1,
                                       const std::string_view& str2)
{
    if (str1.length() != str2.length())
        return false;

    for (int i = 0; i < str1.length(); ++i) {
        if (std::tolower(str1[i]) != std::tolower(str2[i]))
            return false;
    }

    return true;
}

// TODO: unit test
std::optional<Command> ParseLine(const std::string_view& line)
{
    constexpr const char* TRIMMED = " \t\n\r\f\v";
    
    size_t start = line.find_first_not_of(TRIMMED);
    if (start == std::string_view::npos) {
        return std::nullopt;
    }

    size_t end = line.find_last_not_of(TRIMMED);
    
    std::string_view first = std::string_view(line.data() + start, end - start + 1);
    end = first.find_last_not_of(TRIMMED);
    
    std::string_view second = "";
    
    size_t pos = first.find(' ');
    if (pos != std::string::npos) {
        first = std::string_view(line.data() + start, pos);
        second = std::string_view(first.data() + pos + 1, end - pos);
    }
    
    // check what opcode to run
    for (const auto& [code, codeStr] : magic_enum::enum_entries<Opcode>())
    {
        if (StringsEqualInsensitive(first, codeStr))
        {            
            return Command{ code, Any::Parse(std::string(second)) };
        }
    }
    throw ParseError(std::format("No opcode with value: {}", first));
}

constexpr const char* script = R"(
    PUSH 5
    PUSH 7
    ADD
    CALL println
)";

void RunScript(const std::string_view& script)
{
    std::istringstream iss(script.data());

    std::vector<Command> cmds;
    cmds.reserve(1024);
    
    std::string line;
    while (std::getline(iss, line)) {
        std::optional<Command> cmd = ParseLine(line);
        if (cmd.has_value()) {
            cmds.emplace_back(*cmd);
        }
    }
    
    VirtualMachine vm{};
    
    for (const Command& cmd: cmds) {
        vm = vm.Execute(cmd);
    }
    
    std::cout << vm << '\n';
}

void RunRepl()
{
    std::cout << "Theatre Script REPL" << std::endl;
    std::cout << "TASM mode" << std::endl;
    
    std::string input;
    
    const auto IsCmd = [&](const std::string_view& cmd) {
        return StringsEqualInsensitive(input, cmd);
    };
    
    VirtualMachine vm {};
    
    while (!IsCmd("exit") && !IsCmd("quit") && !IsCmd("q")) {
        std::cin.clear();
        std::getline(std::cin, input);
        
        try {
            std::optional<Command> cmd = ParseLine(input);
            if (!cmd.has_value()) {
                throw ParseError("Empty line");
            }
            vm = vm.Execute(*cmd);
        } catch (std::exception& ex) {
            std::cerr << ex.what() << '\n';
        }
    }
}


void StartGame();

int main(int argc, char** argv)
{
    // StartGame();
    // RunRepl();
    RunScript(script);
    return 0;
}

void StartGame()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        }
        EndDrawing();
    }

 
    CloseWindow();
}
