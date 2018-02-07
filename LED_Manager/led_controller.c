#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>

#define NUMBER_OF_LED 4
#define DATA_LENGTH 6
#define LED_FIFO "/tmp/led_fifo"
#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_DIR_PATH	"/sys/class/gpio/gpio"
#define GPIO_DIR_PATH_JOIN "/direction"
#define GPIO_VALUE_PATH_JOIN "/value"
#define	DIR_OUT	"OUT"
#define DIR_IN	"IN"

struct LED_STATE
{
	int state;		// 1 - ON , 2 - TOGGLE , 0 - OFF
	int direction;	// 1 - in and  0 - OUT
	int flag;		// For toggle reference
};

struct LED_STATE GPIO[NUMBER_OF_LED];

pthread_t Led_Controller_Thread;
pthread_t Led_Controller_Processing_Thread;

void *Led_Controller();
void *Led_Controller_Processing();
void pin_ON(int);
void pin_OFF(int);
void update_structure(int, int);

void *Led_Controller()
{
	mkfifo(LED_FIFO, 0666);
	char led_fifo_buff[20];
	char value[3];
	int gpio_number, gpio_value, f_led_fifo = open(LED_FIFO, O_RDWR);
	while(1)
	{
		read(f_led_fifo, led_fifo_buff, DATA_LENGTH);
		usleep(100);
		printf("%s\n",led_fifo_buff);
		if(led_fifo_buff[0] == '#' && (led_fifo_buff[DATA_LENGTH-1] == '!' || led_fifo_buff[DATA_LENGTH-2] == '!'))
		{
			if(led_fifo_buff[2] == '~')
			{
				memset(value, 0, 2);
				value[0] = led_fifo_buff[1];
				sscanf(value, "%d", &gpio_number);
				printf("gpio_value_1 = %d\n",gpio_number);
				if(led_fifo_buff[3] == '1')
					gpio_value = 1;
				else if(led_fifo_buff[3] == '2')
					gpio_value = 2;
				else
					gpio_value = 0;
					
				update_structure(gpio_number, gpio_value);
			}
			else if(led_fifo_buff[3] == '~')
			{
				value[0] = led_fifo_buff[1];
				value[1] = led_fifo_buff[2];
				sscanf(value, "%d", &gpio_number);
				printf("gpio_value_2 = %d\n",gpio_number);
				if(led_fifo_buff[4] == '1')
					gpio_value = 1;
				else if(led_fifo_buff[4] == '2')
					gpio_value = 2;
				else
					gpio_value = 0;
					
				update_structure(gpio_number, gpio_value);
			}
			else
			{
				printf("Some proble is there in packet format\n");
			}
		}
	}
	close(f_led_fifo);
}

void *Led_Controller_Processing()
{	
	int i = 0;	
	while(1)
	{
		for(i =0; i < NUMBER_OF_LED; i++)
		{
			if(GPIO[i].state == 1)
			{
				pin_ON(i);
				GPIO[i].state = 5; // PROCESS EXECUTED
			}
			else if(GPIO[i].state == 0)
			{
				pin_OFF(i);
				GPIO[i].state = 5; // PROCESS EXECUTED			
			}
			else if(GPIO[i].state == 2 && GPIO[i].flag == 1)
			{
				pin_ON(i);
				GPIO[i].flag = 0;
			}
			else if(GPIO[i].state == 2 && GPIO[i].flag == 0)
			{
				pin_OFF(i);
				GPIO[i].flag = 1; 
			}
		}
		usleep(500000);		
	}
}

void pin_ON(int gpio_number)
{
	char buffer[50];
	sprintf(buffer, "%s%d%s %d", GPIO_DIR_PATH, gpio_number, GPIO_VALUE_PATH_JOIN, 1);
	printf("Data = %s\n",buffer);
}

void pin_OFF(int gpio_number)
{
	char buffer[50];
	sprintf(buffer, "%s%d%s %d", GPIO_DIR_PATH, gpio_number, GPIO_VALUE_PATH_JOIN, 0);
	printf("Data = %s\n",buffer);
}

void update_structure(int gpio_number, int gpio_value)
{
	GPIO[gpio_number].state = gpio_value;
}

void main()
{
	if(pthread_create(&Led_Controller_Thread, NULL, Led_Controller, NULL)) {
		fprintf(stderr, "Error creating Led_Controller_Thread\n");
	}

	if(pthread_create(&Led_Controller_Processing_Thread, NULL, Led_Controller_Processing, NULL)) {
		fprintf(stderr, "Error creating Led_Controller_PROCESSING_Thread\n");
	}
	
	if(pthread_join(Led_Controller_Thread, NULL))
	{
		fprintf(stderr, "Error joining Led_Controller_Thread\n");
	}
}
