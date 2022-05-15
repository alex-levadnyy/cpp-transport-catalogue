#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using NodeType = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node : public NodeType {
    public:

        using NodeType::variant;

        bool IsNull() const noexcept;
        bool IsInt() const noexcept;
        bool IsDouble() const noexcept;
        bool IsPureDouble() const noexcept;
        bool IsBool() const noexcept;
        bool IsString() const noexcept;
        bool IsArray() const noexcept;
        bool IsMap() const noexcept;

        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const NodeType& GetValue() const;

        friend bool operator==(const Node& lhs, const Node& rhs) {
            return static_cast<NodeType>(lhs) == static_cast<NodeType>(rhs);
        }
        friend bool operator!=(const Node& lhs, const Node& rhs) {
            return !(lhs == rhs);
        }

    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& rhs) {
            return (root_ == rhs.GetRoot());
        }
        bool operator!=(const Document& rhs) {
            return !(root_ == rhs.GetRoot());
        }
    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
    void PrintNode(const Node& node, std::ostream& output);

    struct RenderContext {
            RenderContext(std::ostream &out, int indent = 0) : out(out), indent(indent) {}
            void RenderIndent() const {
                for (int i = 0; i < indent; ++i) {
                    out.put(' ');
                }
            }
            std::ostream &out;
            int indent = 0;
    };
}  // namespace json