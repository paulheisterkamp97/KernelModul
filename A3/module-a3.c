#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
int main(int argc, char *argv[]){
	printf("Hello World %s\n", (int)argv[1]);
	return 0;
}
