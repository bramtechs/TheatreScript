/*
    fn takeSum(int a, int b) int {
        return a + b;
    }
    
    Program
    - FunctionDeclaration (takeSum)
        - Parameter list
            - Param
                - a
                - int
            - Param b
                - b
                - int
        - Return type int
        - Body
            - Return
                - Expression
                    - identifier a
                    - operator +
                    - identifier b
*/
#pragma once

#include <unordered_map>
#include <stdexcept>
#include <format>
#include <magic_enum.hpp>

#include "types.hh"
#include "lexer.hh"

namespace theatre
{
    using ParseError = std::runtime_error;

    class TypedAny
    {
    public:
        template <AnyType type>
        constexpr TypedAny(const Any& any) : any(any)
        {
            if (!any.IsType<type>()) {
                // TODO: print col and row
                throw ParseError(std::format("Expected type {} but got {}", magic_enum::enum_name(type), any.GetTypeName()));
            }
        }
    private:
        const Any any;
        const AnyType type;
    };

    struct Node;
    struct Node {
        Node* parent;
        std::vector<Node> children;
    };

    struct Function
    {
        std::unordered_map<std::string, TypedAny> params;
    };

    Node ParseTokens(const std::vector<Token>& tokens);
}