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

/// @mainpage
/// A simple object oriented HTML document builder for C++.
///
/// @section example Example usage
/// @code{.cpp}
///   Document doc;
///
///   // Generate a document structure.
///   Document::Element* body = doc.root()->AddChild("body");
///   Document::Element* a = body->AddChild("a");
///   a->AddAttribute("href", "http://unlicense.org/");
///   a->AddTextChild("Click on me!");
///   Document::Element* p = body->AddChild("p");
///   p->AddTextChild("Hello world!");
///
///   // Convert the document to an HTML formatted string.
///   std::string html_string;
///   doc.GetHTML(html_string);
///   std::cout << html_string;
/// @endcode

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <string>
#include <memory>
#include <vector>

/// @brief A container for a single HTML document.
///
/// The Document contains a root node, which is an Element with the name
/// "html". Children can be added to the root node to form a node tree.
class Document {
  public:
    /// @brief An interface used for all HTML nodes.
    class Node {
      public:
        virtual ~Node() {}

        /// @brief Get an HTML formatted string representing this node.
        /// @param[out] out The output string that will receive the HTML.
        virtual void GetHTML(std::string& out) const = 0;
    };

    /// @brief An attribute that can be part of an Element.
    class Attribute : public Node {
      public:
        Attribute(const std::string& name, const std::string& value) :
            name_(name), value_(value) {}

        virtual void GetHTML(std::string& out) const {
          out.append(name_);
          out.append("=\"");
          size_t quote_pos = value_.find('"');
          if (quote_pos != std::string::npos) {
            // We need to escape the string (this should be an uncommon case).
            std::string escaped(value_);
            do {
              escaped.replace(quote_pos, 1, "&quot;", 6);
              quote_pos = escaped.find('"');
            } while (quote_pos != std::string::npos);
            out.append(escaped);
          }
          else
            out.append(value_);
          out.append("\"");
        }

      private:
        const std::string name_;
        const std::string value_;
    };

    /// @brief A text node (typically named "#text" in a DOM).
    class TextNode : public Node {
      public:
        TextNode(const std::string& value) : value_(value) {}

        virtual void GetHTML(std::string& out) const {
          out.append(value_);
        }

      private:
        const std::string value_;
    };

    /// @brief An Element can have attributes and children.
    class Element : public Node {
      public:
        Element(const std::string& name) : name_(name) {}

        virtual void GetHTML(std::string& out) const {
          out.append("<");
          out.append(name_);
          for (auto i = attributes_.begin(); i != attributes_.end(); ++i) {
            out.append(" ");
            i->get()->GetHTML(out);
          }
          if (children_.size() > 0) {
            out.append(">");
            for (auto i = children_.begin(); i != children_.end(); ++i)
              i->get()->GetHTML(out);
            out.append("</");
            out.append(name_);
            out.append(">");
          } else
            out.append(" />");
        }

        /// @brief Add an attribute to this Element.
        /// @param name The attribute name.
        /// @param value The attribute value.
        void AddAttribute(const std::string& name, const std::string& value) {
          std::unique_ptr<Attribute> attribute(new Attribute(name, value));
          attributes_.push_back(std::move(attribute));
        }

        /// @brief Add a child to this Element.
        /// @param name The name of the new child element.
        Element* AddChild(const std::string& name) {
          std::unique_ptr<Node> node(new Element(name));
          children_.push_back(std::move(node));
          return reinterpret_cast<Element*>(children_[children_.size() - 1].get());
        }

        /// @brief Add a text node child to this element.
        /// @param value The text for the new text node.
        void AddTextChild(const std::string& value) {
          std::unique_ptr<Node> node(new TextNode(value));
          children_.push_back(std::move(node));
        }

      private:
        const std::string name_;
        std::vector<std::unique_ptr<Node> > attributes_;
        std::vector<std::unique_ptr<Node> > children_;
    };

    Document() : root_(Element("html")) {}

    /// @brief Get the root element of this document.
    Element* root() {
      return &root_;
    }

    /// @brief Get an HTML formatted string representing this document.
    /// @param[out] out The output string that will receive the HTML.
    void GetHTML(std::string& out) const {
      out.append("<!DOCTYPE html>\n");
      root_.GetHTML(out);
      out.append("\n");
    }

  private:
    Element root_;
};

#endif // DOCUMENT_H_
