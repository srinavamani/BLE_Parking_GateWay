#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_DIR_PATH	"/sys/class/gpio/gpio"
#define GPIO_DIR_PATH_JOIN "/direction"
#define LED_FIFO "/tmp/led_fifo"

void main(int argc, char* argv[])
{
	char buffer[50];
	int fd = open(LED_FIFO, O_RDWR);
	if(argc == 4)
	{
		if(!strcmp(argv[1], "-d"))
		{
			sprintf(buffer, "%s %s", GPIO_EXPORT_PATH, argv[2]);
			printf("%s\n",buffer);
			system(buffer);
			sprintf(buffer, "%s%s%s %s", GPIO_DIR_PATH, argv[2], GPIO_DIR_PATH_JOIN, argv[3]);
			printf("%s\n",buffer);
			system(buffer);
		}
		else if(!strcmp(argv[1], "-o"))
		{
			if(!strcmp(argv[3], "2") || !strcmp(argv[3], "1") || !strcmp(argv[3], "0"))
			{
				sprintf(buffer, "#%s~%s!", argv[2], argv[3]);
				printf("%s\n",buffer);
				write(fd, buffer, strlen(buffer));
			}
			else
				printf("INVALID... GPIO_Value...\n");
		}
	}
	close(fd);
}
