#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>

void* threadFunc(void *arg){
	printf("starting another thread... \n");
	int fd2=0,ret2=0;
	char buff2[80]="";
	fd2=open("/dev/my_device",O_RDONLY);
	printf("fd2 :%d\n",fd2);
	ret2=read(fd2,buff2,10);
	buff2[ret2]='\0';
	printf("buff2: %s ; length: %d bytes\n",buff2, ret2);
	close(fd2);

	return NULL;
}


int main(void){

	pthread_t pth;//thread identifier
	pthread_create(&pth, NULL, threadFunc, "foo");

	int fd=0,ret=0;
	char buff[80]="";	
	fd=open("/dev/my_device",O_RDONLY);
	
	printf("fd :%d\n",fd);	
	ret=read(fd,buff,10);
	pthread_join(pth,NULL);//
	buff[ret]='\0';	
	printf("buff: %s ;length: %d bytes\n",buff,ret);
	close(fd);

	
	//pthread_join(pth,NULL);

}
