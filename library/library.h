#ifndef MYLIB_LIBRARY_H
#define MYLIB_LIBRARY_H

#ifdef _WIN32
#  ifdef LIBRARY_EXPORT
#    define LIBRARY_API __declspec(dllexport)
#  else
#    define LIBRARY_API __declspec(dllimport)
#  endif
#else
#  define LIBRARY_API
#endif

#include <string>

extern "C" {

// Функция №1 — сортирует строку и заменяет каждый чётный индекс на "Kv".
LIBRARY_API void sortAndReplaceEvenPositions(char* cstr);

// Функция №2 — возвращает сумму всех цифр в строке.
LIBRARY_API int  sumOfDigits(const char* cstr);

// Функция №3 — возвращает true/false в зависимости от длины строки.
LIBRARY_API bool checkStringLength(const char* cstr);

}

#endif // MYLIB_LIBRARY_H