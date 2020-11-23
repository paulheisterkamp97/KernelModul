#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <limits.h>


int main(int argc, char *argv[]){
	int token[4];//anzahl,min,max,avg
	token[0] = 0;
	token[1] = INT_MAX;
	token[2] = 0;
	token[3] = 0;


	int maxR = 10; // Anzahl Durchläufe
	int subpr = 100; // Anzahl subprozesse

	int procNr = 0; // ID des Prozesses
	int fd[subpr+1][2];//Filedescriptor 
	int id = 1; // Identifier für parent und child prozesse

	for (int i = 0; i < subpr+1; ++i)
	{
		if(pipe(fd[i])==-1){
			printf("Fehler beim erstellen der Pipes\n");
			return 1;
		}
	}

	for (int i = 0; i < subpr; ++i)
	{
		if(id != 0){
			id = fork();
			procNr = i;
		}
		
	}

	if(id != 0){//Parent
		procNr = procNr + 1; //Parent bekommt den letzten Platz in der Przess liste
		struct timeval start, stop;

		write(fd[0][1],token,sizeof(int)*4); // Token Umlauf wird gestartet
		gettimeofday(&start,NULL);
		

		while(token[0] < maxR){
			read(fd[procNr][0], token, sizeof(int)*4);
			gettimeofday(&stop,NULL);
			
			//Zeit nehmen und Token werte aktualisieren
			int time = (int)(stop.tv_usec - start.tv_usec);
			if(token[1]>time)token[1]=time;
			if(token[2]<time)token[2]=time;
			token[3] += time;
			token[0]++;
			
			if(token[0] % 1 == 0){//Ausgabe des Tokens
				printf("Umlauf %d in %d microsekunden\n",token[0],time);
				printf("Durschnitt: %d // Minimal: %d // Maximal: %d\n",token[3]/token[0],token[1],token[2]);
				printf("--------------------------------------------------------\n");
			}
			gettimeofday(&start,NULL);
			write(fd[0][1],token,sizeof(int)*4);			
		}

	}
	if(id == 0 ){//child
		while(token[0] < maxR){
			
			read(fd[procNr][0],token, sizeof(int)*4);
			write(fd[procNr+1][1],token,sizeof(int)*4);
		}

	}
	
	close(fd[procNr][0]);
	close(fd[procNr][1]);
	
	return 0;
}

