TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        concurrent_file_reader.cpp \
        concurrent_file_word_stream.cpp \
        main.cpp \
        timer.cpp \
        word_counter.cpp \
        word_stream.cpp

HEADERS += \
    concurrent_file_reader.hpp \
    concurrent_file_word_stream.hpp \
    timer.hpp \
    word_counter.hpp \
    word_stream.hpp
