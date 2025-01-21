cmake_minimum_required(VERSION 3.5)

project(Program2 LANGUAGES CXX)

add_executable(program2 main2.cpp)

# Подключаем нашу библиотеку (для функции №3)
target_link_libraries(program2 PRIVATE mylib)

# Сокеты и потоки
target_link_libraries(program2 PRIVATE pthread)