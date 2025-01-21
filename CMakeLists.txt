cmake_minimum_required(VERSION 3.5)
project(MyLibrary LANGUAGES CXX)

# Подпроект с библиотекой
add_subdirectory(library)

# Подпроект с program1
add_subdirectory(program1)

# Подпроект с program2
add_subdirectory(program2)