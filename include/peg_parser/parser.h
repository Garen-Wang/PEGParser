#pragma once

#include <stdexcept>
#include <utility>

#include "grammar.h"


namespace peg_parser {
  // newly-defined class for parsing non-consecutive strings without copying strings
  /*
  class StringViews {
  private:
    std::vector<std::string_view> strings;
    std::vector<std::size_t> prefixSums;
  public:
    explicit StringViews();
    explicit StringViews(std::vector<std::string>& strings);
    [[nodiscard]] std::size_t size() const;
    std::size_t getStringSize() const;
    std::size_t getPrefixSum(long idx) const;
    std::string_view getString(std::size_t idx) const;
    void append(std::string_view sv);
    void append(std::vector<std::string>::iterator it);
    std::vector<std::string_view>::iterator begin();
    std::vector<std::string_view>::iterator end();
    long upper_bound(std::size_t position) const;
  };
  */
  class Input {
  public:
    virtual ~Input() = default;
    virtual char current() const = 0;
    virtual void advance(int amount = 1) = 0;
    virtual int getPosition() const = 0;
    virtual void setPosition(int pos) = 0;
    virtual int length() const = 0;
    virtual bool isAtEnd() const = 0;
    virtual std::string slice(int begin, int end) = 0;
    virtual Input* copy() = 0;
  };


  struct SyntaxTree {
    std::shared_ptr<grammar::Rule> rule;
    std::string_view fullString;
    Input* customizedInput;
    bool customized;
    std::vector<std::shared_ptr<SyntaxTree>> inner;
    size_t begin, end;

    bool valid = false;
    bool active = true;
    bool recursive = false;

    SyntaxTree(const std::shared_ptr<grammar::Rule> &r, std::string_view s, size_t p);
    SyntaxTree(const std::shared_ptr<grammar::Rule> &r, Input* i, size_t p);

    [[nodiscard]] size_t length() const { return end - begin; }
    // deprecated apis
    std::string_view view() const {
      if (customized) return std::string_view(); // cannot view when it is not single string
      return fullString.substr(begin, length());
    }
    std::string string() {
      if (!customized) return std::string(view());
      else {
        return customizedInput->slice(begin, end);
//        std::size_t now = 0;
//        std::string res;
//        for (auto & sv : svs) {
//          if (now <= begin && begin < now + sv.size()) {
//            // exactly once
//            if (now <= end && end <= now + sv.size()) res += sv.substr(begin - now, length());
//            else res += sv.substr(begin - now);
//          } else if (now <= end && end <= now + sv.size()) {
//            res += sv.substr(0, end - now);
//          } else if (begin <= now && now + sv.size() <= end) {
//            res += sv;
//          }
//          now += sv.size();
//        }
//        return res;
      }
    }
  };

  struct Parser {
    struct Result {
      std::shared_ptr<SyntaxTree> syntax;
      std::shared_ptr<SyntaxTree> error;
    };

    struct GrammarError : std::exception {
      enum Type { UNKNOWN_SYMBOL, INVALID_RULE } type;
      grammar::Node::Shared node;
      mutable std::string buffer;
      GrammarError(Type t, grammar::Node::Shared n) : type(t), node(n) {}
      const char *what() const noexcept override;
    };

    std::shared_ptr<grammar::Rule> grammar;

    Parser(const std::shared_ptr<grammar::Rule> &grammar
           = std::make_shared<grammar::Rule>("undefined", grammar::Node::Error()));

    static Result parseAndGetError(const std::string_view &str,
                                   std::shared_ptr<grammar::Rule> grammar);
    static Result parseAndGetError(Input* input,
                                   std::shared_ptr<grammar::Rule> grammar);
    static std::shared_ptr<SyntaxTree> parse(const std::string_view &str,
                                             std::shared_ptr<grammar::Rule> grammar);

    std::shared_ptr<SyntaxTree> parse(const std::string_view &str) const;
    Result parseAndGetError(const std::string_view &str) const;
    Result parseAndGetError(Input* input) const;
  };

  std::ostream &operator<<(std::ostream &stream, const SyntaxTree &tree);

}  // namespace peg_parser
