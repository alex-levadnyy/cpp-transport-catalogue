#include "json_builder.h"

namespace json {

    using namespace std;

    const Node& Builder::Build() const {
        // Если строитель пустой или есть незакрытые контейнеры
        if (IsEmpty() || !nodes_stack_.empty()) {
            throw std::logic_error("Builder state is invalid"s);
        }
        return root_;
    }

    KeyItemContext Builder::Key(std::string key) {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsMap() && !key_) {
            key_ = std::move(key);
            return KeyItemContext(*this);
        }

        throw std::logic_error("Incorrect place for key : "s + key);
    }

    Builder& Builder::Value(NodeType value) {

        Node new_node = std::visit([](auto val) {
            return Node(val);
            }, value);

        if (IsEmpty()) {
            root_ = new_node;
            return *this;
        }

        if (!nodes_stack_.empty() && nodes_stack_.back()->IsMap() && key_) {
            const_cast<Dict&>(nodes_stack_.back()->AsMap()).insert({ key_.value(), new_node});
            return *this;
        }

        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(new_node);
            return *this;
        }

        // в остальных случаях выбрасываем исключение
        throw std::logic_error("Incorrect place for value"s);
    }

    DictItemContext Builder::StartDict() {
        Value(Dict{});
        AddRef(Node(Dict{}));
        return DictItemContext(*this);
    }

    Builder& Builder::EndDict() {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsMap() && !key_) {
            nodes_stack_.pop_back();
            return *this;
        }
        throw std::logic_error("Incorrect place for EndDict"s);
    }

    ArrayItemContext Builder::StartArray() {
        Value(Array{});
        AddRef(Node(Array{}));
        return ArrayItemContext(*this);
    }

    Builder& Builder::EndArray() {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            nodes_stack_.pop_back();
            return *this;
        }
        throw std::logic_error("Incorrect place for EndArray"s);
    }

    // Добавляет указатель на новую структуру в стек nodes_stack_, если
    // вне структуры
    // в массиве
    // в словаре
    void Builder::AddRef(const Node& value) {
        
        if (value.IsArray() || value.IsMap()) {
            if (nodes_stack_.empty()) {
                nodes_stack_.push_back(&root_);
                return;
            }
            if (nodes_stack_.back()->IsArray()) {
                auto p = &nodes_stack_.back()->AsArray().back();
                nodes_stack_.push_back(const_cast<Node*>(p));
                return;
            }
            if (nodes_stack_.back()->IsMap()) {
                auto p = &nodes_stack_.back()->AsMap().at(key_.value());
                nodes_stack_.push_back(const_cast<Node*>(p));
                return;
            }
        }
    }

    bool Builder::IsEmpty() const {
        return nodes_stack_.back()->IsNull();
    }
    // Вспомогательные классы

    KeyItemContext::KeyItemContext(Builder& builder) : ItemContext(builder) {}

    KeyValueItemContext::KeyValueItemContext(Builder& builder) : ItemContext(builder) {}

    KeyValueItemContext KeyItemContext::Value(NodeType value) {
        builder_.Value(std::move(value));
        return KeyValueItemContext{ builder_ };
    }

    DictItemContext::DictItemContext(Builder& builder) : ItemContext(builder) {}

    ArrayItemContext::ArrayItemContext(Builder& builder) : ItemContext(builder) {}

    ArrayItemContext ArrayItemContext::Value(NodeType value) {
        builder_.Value(std::move(value));
        return ArrayItemContext{ builder_ };
    }

    KeyItemContext ItemContext::Key(std::string key) {
        return builder_.Key(std::move(key));
    }

    DictItemContext ItemContext::StartDict() {
        return builder_.StartDict();
    }

    Builder& ItemContext::EndDict() {
        return builder_.EndDict();
    }

    ArrayItemContext ItemContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& ItemContext::EndArray() {
        return builder_.EndArray();
    }

} // namespace json
