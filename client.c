#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>  // Добавили для isdigit()

#define MIN_NUM 1
#define MAX_NUM 100

int is_number(const char *s) {
	if (*s == '\n') return 0;  // Пустая строка

	while (*s && *s != '\n') {  // Проверяем до символа новой строки
		if (!isdigit(*s)) return 0;
		s++;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		return 1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2]))
	};
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	printf("Connected! Guess number (%d-%d). Enter 0 to exit.\n", MIN_NUM, MAX_NUM);

	char buffer[256];
	while (1) {
		printf("Your guess: ");
		if (!fgets(buffer, 255, stdin)) break;  // Если ошибка ввода
		
		// Удаляем символ новой строки
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (!strcmp(buffer, "0")) {
		    write(sockfd, "0\n", 2);
		    break;
		}
		
		if (!is_number(buffer)) {
		    printf("Error: Please enter a valid number\n");
		    continue;
		}
		
		// Добавляем \n обратно для сервера
		strcat(buffer, "\n");
		write(sockfd, buffer, strlen(buffer));
		
		read(sockfd, buffer, 255);
		printf("Server: %s", buffer);
		
		if (strstr(buffer, "Correct!")) {
		    printf("Play again? (0 - no, 1 - yes): ");
		    if (!fgets(buffer, 255, stdin)) break;
		    write(sockfd, buffer, strlen(buffer));
		    
		    if (!strcmp(buffer, "0\n")) break;
		    
		    read(sockfd, buffer, 255);
		    printf("Server: %s", buffer);
		}
	}
	close(sockfd);
	return 0;
}
