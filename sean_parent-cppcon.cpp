#include <iostream>
#include <vector>
#include <memory>
#include <cassert>


template <class T>
void draw(const T& x, std::ostream& out, size_t position)
{
    out << std::string(position, ' ') << x << std::endl;
}


class object_t {
public:
    template <class T>
    object_t(T x)
    : self_(std::make_shared<model<T>>(std::move(x)))
    {
        std::cout << "ctor" << std::endl;
    }

    friend void draw(const object_t& x, std::ostream& out, size_t position) {
        x.self_->draw_(out, position);
    }

private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void draw_(std::ostream&, size_t) const = 0;
    };

    template <class T>
    struct model : concept_t {
        model(T x)
        : data_(std::move(x))
        { }

        void draw_(std::ostream& out, size_t position) const override {
            draw(data_, out, position);
        }

        T data_;
    };

    std::shared_ptr<const concept_t> self_;
};

using document_t = std::vector<object_t>;

void draw(const document_t& x, std::ostream& out, size_t position)
{
    out << std::string(position, ' ') << "<document>" << std::endl;
    for (const auto& e : x)
        draw(e, out, position + 2);
    out << std::string(position, ' ') << "</document>" << std::endl;
}

using history_t = std::vector<document_t>;

void commit(history_t& x)
{
    assert(x.size());
    x.push_back(x.back());
}

void undo(history_t& x)
{
    assert(x.size());
    x.pop_back();
}

document_t& current(history_t& x)
{
    assert(x.size());
    return x.back();
}

class my_class_t {
};

void draw(const my_class_t& x, std::ostream& out, size_t position)
{
    out << std::string(position, ' ') << "my_class_t" << std::endl;
}

int main()
{
    history_t h(1);

    current(h).push_back(0);
    current(h).push_back(std::string("hello"));

    draw(current(h), std::cout, 0);
    std::cout << "------------------------------------" << std::endl;

    commit(h);

    current(h).push_back(current(h));
    current(h).push_back(my_class_t());
    current(h)[1] = std::string("world");

    draw(current(h), std::cout, 0);
    std::cout << "------------------------------------" << std::endl;

    undo(h);

    draw(current(h), std::cout, 0);
}

