#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <cstring>
#include <cctype>
#include <arpa/inet.h>   // для socket/bind/connect и т.д.
#include <unistd.h>      // для close()
#include <csignal>

#include "library.h"     // Наши функции: sortAndReplaceEvenPositions, sumOfDigits

// Общий буфер — очередь строк
static std::queue<std::string> g_queue;
static std::mutex g_mutex;
static std::condition_variable g_cv;

// Флаг для завершения потоков (примитивный способ)
static bool g_finished = false;

// Функция-поток №1: читает с stdin, проверяет строку, вызывает функцию №1, кладёт в очередь
void inputThreadFunc()
{
    while (!g_finished)
    {
        std::string userInput;
        std::cout << "[Thread1] Введите строку (только цифры, не более 64 символов): ";
        if (!std::getline(std::cin, userInput)) {
            // Если ввод прерван — завершаемся
            g_finished = true;
            break;
        }

        // Проверка длины
        if (userInput.size() > 64)
        {
            std::cout << "[Thread1] Ошибка: строка слишком длинная!\n";
            continue;
        }
        // Проверка, что все символы — цифры
        bool allDigits = true;
        for (char c : userInput)
        {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                allDigits = false;
                break;
            }
        }
        if (!allDigits)
        {
            std::cout << "[Thread1] Ошибка: строка содержит нецифровые символы!\n";
            continue;
        }

        // Теперь вызываем Функцию №1 из библиотеки (сортировка + замена).
        // Нужно передавать C-style строку (char*), поэтому скопируем во временный буфер.
        char buffer[256];
        std::memset(buffer, 0, sizeof(buffer));
        std::snprintf(buffer, sizeof(buffer), "%s", userInput.c_str());

        sortAndReplaceEvenPositions(buffer);

        // Положим результат в очередь
        {
            std::unique_lock<std::mutex> lock(g_mutex);
            g_queue.push(std::string(buffer));
        }
        g_cv.notify_one();
    }
}

// Функция-поток №2: извлекает из очереди, печатает, вызывает Функцию №2, пересылает в Программу №2
void workerThreadFunc(const std::string& serverIP, int serverPort)
{
    // Сразу установим соединение с Программой №2 (сервером)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "[Thread2] Ошибка: не удалось создать сокет\n";
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr);

    // Пытаемся подключиться
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Thread2] Ошибка: не удалось подключиться к " 
                  << serverIP << ":" << serverPort << "\n";
        close(sockfd);
        return;
    }
    std::cout << "[Thread2] Успешно подключились к Программе №2!\n";

    while (!g_finished)
    {
        std::string str;
        // Извлекаем из очереди
        {
            std::unique_lock<std::mutex> lock(g_mutex);
            if (g_queue.empty()) {
                // Ждём, когда появятся данные или когда будет завершение
                g_cv.wait(lock, []{ return !g_queue.empty() || g_finished; });
                if (g_finished && g_queue.empty()) {
                    break;
                }
            }
            if (!g_queue.empty()) {
                str = g_queue.front();
                g_queue.pop();
            }
        }

        // Если извлекли пустую строку (в случае завершения), пропустим
        if (str.empty()) {
            continue;
        }

        // Выводим на экран
        std::cout << "[Thread2] Из очереди получена строка: " << str << std::endl;

        // Вызываем Функцию №2 (сумма цифр)
        int sum = sumOfDigits(str.c_str());
        std::cout << "[Thread2] Сумма цифр: " << sum << std::endl;
// По заданию надо отправить данные в Программу №2.
        // Предположим, что отправляем саму строку, а не сумму (или отправляем и то, и другое).
        // Для иллюстрации отправим саму строку.
        // Можно передавать и sum, если того требует задание, — это легко заменить.
        std::string toSend = str + "\n";
        if (write(sockfd, toSend.c_str(), toSend.size()) < 0) {
            std::cerr << "[Thread2] Ошибка при отправке данных в Программу №2\n";
        }
    }

    // Закрываем сокет перед выходом
    close(sockfd);
}
void sigintHandler(int signum){
    exit(0);
}
int main()
{
    signal(SIGINT, sigintHandler); // обрабатываем завершение программы корректным образом

    // Параметры подключения к Программе №2 (серверу)
    std::string serverIP   = "127.0.0.1";
    int         serverPort = 5000;

    // Запускаем поток №1 (ввод пользователя)
    std::thread t1(inputThreadFunc);

    // Запускаем поток №2 (обработка очереди + сокеты)
    std::thread t2(workerThreadFunc, serverIP, serverPort);

    t1.join();
    // Когда поток №1 завершён, ставим флаг, чтобы поток №2 тоже завершился
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_finished = true;
    }
    g_cv.notify_all();
    t2.join();

    std::cout << "[Main] Программа №1 завершается.\n";
    return 0;
}
