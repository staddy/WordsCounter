#include "word_counter.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <future>

#include "concurrent_file_word_stream.hpp"

WordCounter::WordCounter(size_t threadCount_, size_t fastSearchLength_)
    : threadCount(threadCount_), fastSearchLength(fastSearchLength_) {}

std::unordered_map<std::string, int>
WordCounter::countWords(WordStream &wordStream) const {
  std::unordered_map<std::string, int> wordCounts;
  size_t dimension = std::pow(27, fastSearchLength);
  std::vector<int> wordVector(dimension, 0);
  std::string word;

  while (wordStream >> word, !word.empty()) {
    if (word.size() <= fastSearchLength) {
      size_t index = 0;
      size_t mult = 1;
      for (char c : word) {
        index += (c - 'a' + 1) * mult;
        mult *= 27;
      }
      ++wordVector[index];
      continue;
    }
    ++wordCounts[word];
  }

  for (size_t i = 0; i < wordVector.size(); ++i) {
    if (wordVector[i] == 0)
      continue;
    std::string word;
    size_t value = i;
    for (size_t j = 0; j < fastSearchLength; ++j) {
      char c = value % 27;
      if (c > 0)
        word += ('a' + c - 1);
      value /= 27;
    }
    wordCounts[word] += wordVector[i];
  }

  return wordCounts;
}

void WordCounter::mergeWordCounts(
    std::unordered_map<std::string, int> &globalCounts,
    const std::unordered_map<std::string, int> &localCounts) {
  std::unique_lock<std::mutex> lock(m_mutex);
  for (const auto &entry : localCounts) {
    globalCounts[entry.first] += entry.second;
  }
}

void WordCounter::adjustChunkBoundaries(std::ifstream &file,
                                        std::streampos &end) {
  file.seekg(end);
  char ch;
  while (file.get(ch)) {
    if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')) {
      do {
        end = file.tellg();
      } while (file.get(ch) &&
               (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')));
      break;
    }
  }
}

std::vector<std::pair<std::string, int>>
WordCounter::countWordsInFile(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open the file: " + filePath);
  }

  std::streampos fileSize = file.tellg();
  size_t chunkSize = fileSize / threadCount + 1;

  std::unordered_map<std::string, int> globalCounts;
  std::vector<std::future<void>> futures;

  std::streampos previous = 0;
  for (size_t i = 0; i < threadCount; ++i) {
    std::streampos start = previous;
    std::streampos end = (i + 1) * chunkSize;
    if (start >= end)
      continue;
    if (i == threadCount - 1) {
      end = fileSize;
    } else {
      adjustChunkBoundaries(file, end);
    }
    previous = end;

    futures.push_back(std::async(std::launch::async, [=, &globalCounts] {
      ConcurrentFileWordStream wordStream(filePath, start, end);
      auto localCounts = countWords(wordStream);
      mergeWordCounts(globalCounts, localCounts);
    }));
  }

  for (auto &future : futures) {
    future.get();
  }

  std::vector<std::pair<std::string, int>> sortedCounts(globalCounts.begin(),
                                                        globalCounts.end());
  {
    std::sort(sortedCounts.begin(), sortedCounts.end(),
              [](const auto &a, const auto &b) {
                return a.second > b.second ||
                       (a.second == b.second && a.first < b.first);
              });
  }

  return sortedCounts;
}
