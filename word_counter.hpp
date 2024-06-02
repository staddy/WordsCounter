#ifndef WORD_COUNTER_H
#define WORD_COUNTER_H

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "word_stream.hpp"

class WordCounter {
public:
  WordCounter(size_t threadCount = std::thread::hardware_concurrency(),
              size_t fastSearchLength = 5);

  std::vector<std::pair<std::string, int>>
  countWordsInFile(const std::string &filePath);
  size_t threadCount;
  size_t fastSearchLength;

private:
  std::unordered_map<std::string, int> countWords(WordStream &wordStream) const;
  void mergeWordCounts(std::unordered_map<std::string, int> &globalCounts,
                       const std::unordered_map<std::string, int> &localCounts);
  static void adjustChunkBoundaries(std::ifstream &file, std::streampos &end);
  std::mutex m_mutex;
};

#endif // WORD_COUNTER_H
