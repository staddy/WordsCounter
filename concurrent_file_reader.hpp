#ifndef CONCURRENT_FILE_READER_HPP
#define CONCURRENT_FILE_READER_HPP

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

class ConcurrentFileReader {
public:
  ConcurrentFileReader();
  explicit ConcurrentFileReader(const std::string &filename);
  ~ConcurrentFileReader();

  void open(const std::string &filename);
  bool isOpen() const;
  void close();
  std::string readChunk(std::size_t start, std::size_t end);
  std::size_t size() const;
  const char *fileData;

private:
  void init(const std::string &filename);

  std::size_t m_fileSize;
  bool m_isOpen;

#ifdef _WIN32
  HANDLE m_hFile;
  HANDLE m_hMapFile;
#else
  int m_fd;
#endif
};

#endif // CONCURRENT_FILE_READER_HPP
