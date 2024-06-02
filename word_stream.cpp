#include "word_stream.hpp"

WordStream::operator bool() const { return hasNext(); }

WordStream &operator>>(WordStream &stream, std::string &word) {
  if (stream.hasNext()) {
    word = stream.next();
  } else {
    word.clear();
  }
  return stream;
}
