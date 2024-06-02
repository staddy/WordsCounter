#include "concurrent_file_reader.hpp"

ConcurrentFileReader::ConcurrentFileReader()
    : m_fileSize(0), m_isOpen(false), m_fileData(nullptr) {}

ConcurrentFileReader::ConcurrentFileReader(const std::string &filename)
    : ConcurrentFileReader() {
  open(filename);
}

ConcurrentFileReader::~ConcurrentFileReader() { close(); }

void ConcurrentFileReader::open(const std::string &filename) {
  if (m_isOpen) {
    close();
  }

  init(filename);
}

bool ConcurrentFileReader::isOpen() const { return m_isOpen; }

void ConcurrentFileReader::close() {
  if (!m_isOpen) {
    return;
  }

#ifdef _WIN32
  if (m_fileData) {
    UnmapViewOfFile(m_fileData);
  }
  if (m_hMapFile) {
    CloseHandle(m_hMapFile);
  }
  if (m_hFile) {
    CloseHandle(m_hFile);
  }
#else
  if (m_fileData && m_fileData != MAP_FAILED) {
    munmap(const_cast<char *>(m_fileData), m_fileSize);
  }
  if (m_fd != -1) {
    ::close(m_fd);
  }
#endif

  m_isOpen = false;
  m_fileData = nullptr;
  m_fileSize = 0;
}

std::string ConcurrentFileReader::readChunk(std::size_t start,
                                            std::size_t end) {
  if (!m_isOpen) {
    throw std::runtime_error("File not open.");
  }
  if (start >= m_fileSize) {
    throw std::out_of_range("Start position is out of file bounds.");
  }
  if (end > m_fileSize) {
    end = m_fileSize;
  }

  return std::string(m_fileData + start, m_fileData + end);
}

std::size_t ConcurrentFileReader::size() const { return m_fileSize; }

#ifdef _WIN32
void ConcurrentFileReader::init(const std::string &filename) {
  m_hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ,
                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (m_hFile == INVALID_HANDLE_VALUE) {
    throw std::runtime_error("Unable to open the file: " + filename);
  }

  LARGE_INTEGER size;
  if (!GetFileSizeEx(m_hFile, &size)) {
    CloseHandle(m_hFile);
    throw std::runtime_error("Error getting the file size.");
  }
  m_fileSize = size.QuadPart;

  m_hMapFile =
      CreateFileMapping(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
  if (!m_hMapFile) {
    CloseHandle(m_hFile);
    throw std::runtime_error("Error creating file mapping.");
  }

  m_fileData = static_cast<const char *>(
      MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, m_fileSize));
  if (!m_fileData) {
    CloseHandle(m_hMapFile);
    CloseHandle(m_hFile);
    throw std::runtime_error("Error mapping the file into memory.");
  }

  m_isOpen = true;
}
#else
void ConcurrentFileReader::init(const std::string &filename) {
  m_fd = ::open(filename.c_str(), O_RDONLY);
  if (m_fd == -1) {
    throw std::runtime_error("Unable to open the file: " + filename);
  }

  m_fileSize = lseek(m_fd, 0, SEEK_END);
  if (m_fileSize == -1) {
    ::close(m_fd);
    throw std::runtime_error("Error getting the file size.");
  }

  m_fileData = static_cast<const char *>(
      mmap(nullptr, m_fileSize, PROT_READ, MAP_PRIVATE, m_fd, 0));
  if (m_fileData == MAP_FAILED) {
    ::close(m_fd);
    throw std::runtime_error("Error mapping the file into memory.");
  }

  m_isOpen = true;
}
#endif
