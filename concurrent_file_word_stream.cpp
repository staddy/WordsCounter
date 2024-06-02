#include "concurrent_file_word_stream.hpp"

#include <cctype>
#include <stdexcept>

ConcurrentFileWordStream::ConcurrentFileWordStream(const std::string &filename,
                                                   std::size_t start,
                                                   std::size_t end)
    : m_fileReader(filename), m_currentPosition(start), m_end(end) {
  if (!m_fileReader.isOpen())
    throw std::runtime_error("Unable to open the file: " + filename);
  if (m_end == 0)
    m_end = m_fileReader.size();
  advance();
}

std::string ConcurrentFileWordStream::next() {
  std::string temp = std::move(m_currentWord);
  advance();
  return temp;
}

bool ConcurrentFileWordStream::hasNext() const {
  return !m_currentWord.empty();
}

bool ConcurrentFileWordStream::readChunk() {
  if (m_currentPosition >= m_end)
    return false;
  std::size_t chunkEnd = std::min(m_currentPosition + ChunkSize, m_end);
  m_chunk = m_fileReader.readChunk(m_currentPosition, chunkEnd);
  m_currentPosition = chunkEnd;
  m_chunkPosition = 0;
  return true;
}

void ConcurrentFileWordStream::advance() {
  m_currentWord.clear();
  while (m_chunkPosition < m_chunk.size() || readChunk()) {
    auto symbol = m_chunk[m_chunkPosition++];
    if (('A' <= symbol && symbol <= 'Z') || ('a' <= symbol && symbol <= 'z')) {
      if (symbol < 'a')
        symbol += ('a' - 'A');
      m_currentWord += symbol;
    } else if (!m_currentWord.empty())
      break;
  }
}
