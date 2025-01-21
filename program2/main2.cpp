#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <csignal>

#include "library.h" // checkStringLength

void sigintHandler(int signum){
    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandler); // обрабатываем завершение программы корректным образом
    int port = 5000; // порт, на котором слушаем

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        std::cerr << "[Program2] Ошибка: не удалось создать сокет.\n";
        return 1;
    }

    // Позволяем переиспользовать порт после перезапуска
    int optval = 1;
    //setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    sockaddr_in server_addr{};
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // слушать на всех доступных интерфейсах
    server_addr.sin_port        = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[Program2] Ошибка bind.\n";
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        std::cerr << "[Program2] Ошибка listen.\n";
        close(server_sock);
        return 1;
    }

    std::cout << "[Program2] Сервер запущен, порт " << port << ". Ожидание подключений...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_sock < 0) {
            std::cerr << "[Program2] Ошибка accept.\n";
            continue;
        }
        std::cout << "[Program2] Подключился клиент.\n";

        // Читаем данные в цикле, пока клиент не отключится
        while (true) {
            char buffer[1024];
            std::memset(buffer, 0, sizeof(buffer));

            // read/recv может вернуть 0 (закрытие сокета) или -1 (ошибка)
            ssize_t bytesRead = read(client_sock, buffer, sizeof(buffer)-1);
            if (bytesRead <= 0) {
                std::cerr << "[Program2] Клиент отключился или ошибка чтения.\n";
                break;
            }

            // Удаляем перенос строки (если есть)
            std::string received(buffer);
            if (!received.empty() && received.back() == '\n') {
                received.pop_back();
            }

            // Вызываем Функцию №3: checkStringLength
            bool ok = checkStringLength(received.c_str());
            if (ok) {
                std::cout << "[Program2] Получены корректные данные: \"" 
                          << received << "\"\n";
            } else {
                std::cout << "[Program2] Ошибка в полученных данных: \"" 
                          << received << "\"\n";
            }
        }

        close(client_sock);
        std::cout << "[Program2] Ожидаем новое подключение...\n";
    }

    close(server_sock);
    return 0;
}