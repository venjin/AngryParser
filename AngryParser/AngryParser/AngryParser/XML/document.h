#ifndef _DOCUMENT_HPP
#define _DOCUMENT_HPP

#include <cassert>
#include <cstring>

#include <new>
#include <iostream>

#include "../Core/compilerdetection.h"

#include "../Core/string.h"
#include "../Core/exception.h"

#include "Handler.h"
#include "Parser.h"

NS_BEGINE
inline namespace XML
{

class XMLNode;
class XMLElement;
class XMLText;
class XMLCDATA;
class XMLComment;
class XMLProcessingInstruction;
class XMLDocument;

namespace Impl
{

template <typename T>
class AngryParser_API List
{

public:
    struct AngryParser_API ListElement
    {

        T *prev;
        T *next;
        XMLNode *parent;

        ListElement() : prev(), next(), parent() {}
    };

    class Iterator
    {

    private:
        List *list;
        ListElement *p;

    public:
        Iterator(List *list_, ListElement *p_) : list(list_), p(p_) {}
        Iterator(const Iterator &src) : list(src.list), p(src.p) {}

        Iterator &operator=(const Iterator &src)
        {
            list = src.list, p = src.p;
            return *this;
        }

        T &operator*() const { return static_cast<T &>(*p); }
        T *operator->() const { return static_cast<T *>(p); }
        bool operator==(const Iterator &it) { return p == it.p; }
        bool operator==(XMLNode *q) { return p == q; }
        bool operator!=(const Iterator &it) { return p != it.p; }
        Iterator &operator++()
        {
            p = p->next;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }
        Iterator &operator--()
        {
            p = p ? p->prev : list->last;
            return *this;
        }
        Iterator operator--(int)
        {
            Iterator tmp = *this;
            --*this;
            return tmp;
        }
    };

private:
    T *first;
    T *last;

public:
    List() : first(), last() {}
    List(const List &src) = delete;
    ~List() = default;

    T &append(XMLNode &parent, T &child)
    {

        assert(!child.parent);
        if (first)
        {
            child.prev = last;
            last->next = &child;
            last = &child;
        }
        else
            first = last = &child;
        child.parent = &parent;
        return child;
    }

    T &insertBefore(T &child, T &ref)
    {

        assert(!child.parent && ref.parent);
        auto pPrev = ref.prev;
        child.prev = pPrev;
        child.next = &ref;
        if (pPrev)
            pPrev->next = &child;
        ref.prev = &child;
        child.parent = ref.parent;
        return child;
    }

    T &remove(T &child)
    {

        auto pPrev = child.prev;
        auto pNext = child.next;
        if (pPrev)
            pPrev->next = pNext;
        else
            first = pNext;
        if (pNext)
            pNext->next = pPrev;
        else
            last = pPrev;
        child.prev = nullptr;
        child.next = nullptr;
        child.parent = nullptr;
        return child;
    }

    T &getFirst() { return *first; }
    T &getLast() { return *last; }

    bool empty() const { return !first; }

    Iterator begin() { return Iterator(this, first); }
    Iterator end() { return Iterator(this, nullptr); }
};

} // namespace Impl

class XMLDOMException : public Exception
{

public:
    XMLDOMException(const StringView&data) : Exception("XMLDOMException: " + data) {}
};

enum class XMLNodeType : uint16_t
{

    Element,
    Text,
    CDATA,
    Comment,
    ProcessingInstruction,
    Document,

};

class AngryParser_API XMLNode : public Impl::List<XMLNode>::ListElement
{

private:
    const XMLNodeType type;
    Impl::List<XMLNode> listChild;

public:
    XMLNode(XMLNodeType type_) : Impl::List<XMLNode>::ListElement(), type(type_), listChild() {}
    XMLNode(const XMLNode &src) = delete;

    XMLNodeType getType() const { return type; }

    Impl::List<XMLNode> &child() { return listChild; }

    XMLNode &getFirstChild() { return listChild.getFirst(); }
    XMLNode &getLastChild() { return listChild.getLast(); }

    XMLNode &appendChild(XMLNode &child) { return listChild.append(*this, child); }
    XMLNode &insertBefore(XMLNode &child, XMLNode &ref) { return listChild.insertBefore(child, ref); }
    XMLNode &removeChild(XMLNode &child) { return listChild.remove(child); }
    bool hasChildNodes() { return !listChild.empty(); }

    XMLElement &asElement() noexcept { return reinterpret_cast<XMLElement &>(*this); }
    const XMLElement &asElement() const noexcept { return reinterpret_cast<const XMLElement &>(*this); }
    XMLText &asText() noexcept { return reinterpret_cast<XMLText &>(*this); }
    const XMLText &asText() const noexcept { return reinterpret_cast<const XMLText &>(*this); }
    XMLCDATA &asCDATA() noexcept { return reinterpret_cast<XMLCDATA &>(*this); }
    const XMLCDATA &asCDATA() const noexcept { return reinterpret_cast<const XMLCDATA &>(*this); }
    XMLComment &asComment() noexcept { return reinterpret_cast<XMLComment &>(*this); }
    const XMLComment &asComment() const noexcept { return reinterpret_cast<const XMLComment &>(*this); }
    XMLProcessingInstruction &asProcessingInstruction() noexcept { return reinterpret_cast<XMLProcessingInstruction &>(*this); }
    const XMLProcessingInstruction &asProcessingInstruction() const noexcept { return reinterpret_cast<const XMLProcessingInstruction &>(*this); }
    XMLDocument &asDocument() noexcept { return reinterpret_cast<XMLDocument &>(*this); }
    const XMLDocument &asDocument() const noexcept { return reinterpret_cast<const XMLDocument &>(*this); }
};

class AngryParser_API XMLAttribute : public Impl::List<XMLAttribute>::ListElement
{
private:
    StringView name;
    StringView value;

public:
    XMLAttribute() : Impl::List<XMLAttribute>::ListElement(), name(), value() {}
    XMLAttribute(StringView name_, StringView value_) : Impl::List<XMLAttribute>::ListElement(), name(name_), value(value_) {}
    XMLAttribute(const XMLAttribute &src) = delete;

    StringView getName() const { return name; }
    void setName(StringView name_) { name = name_; }
    StringView getValue() const { return value; }
    void setValue(StringView value_) { value = value_; }
};

class AngryParser_API XMLElement : public XMLNode
{
private:
    Impl::List<XMLAttribute> listAttr;
    StringView name;

public:
    XMLElement() : XMLNode(XMLNodeType::Element), listAttr(), name() {}
    XMLElement(StringView name_) : XMLNode(XMLNodeType::Element), listAttr(), name(name_) {}
    XMLElement(const XMLElement &src) = delete;

    Impl::List<XMLAttribute> &attribute() { return listAttr; }

    StringView getName() const { return name; }
    void setName(StringView name_) { name = name_; }

    XMLAttribute &getFirstAttribute() { return listAttr.getFirst(); }
    XMLAttribute &getLastAttribute() { return listAttr.getLast(); }
    XMLAttribute &appendAttribute(XMLAttribute &attr) { return listAttr.append(*this, attr); }
    XMLAttribute &removeAttribute(XMLAttribute &attr) { return listAttr.remove(attr); }
};

class AngryParser_API XMLText : public XMLNode
{
private:
    StringView value;

public:
    XMLText() : XMLNode(XMLNodeType::Text), value() {}
    XMLText(StringView value_) : XMLNode(XMLNodeType::Text), value(value_) {}
    XMLText(const XMLText &src) = delete;

    StringView getValue() const { return value; }
    void setValue(StringView value_) { value = value_; }
};

class AngryParser_API XMLCDATA : public XMLNode
{
private:
    StringView value;

public:
    XMLCDATA() : XMLNode(XMLNodeType::CDATA), value() {}
    XMLCDATA(StringView value_) : XMLNode(XMLNodeType::CDATA), value(value_) {}
    XMLCDATA(const XMLCDATA &src) = delete;

    StringView getValue() const { return value; }
    void setValue(StringView value_) { value = value_; }
};

class AngryParser_API XMLComment : public XMLNode
{
private:
    StringView value;

public:
    XMLComment() : XMLNode(XMLNodeType::Comment), value() {}
    XMLComment(StringView value_) : XMLNode(XMLNodeType::Comment), value(value_) {}
    XMLComment(const XMLComment &src) = delete;

    StringView getValue() const { return value; }
    void setValue(StringView value_) { value = value_; }
};

class AngryParser_API XMLProcessingInstruction : public XMLNode
{
private:
    StringView name;
    StringView value;

public:
    XMLProcessingInstruction() : XMLNode(XMLNodeType::ProcessingInstruction), name(), value(){};
    XMLProcessingInstruction(StringView &name_, StringView &value_) : XMLNode(XMLNodeType::ProcessingInstruction), name(name_), value(value_) {}
    XMLProcessingInstruction(const XMLProcessingInstruction &src) = delete;

    StringView getName() const { return name; }
    void setName(StringView name_) { name = name_; }
    StringView getValue() const { return value; }
    void setValue(StringView value_) { value = value_; }
};

class AngryParser_API XMLDocument : public XMLNode
{

public:
    XMLDocument() : XMLNode(XMLNodeType::Document) {}
    XMLDocument(const XMLDocument &src) = delete;

    XMLElement &createElement(StringView name)
    {

        return *new XMLElement(name);
    }
    XMLAttribute &createAttribute(StringView name, StringView value)
    {

        return *new XMLAttribute(name, value);
    }
    XMLText &createText(StringView value)
    {

        return *new XMLText(value);
    }
    XMLCDATA &createCDATA(StringView value)
    {

        return *new XMLCDATA(value);
    }
    XMLComment &createComment(StringView value)
    {

        return *new XMLComment(value);
    }
    XMLProcessingInstruction &createProcessingInstruction(StringView name, StringView value)
    {

        return *new XMLProcessingInstruction(name, value);
    }

    void clear()
    {
    }

    XMLElement &getRootElement()
    {

        for (auto &node : child())
            if (node.getType() == XMLNodeType::Element)
                return static_cast<XMLElement &>(node);
        throw XMLDOMException("Root element not found");
    }

    template <XMLParser::Flag F = XMLParser::Flag::Default>
    void parse(char *data)
    {

        class Handler : public XMLHandlerBase
        {

        private:
            XMLDocument *document;
            XMLNode *cur;

        public:
            Handler(XMLDocument *document_) : document(document_), cur(nullptr) {}

            void startDocument() { cur = document; }
            void startElement(StringView name)
            {

                auto &element = document->createElement(name);
                cur->appendChild(element);
                cur = &element;
            }
            void endElement(StringView /*name*/)
            {

                cur = cur->parent;
            }
            void endAttributes(bool empty)
            {

                if (empty)
                    cur = cur->parent;
            }
            void attribute(StringView name, StringView value)
            {

                static_cast<XMLElement *>(cur)->appendAttribute(document->createAttribute(name, value));
            }
            void text(StringView value)
            {

                cur->appendChild(document->createText(value));
            }
            void cdata(StringView value)
            {

                cur->appendChild(document->createCDATA(value));
            }
            void comment(StringView value)
            {

                cur->appendChild(document->createComment(value));
            }
            void processingInstruction(StringView name, StringView value)
            {

                cur->appendChild(document->createProcessingInstruction(name, value));
            }
        };

        assert(data);

        clear();
        XMLParser parser;
        Handler handler(this);
        parser.parse<F>(data, handler);
    }

    void print(std::ostream &stream)
    {
        if (hasChildNodes())
        {

            XMLNode *cur = &getFirstChild();
            while (true)
            {

                switch (cur->getType())
                {

                case XMLNodeType::Element:
                {

                    auto &element = static_cast<XMLElement &>(*cur);
                    auto name = element.getName();
                    stream.write("<", 1);
                    stream.write(name.getData(), name.getLength());
                    for (auto &attr : element.attribute())
                    {
                        auto attrName = attr.getName();
                        auto attrValue = attr.getValue();
                        stream.write(" ", 1);
                        stream.write(attrName.getData(), attrName.getLength());
                        stream.write("=\"", 2);
                        stream.write(attrValue.getData(), attrValue.getLength());
                        stream.write("\"", 1);
                    }
                    bool empty = !cur->hasChildNodes();
                    {
                        if (empty)
                            stream.write("/>", 2);
                        else
                            stream.write(">", 1);
                    }
                    if (!empty)
                    {
                        cur = &cur->getFirstChild();
                        continue;
                    }
                    break;
                }
                case XMLNodeType::Text:
                {

                    auto &text = static_cast<XMLText &>(*cur);
                    auto value = text.getValue();
                    stream.write(value.getData(), value.getLength());
                    break;
                }
                case XMLNodeType::CDATA:
                {

                    auto &cdata = static_cast<XMLCDATA &>(*cur);
                    auto value = cdata.getValue();
                    stream.write("<![CDATA[", 9);
                    stream.write(value.getData(), value.getLength());
                    stream.write("]]>", 3);
                    break;
                }
                case XMLNodeType::Comment:
                {

                    auto &comment = static_cast<XMLComment &>(*cur);
                    auto value = comment.getValue();
                    stream.write("<!--", 4);
                    stream.write(value.getData(), value.getLength());
                    stream.write("-->", 3);
                    break;
                }
                case XMLNodeType::ProcessingInstruction:
                {

                    auto &pi = static_cast<XMLProcessingInstruction &>(*cur);
                    auto name = pi.getName();
                    auto value = pi.getValue();

                    stream.write("<?", 2);
                    stream.write(name.getData(), name.getLength());
                    stream.write(" ", 1);
                    stream.write(value.getData(), value.getLength());
                    stream.write("?>", 2);
                    break;
                }
                default:
                    throw XMLDOMException("Invalid node type");
                }
                while (!cur->next)
                {

                    cur = cur->parent;
                    if (cur == this)
                        break;
                    auto name = static_cast<XMLElement *>(cur)->getName();

                    stream.write("</", 2);
                    stream.write(name.getData(), name.getLength());
                    stream.write(">", 1);
                }
                if (cur == this)
                    break;
                cur = cur->next;
            }
            stream.flush();
        }
    }
};

inline std::ostream &operator<<(std::ostream &stream, XMLDocument &document)
{
    document.print(stream);
    return stream;
}

} // namespace XML
NS_END

#endif
