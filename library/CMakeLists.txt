cmake_minimum_required(VERSION 3.5)

project(MyLibrary LANGUAGES CXX)

# Создаём динамическую (shared) библиотеку
add_library(mylib SHARED library.cpp)

# Устанавливаем макрос для экспорта функций (LIBRARY_EXPORT),
target_compile_definitions(mylib PRIVATE LIBRARY_EXPORT)

# Заголовочный файл
target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
