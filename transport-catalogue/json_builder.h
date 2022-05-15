#pragma once

#include <string>
#include <variant>
#include <vector>

#include "json.h"

namespace json {

    class Builder final {

        class ItemContext;
        class KeyItemContext;
        class KeyValueItemContext;
        class DictItemContext;
        class ArrayItemContext;

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

        Node root_;
        std::vector<Node*> nodes_stack_;
        // По умоолчанию словарь пуст
        bool is_empty_ = true;
        // Наличие введенного ключа
        bool has_key_ = false;
        std::string key_;
        
    };

    // Вспомогательные классы

    class Builder::ItemContext {
    public:
        ItemContext(Builder& builder) : builder_{ builder } {}
    protected:
        KeyItemContext Key(std::string key);
        DictItemContext StartDict();
        Builder& EndDict();
        ArrayItemContext StartArray();
        Builder& EndArray();

        Builder& builder_;
    };

    class Builder::KeyValueItemContext final : public ItemContext {
    public:
        using ItemContext::ItemContext;
        using ItemContext::Key;
        using ItemContext::EndDict;

    };

    class Builder::KeyItemContext final : public ItemContext {
    public:
        using ItemContext::ItemContext;
        KeyValueItemContext Value(NodeType value);
        using ItemContext::StartDict;
        using ItemContext::StartArray;
    };

    class Builder::DictItemContext final : public ItemContext {
    public:
        using ItemContext::ItemContext;
        using ItemContext::Key;
        using ItemContext::EndDict;
    };

    class Builder::ArrayItemContext final : public ItemContext {
    public:
        using ItemContext::ItemContext;
        ArrayItemContext Value(NodeType value);
        using ItemContext::StartDict;
        using ItemContext::StartArray;
        using ItemContext::EndArray;
    };

} // namespace json
