#include <fstream>
#include <iostream>

#include "timer.hpp"
#include "word_counter.hpp"

void writeWordCountsToFile(
    const std::vector<std::pair<std::string, int>> &sortedCounts,
    const std::string &outputFileName) {
  std::ofstream outFile(outputFileName);
  if (!outFile) {
    std::cerr << "Error opening output file: " << outputFileName << std::endl;
    return;
  }
  for (const auto &wordCount : sortedCounts) {
    outFile << wordCount.second << " " << wordCount.first << std::endl;
  }
  outFile.close();
}

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <output_file> [<thread_count>]" << std::endl;
    return 1;
  }
  size_t threadCount = std::thread::hardware_concurrency();
  if (argc >= 4) {
    try {
      threadCount = std::stoul(argv[3]);
    } catch (const std::exception &e) {
      std::cerr << "Error: thread_count must be an unsigned integer."
                << std::endl;
      return 1;
    }
  }

  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];
  std::vector<std::pair<std::string, int>> result;
  try {
    std::cout << "Thread count: " << threadCount << std::endl;
    Timer timer("Counting words");
    WordCounter wordCounter(threadCount);
    result = wordCounter.countWordsInFile(inputFileName);
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }
  writeWordCountsToFile(result, outputFileName);
  return 0;
}
