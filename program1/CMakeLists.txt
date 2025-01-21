cmake_minimum_required(VERSION 3.5)

project(Program1 LANGUAGES CXX)

add_executable(program1 main1.cpp)

# Подключаем библиотеку mylib (из проекта library)
target_link_libraries(program1 PRIVATE mylib)

# Для Unix: нужно линковаться с pthread и т.д.
target_link_libraries(program1 PRIVATE pthread)

# Для использования socket-библиотек на Linux
target_link_libraries(program1 PRIVATE -lpthread)