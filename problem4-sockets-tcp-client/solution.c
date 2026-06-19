#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <signal.h>

// Программе передаются два аргумента:
//   argv[1] — IPv4-адрес сервера в десятичной записи (например, "127.0.0.1")
//   argv[2] — номер порта
//
// Программа должна:
//   1. Установить TCP-соединение с указанным сервером.
//   2. В цикле читать со stdin целые знаковые числа в текстовом формате.
//   3. Отправлять каждое число на сервер в бинарном виде (int32, Little Endian).
//   4. Получать от сервера int32 LE в ответ и выводить его в stdout в текстовом виде.
//   5. Если сервер закрыл соединение — завершиться с кодом возврата 0.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ipv4_addr> <port>\n", argv[0]);
        return 1;
    }

    // TODO: создайте TCP-сокет (AF_INET, SOCK_STREAM),
    //       заполните struct sockaddr_in с помощью inet_aton/inet_pton,
    //       подключитесь через connect,
    //       реализуйте цикл чтения/отправки/приёма/вывода чисел.
    //       Порядок байт — Little Endian (на x86/x86_64 это нативный порядок).
  signal(SIGPIPE, SIG_IGN);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    int value;
    while (scanf("%d", &value) == 1) {
        int32_t send_value = (int32_t)value;
        char *send_ptr = (char *)&send_value;
        size_t sent = 0;

        while (sent < sizeof(send_value)) {
            ssize_t res = send(sock, send_ptr + sent, sizeof(send_value) - sent, 0);

            if (res < 0) {
                if (errno == EINTR) {
                    continue;
                }
                close(sock);
                return 0;
            }

            if (res == 0) {
                close(sock);
                return 0;
            }

            sent += res;
        }

        int32_t recv_value;
        char *recv_ptr = (char *)&recv_value;
        size_t received = 0;

        while (received < sizeof(recv_value)) {
            ssize_t res = recv(sock, recv_ptr + received, sizeof(recv_value) - received, 0);

            if (res < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("recv");
                close(sock);
                return 1;
            }

            if (res == 0) {
                close(sock);
                return 0;
            }

            received += res;
        }

        printf("%d\n", recv_value);
        fflush(stdout);
    }

    close(sock);
    return 0;
}
