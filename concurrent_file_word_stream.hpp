#ifndef CONCURRENT_FILE_WORD_STREAM_HPP
#define CONCURRENT_FILE_WORD_STREAM_HPP

#include "concurrent_file_reader.hpp"
#include "word_stream.hpp"

#include <sstream>

class ConcurrentFileWordStream : public WordStream {
public:
  ConcurrentFileWordStream(const std::string &filename, std::size_t start = 0,
                           std::size_t end = 0);

  std::string next() override;
  bool hasNext() const override;

private:
  void advance();

  ConcurrentFileReader m_fileReader;
  std::size_t m_currentPosition;
  std::size_t m_end;
  std::string m_currentWord;
};

#endif // CONCURRENT_FILE_WORD_STREAM_HPP
