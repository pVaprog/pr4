#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int count_client=1;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
      // Приведение типа и получение значений
    struct sockaddr_in *client_info = (struct sockaddr_in *)&cli_addr;

    // Получение IP-адреса клиента
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_info->sin_addr, client_ip, sizeof(client_ip));
    
    // Получение порта клиента
    int client_port = ntohs(client_info->sin_port);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        if (fork() == 0) {
            close(sockfd); 
            
            char buffer[BUFFER_SIZE];
            int number = rand() % 100 + 1;
            int guess;

            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_read = read(newsockfd, buffer, BUFFER_SIZE - 1);
                if (bytes_read <= 0) {
                    perror("Read failed or client disconnected");
                    break;
                }
                buffer[bytes_read] = '\0';

                guess = atoi(buffer);
                printf("Клиент %s:%d  ввёл: %d\n", client_ip, client_port ,guess);

                if (guess < number) {
                    strcpy(buffer, "Загаданое число больше!\n");
                } else if (guess > number) {
                    strcpy(buffer, "Загаданое число мельше!\n");
                } else {
                    strcpy(buffer, "Поздравляем! Вы угадали число!\n");
                    count_client++;
                    send(newsockfd, buffer, strlen(buffer), 0);
                    break;
                }

                send(newsockfd, buffer, strlen(buffer), 0);
            }

            close(newsockfd);
            
            exit(0); 
            count_client++;
        }
        close(newsockfd); 
    }

    close(sockfd); 
    return 0;
}

