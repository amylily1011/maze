#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>


int main(int argc,char *argv[])
{

int val;
pid_t pid;
FILE* fileHowtoplay,*filescore;
char string [100],name[10],string2[100];
int pipefd[2],nbyte, data;
char buffer[BUFSIZ+1];

filescore=fopen("score","r+");


while(1){				//loop this program forever until user press 4 to exit
 memset(buffer, '\0', sizeof(buffer));
    int i,j;
do{system("clear");		//print MENU
printf("****Maze 3D****\n");
 
        
        for(j=4;j>0;j--){
            printf("*       *  *        *\n");
            printf(" *     *    *      *\n");
            printf("  *   *       *   *\n");
        }
    
    
printf("Press 1 Play \n");
printf("Press 2 Instructions\n");
printf("Press 3 Score Board\n");
printf("Press 4 Exit\n");
scanf("%d",&val);
}while(val<0||val>4);		



switch(val){
case 1: 
	
	if(pipe(pipefd)<0)exit(1);	//will exit if error when creating pipe	
	
	pid=fork();			//create another processes
	
    switch (pid){
	
    case 0://child process 
	
            close(pipefd[0]);	//close input end of the pipe because child will only write to pipe
            sprintf(buffer, "%d", pipefd[1]); 	//print pipefd[1] to buffer
            execl("maze","maze",buffer,NULL); //replace the child process with argument and send buffer as argument
            exit(EXIT_FAILURE); break;

    default:	//parent process
	
            close(pipefd[1]);   	 //close output end of the pipe
            nbyte=read( pipefd[0],&data,sizeof(int)); // read the information in the pipe and save in data
            pause();     		// wait for the child signal	
	
            if(data>0)
                {printf("Please enter your name:\n");
                        scanf("%s",&name);
                        fseek(filescore,0,SEEK_END);
                        fprintf(filescore,"%s %d\n",name,data);		//save the name and scores to file
                        } break;
    }
	
        break;

case 2: fileHowtoplay=fopen("how","r");
	
            system("clear");
        
            while ( fgets (string , 100 , fileHowtoplay) != NULL )	//print out information on how to play this game from file 
                    puts(string);	
	
	       getchar();getchar();break;

case 3: system("clear");
        fseek(filescore,0,SEEK_SET);
        printf("##########SCORE BOARD##########\n"); 			//Print the scores from file 
        
        while ( fgets (string2 , 100 ,filescore) != NULL )
            { printf("%s",string2);	}
        
        printf("\nPress Enter to go back to MENU.");
        getchar();getchar();break;
case 4:exit(0);
}

}
}
