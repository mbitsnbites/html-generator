//----------------------------------------------------------------------------
// A simple object oriented HTML document builder for C++.
//
// Created by Marcus Geelnard, 2014-09-28
//----------------------------------------------------------------------------
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//----------------------------------------------------------------------------
// Example usage:
//
//   Document doc;
//   Document::Element* body = doc.root().AddChild("body");
//   Document::Element* a = body->AddChild("a");
//   a->AddAttribute("href", "http://unlicense.org/");
//   a->AddTextChild("Click on me!");
//   Document::Element* p = body->AddChild("p");
//   p->AddTextChild("Hello world!");
//   std::cout << doc.GetHTML();
//----------------------------------------------------------------------------

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <string>
#include <memory>
#include <vector>

class Document {
  public:
    class Attribute {
      public:
        Attribute(const std::string& name, const std::string& value) :
            name_(name), value_(value) {}

        std::string GetHTML(void) {
          // TODO: Escape the value_ string.
          return name_ + "=\"" + value_ + "\"";
        }

      private:
        const std::string name_;
        const std::string value_;
    };

    class Node {
      public:
        virtual ~Node() {}
        virtual std::string GetHTML(void) const = 0;
    };

    class TextNode : public Node {
      public:
        TextNode(const std::string& value) : value_(value) {}

        virtual std::string GetHTML(void) const {
          return value_;
        }

      private:
        const std::string value_;
    };

    class Element : public Node {
      public:
        Element(const std::string& name) : name_(name) {}

        virtual std::string GetHTML(void) const {
          std::string result = "<" + name_;
          for (auto i = attributes_.begin(); i != attributes_.end(); ++i)
            result += " " + i->get()->GetHTML();
          if (children_.size() > 0) {
            result += ">";
            for (auto i = children_.begin(); i != children_.end(); ++i)
              result += i->get()->GetHTML();
            result += "</" + name_ + ">";
          } else
            result += " />";
          return result;
        }

        void AddAttribute(const std::string& name, const std::string& value) {
          std::unique_ptr<Attribute> attribute(new Attribute(name, value));
          attributes_.push_back(std::move(attribute));
        }

        Element* AddChild(const std::string& name) {
          std::unique_ptr<Node> node(new Element(name));
          children_.push_back(std::move(node));
          return reinterpret_cast<Element*>(children_[children_.size() - 1].get());
        }

        void AddTextChild(const std::string& value) {
          std::unique_ptr<Node> node(new TextNode(value));
          children_.push_back(std::move(node));
        }

      private:
        const std::string name_;
        std::vector<std::unique_ptr<Attribute> > attributes_;
        std::vector<std::unique_ptr<Node> > children_;

        friend class Document;
    };

    Document() : root_(Element("html")) {}

    Element& root() {
      return root_;
    }

    std::string GetHTML(void) const {
      return "<!DOCTYPE html>\n" + root_.GetHTML() + "\n";
    }

  private:
    Element root_;
};

#endif // DOCUMENT_H_
