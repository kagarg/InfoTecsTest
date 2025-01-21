#include "library.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>

extern "C" {

void sortAndReplaceEvenPositions(char* cstr)
{
    if (!cstr) return;
    std::string input(cstr);

    // 1) Сортируем по убыванию
    std::sort(input.begin(), input.end(), std::greater<char>());

    // 2) Создаём результат, где на чётных индексах вставим "KB", а на нечётных оставим исходный символ.

    std::string output;
    output.reserve(input.size() * 2); // чтобы не было проблем с реаллокациями

    for (size_t i = 0; i < input.size(); ++i) {
        if (i % 2 == 0) {
            // Добавляем "KB"
            output += "KB";
        } else {
            // Добавляем исходный символ
            output.push_back(input[i]);
        }
    }

    // 3) Копируем результат обратно в cstr (с проверкой длины).
    if (output.size() + 1 < 256) {
        std::strcpy(cstr, output.c_str());
    } else {
        std::strncpy(cstr, output.c_str(), 255);
        cstr[255] = '\0';
    }
}
int sumOfDigits(const char* cstr)
{
    if (!cstr) return 0;

    int sum = 0;
    for (; *cstr; ++cstr) {
        if (std::isdigit(*cstr)) {
            sum += (*cstr - '0');
        }
    }
    return sum;
}

bool checkStringLength(const char* cstr)
{
    if (!cstr) return false;
    std::string input(cstr);
    // Условие: длина > 2 и длина кратна 32
    if (input.size() > 2 && (input.size() % 32 == 0)) {
        return true;
    }
    return false;
}

} // extern "C"