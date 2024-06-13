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

#include "theatre/types.hh"
#include "magic_enum.hpp"

namespace theatre {

// Virtual machine and its types' methods should be pure.

using ParseError = std::runtime_error;
using VmError = std::runtime_error;

enum class Opcode : int
{
    PUSH,
    SUB,
    MUL,
    DIV,
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

class VirtualMachine;
struct HookContext {
    std::ostream& out;
    const std::span<Any>& args;
    VirtualMachine& vm;

    HookContext(VirtualMachine& vm, const std::span<Any>& args, std::ostream& out)
        : vm(vm), args(args), out(out)
        {
        }
};

using HookFunc = std::function<Any(HookContext&&)>;
struct Hook {
    int argc; // -1 for infinite
    HookFunc func;
    std::string name;
    
    Any Call(VirtualMachine* vm, const std::span<Any>& args) const;
};


class VirtualMachine
{
public:
    VirtualMachine(const std::string& name = "", std::ostream& outStream = std::cout)
        : name(name), outStream(&outStream)
    {
    }
    
    void Init() {
        // add standard library
        Register("print", Print, -1);
        Register("println", PrintLn, -1);
        Register("throw", Throw, -1);
    }

    [[nodiscard]] VirtualMachine Execute(const Command& cmd) const {
        VirtualMachine m = *this;
        
        switch (cmd.code) {
            case Opcode::PUSH: {
                m = m.PushStack(cmd.value);
                break;
            }
            case Opcode::ADD: {
                EnsureStackLength(2);
                Any a, b;
                m = m.PopStack(&a);
                m = m.PopStack(&b);
                m = m.PushStack(a + b);
                break;
            }
            case Opcode::SUB: {
                EnsureStackLength(2);
                Any a, b;
                m = m.PopStack(&a);
                m = m.PopStack(&b);
                m = m.PushStack(a - b);
                break;
            }
            case Opcode::MUL: {
                EnsureStackLength(2);
                Any a, b;
                m = m.PopStack(&a);
                m = m.PopStack(&b);
                m = m.PushStack(a * b);
                break;
            }
            case Opcode::DIV: {
                EnsureStackLength(2);
                Any a, b;
                m = m.PopStack(&a);
                m = m.PopStack(&b);
                m = m.PushStack(a / b);
                break;
            }
            case Opcode::CALL: {
                const std::string& func = cmd.value.Extract<std::string>();
                Hook hook = GetHook(func);
                // eat rest stack
                std::vector<Any> args; args.reserve(8);
                while (!m.IsStackEmpty()) {
                    Any arg;
                    m = m.PopStack(&arg);
                    args.emplace_back(arg);
                }
                Any result = hook.Call(&m, args);
                if (!result.IsMono()) {
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
    
    void EnsureStackLength(int argc) const
    {
        if (stack.size() < argc) {
            throw VmError(std::format("Stack underflow. Expected {} items but got {}.",
                                      argc, stack.size()));
        }
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
    
    void Register(const std::string& name, HookFunc func, int paramCount = -1) {
        hooks[name] = Hook{ paramCount, func };
    }
    
    Hook GetHook(const std::string& name) const {
        try {
            return hooks.at(name);
        } catch (std::range_error& ex) {
            throw(std::format("No external hook found with name: {}", name));
        }
    }

    inline std::ostream& GetOutStream() {
        return *outStream;
    }

private:
    std::string name;
    std::deque<Command> history{};
    std::vector<Any> stack{};
    std::unordered_map<std::string, Hook> hooks;
    std::ostream* outStream;

    static Any Throw(HookContext&& ctx) {
        try {
            Any base;
            for (const Any& a: ctx.args) {
                base = base + a;
            }
            throw VmError(base.ToString());
        } catch (OperationError& ex) {
            std::stringstream ss;
            ss << "Error: ";
            for (const Any& a: ctx.args) {
                ss << a << " ";
            }
            throw VmError(ss.str());
        }
    }
    
    static void _BasePrint(HookContext& ctx) {
        if (ctx.args.size() == 1) {
            ctx.out << ctx.args[0];
        } else {
            std::string formatted = FormatWithVector(ctx.args[0].ToString(),
                                                     std::span(ctx.args.begin() + 1, ctx.args.end()));
            ctx.out << formatted;
        }
    }
    
    static Any Print(HookContext&& ctx) {
        _BasePrint(ctx);
        return {};
    }
    
    static Any PrintLn(HookContext&& ctx) {
        _BasePrint(ctx);
        ctx.out << "\n";
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

    Any Hook::Call(VirtualMachine* vm, const std::span<Any>& args) const {
        if (argc != -1 && args.size() != argc) {
            throw VmError(std::format("Function {} expects {} args but {} were given.",
                                      name, argc, args.size()));
        }\
        return func(HookContext(*vm, args, vm->GetOutStream()));
    }

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

Any RunScript(const std::string_view& script, std::ostream& target)
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
    
    VirtualMachine vm("default", target);
    vm.Init();
    
    for (const Command& cmd: cmds) {
        vm = vm.Execute(cmd);
    }
        
    // first item of stack is the result
    // TODO: if multiple items left on stack return it as array
    Any result {};
    if (!vm.IsStackEmpty()) {
        vm = vm.PopStack(&result);   
    }
    return result;
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

};