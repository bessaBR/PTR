
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

struct periodic_info {
	int timer_fd;
	unsigned long long wakeups_missed;
};

static int make_periodic(unsigned int period, struct periodic_info *info)
{
	int ret;
	unsigned int ns;
	unsigned int sec;
	int fd;
	struct itimerspec itval;

	/* Create timer */
	fd = timerfd_create(CLOCK_MONOTONIC, 0);
	info->wakeups_missed = 0;
	info->timer_fd = fd;
	if (fd == -1)
		return fd;

	/* Make timer periodic */
	sec = period / 1000000;
	ns = (period - (sec * 1000000)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	ret = timerfd_settime(fd, 0, &itval, NULL);
	return ret;
}

static void wait_period(struct periodic_info *info)
{
	unsigned long long missed;
	int ret;

	/* Wait for the next timer event. If we have missed any the
	   number is written to "missed" */
	ret = read(info->timer_fd, &missed, sizeof(missed));
	if (ret == -1) {
		perror("read timer");
		return;
	}

	info->wakeups_missed += missed;
}

int out[1000];
int temp=0;


static void *thread_1(void *arg)
{
	
	clock_t timer;
	timer = clock();
	
	struct periodic_info info;
	make_periodic(1000, &info);

		for(int t=1; t <=1000; t++){	
			out[t] = clock() - timer;
			timer = clock();
			wait_period(&info);
			}
			
		
	return NULL;
}

static void *thread_2(void *arg)
{
	char url[]="PTR.txt";
	int dif=0;
	
	FILE *arq;
	
	arq = fopen(url, "w");
	if(arq == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");
	else{
		for(int i=1;i<=1000;i++){
			
			temp+= out[i];
		}
			
		dif=temp;
		temp+=1000;
		fprintf(arq,"\nTempo Esperado = 1000 ms\n");
		fprintf(arq,"Tempo Decorrido = %u ms\n", temp);
		fprintf(arq,"Diferenca = %d ms\n", dif );
	}
	
	fclose(arq);
			
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t t_1;
	pthread_t t_2;

	
	pthread_create(&t_1, NULL, thread_1, NULL);
	pthread_create(&t_2, NULL, thread_2, NULL);
	
	sleep(10);
	
	return 0;
}
