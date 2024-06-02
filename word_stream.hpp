#ifndef WORD_STREAM_HPP
#define WORD_STREAM_HPP

#include <string>

class WordStream {
public:
  virtual ~WordStream() = default;

  virtual std::string next() = 0;
  virtual bool hasNext() const = 0;

  explicit operator bool() const;
};

WordStream &operator>>(WordStream &stream, std::string &word);

#endif // WORD_STREAM_HPP
