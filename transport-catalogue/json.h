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

    class Node {
    public:
        Node() = default;
        Node(bool value)
            : node_(value) {
        }
        Node(nullptr_t)
            : node_(nullptr) {
        }
        Node(Array array)
            : node_(move(array)) {
        }
        Node(Dict map)
            : node_(move(map)) {
        }
        Node(int value)
            : node_(value) {
        }
        Node(double value)
            : node_(value) {
        }
        Node(std::string value)
            : node_(move(value)) {
        }

        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsMap() const;

        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const NodeType& GetValue() const;

        bool operator==(const Node& rhs) const {
            return (node_ == rhs.node_);
        }
        bool operator!=(const Node& rhs) const {
            return !(node_ == rhs.node_);
        }

    private:
        NodeType node_;
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
}  // namespace json