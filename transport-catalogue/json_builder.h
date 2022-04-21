#pragma once

#include "json.h"

#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace json {

    class KeyItemContext;
    class KeyValueItemContext;
    class DictItemContext;
    class ArrayItemContext;

    class Builder final {
    public:
        Builder() = default;

        // выбрасывает исключение std::logic_error если на момент вызова объект некорректен
        const Node& Build() const;

        KeyItemContext Key(std::string key);
        Builder& Value(NodeType value);

        DictItemContext StartDict();
        Builder& EndDict();

        ArrayItemContext StartArray();
        Builder& EndArray();

    private:
        // В зависимости от тип value добавляет новый указатель в nodes_stack_
        void AddRef(const Node& value);
        bool IsEmpty() const;

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> key_;

    };

    // Вспомогательные классы

    class ItemContext {
    public:
        ItemContext(Builder& builder) : builder_{ builder } {}
    protected:
        Builder& builder_;

        KeyItemContext Key(std::string key);
        DictItemContext StartDict();
        Builder& EndDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
    };

    class KeyValueItemContext final : public ItemContext {
    public:
        KeyValueItemContext(Builder& builder);
        using ItemContext::Key;
        using ItemContext::EndDict;
    };

    class KeyItemContext final : public ItemContext {
    public:
        KeyItemContext(Builder& builder);
        KeyValueItemContext Value(NodeType value);
        using ItemContext::StartDict;
        using ItemContext::StartArray;
    };

    class DictItemContext final : public ItemContext {
    public:
        DictItemContext(Builder& builder);
        using ItemContext::Key;
        using ItemContext::EndDict;
    };

    class ArrayItemContext final : public ItemContext {
    public:
        ArrayItemContext(Builder& builder);
        ArrayItemContext Value(NodeType value);
        using ItemContext::StartDict;
        using ItemContext::StartArray;
        using ItemContext::EndArray;
    };

} // namespace json
