#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <float.h>


int main(int argc, char *argv[]){
	
	int maxR = 100; // Anzahl Durchläufe
	int subpr = 100; // Anzahl subprozesse

	int token = 0;
	int procNr = 0;
	int fd[subpr+1][2];
	int id = 1;

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
		procNr = procNr + 1;
		double Tmin = DBL_MAX;
		double Tcum = 0;
		double Tmax = 0;

		struct timeval start, stop;

		write(fd[0][1],&token,sizeof(int));
		gettimeofday(&start,NULL);
		

		while(token < maxR){
			read(fd[procNr][0],&token, sizeof(int));
			gettimeofday(&stop,NULL);
			token += 1;

			double Tcurr = (double)(stop.tv_usec - start.tv_usec) / 1000;
			if(Tcurr<Tmin)Tmin = Tcurr;
			if(Tcurr>Tmax)Tmax = Tcurr;
			Tcum += Tcurr;
			
			if(token % 10 == 0){
				printf("umlauf %d in %f microsekunden\n",token,Tcurr);
				printf("Durschnitt: %f // Minimal: %f // Maximal: %f \n",Tcum/token,Tmin,Tmax);
			}
			write(fd[0][1],&token,sizeof(int));
			gettimeofday(&start,NULL);
		}

	}
	if(id == 0 ){//child		
		while(token < maxR){
			read(fd[procNr][0],&token, sizeof(int));
			write(fd[procNr+1][1],&token,sizeof(int));
		}

	}
	
	close(fd[procNr][0]);
	close(fd[procNr][1]);
	
	return 0;
}

