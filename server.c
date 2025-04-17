#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 100

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		return 1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(atoi(argv[1]))
	};

	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);
	printf("Server started on port %s\n", argv[1]);

	srand(time(NULL));

	while (1) {
		struct sockaddr_in cli_addr;
		socklen_t clilen = sizeof(cli_addr);
		int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
		
		if (fork() == 0) {
		    close(sockfd);
		    char buffer[256];
		    int game_active = 1;
		    
		    while (game_active) {
		        int number = rand() % MAX_NUM + MIN_NUM;
		        printf("New number generated: %d\n", number);
		        
		        while (1) {
		            memset(buffer, 0, 256);
		            read(newsockfd, buffer, 255);
		            
		            if (!strcmp(buffer, "0\n")) {
		                game_active = 0;
		                break;
		            }
		            
		            int guess = atoi(buffer);
		            
		            if (guess == 0 && buffer[0] != '0') {
		                strcpy(buffer, "Error: Please enter a number\n");
		            }
		            else if (guess < MIN_NUM || guess > MAX_NUM) {
		                snprintf(buffer, 256, "Error: Number must be between %d and %d\n", MIN_NUM, MAX_NUM);
		            }
		            else if (guess < number) {
		                strcpy(buffer, "Higher!\n");
		            }
		            else if (guess > number) {
		                strcpy(buffer, "Lower!\n");
		            }
		            else {
		                strcpy(buffer, "Correct! Play again? (0 - no, 1 - yes)\n");
		                write(newsockfd, buffer, strlen(buffer));
		                
		                memset(buffer, 0, 256);
		                read(newsockfd, buffer, 255);
		                
		                if (strcmp(buffer, "1\n") == 0) {
		                    strcpy(buffer, "New game started! Guess a number between 1 and 100\n");
		                    write(newsockfd, buffer, strlen(buffer));
		                    break; // Выходим из внутреннего цикла для генерации нового числа
		                } else {
		                    game_active = 0;
		                }
		            }
		            write(newsockfd, buffer, strlen(buffer));
		        }
		    }
		    close(newsockfd);
		    exit(0);
		}
		close(newsockfd);
	}
}
