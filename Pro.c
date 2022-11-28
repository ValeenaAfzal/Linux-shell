#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<stdbool.h>
#include<string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define size 100
#define size2 32

extern int errno;
int commandCount=0;//to store count
char history[10][size]; //history array to store history commands
int count = 0; // it checks if count exceeds 10

void Update(char str[])// 0 or 9 
{
	commandCount++;// increment command count
	if(commandCount < 11 ) // to fill first 10 commands 
	{
		strcpy(history[count], str);
		count++;
	}
	else //after 10 
	{
		count= 9;
		int i=0;
		for(; i<count ;i++)// move backward
			strcpy(history[i],history[i+1] );
		strcpy(history[count],str);//place latest command on top
	}
}

void display()
{
	if(commandCount ==0)
		printf("Error:\t History is Empty\n");
	else if(commandCount < 10)// if there are less than 10 command in history
	{
		int hist = count-1; //9 
		int i=hist;
		int j=0;
		int number=commandCount;
		for(; i>-1;i--)
		{
			printf("%d.  ", number);
			while (history[i][j] != '\n' && history[i][j] != '\0')
			{	
			    printf("%c", history[i][j]);
			    j++;
			}
			printf("\n");
			j = 0;
			number--;
		}
	}
	else
	{
		int i=9;
		int j=0;
		int number=commandCount;
		for(; i>-1;i--)
		{
			printf("%d.  ", number);
			while (history[i][j] != '\n' && history[i][j] != '\0')
			{	
			    printf("%c", history[i][j]);
			    j++;
			}
			printf("\n");
			j = 0;
			number--;
		}
	}
}

void retreive(char* arr, char cmd[])
{
	if(arr[1] == '!')//get most recent
	{
		if(commandCount < 10)//if count is less than 10
			strcpy(cmd,history[count-1]);
		else 
			strcpy(cmd,history[9]);// top in case of count > 10
		Update(cmd);
	}
	else if((int)arr[1] > 47 && (int)arr[1] < 58)//Command out of Range // negative value check
	{
		arr[0]='0';
		int y = atoi(arr);
		if(y > commandCount || y <= commandCount-10)	//check if the number is between last 10 or not //comand-10 error
			printf("Error:\t Command out of Range\nError: %s\n",strerror(errno));
		else
		{
			if(y == 0 || y < 0)//if !0
				printf("Error:\t Wrong Command - Enter Value between 1-10\nError: %s\n",strerror(errno));
			else
			{
				if (commandCount < 10)
					strcpy(cmd,history[y-1]);
				else
				{
				
					int cmdn=0;
					int l=commandCount-9;
					while (l != y) 
					{
						l++;
						cmdn++;
					}
					strcpy(cmd,history[cmdn]);
				}
				Update(cmd);
				printf("%s\t%d",cmd,count);
			}
		}
	}
	else
		printf("Error:\tWrong Entry\nError: %s\n",strerror(errno));
}

int  convert(char str[], char *argv[], int *re, int* err)//traverse entire line and store strings in argv
{
	int ISPipe=0;
	while (*str != '\0') //while end not reached
	{	
		while (*str == ' ' || *str== '\t' || *str == '\n' )	//place NUll character at every white space
			*str++ = '\0';
		*argv++= str;          //make argv point to start of each new string in str starting after '\0'
		//printf("%s",*argv);
		//char* ff = strstr(*argv,"2>");
		if(*str == '2' && *(str+1) == '>')
			*re=*re+1;
			//err++;
		if(*str == '|')  
			ISPipe++;
		if(*str == '>' || *str == '<')
			*re=*re+1;
		while (*str != '\0' && *str != ' ' && *str!= '\t' && *str!='\n')//ignore till start of next string
			str++;
			     
	}
	*argv = NULL;                 //NULL character at end
	return ISPipe;
}

int OutRedirection(char **argv, int i)
{
	int file_des = open(argv[i + 1], O_WRONLY | O_CREAT, 0644);//open file 
	//printf("%s",argv[i+1]);
        if (file_des == -1 || argv[i+1]  == NULL)
        {
            printf("Invalid Command!\nError: %s\n",strerror(errno));
            exit(1);
        }
        bool flag = false;
      	int z=i+2;
      	while(argv[z] != NULL)// if there are arguments at higher indeces move back 
      	{
		if(argv[z])
		{
			argv[z-2]=argv[z];// move
			argv[z]=NULL;
			z++;	
		}
		flag=true;
	}
	//argv[z]=NULL;
	if(!flag)
		argv[i]=NULL, argv[i+1]=NULL;
        dup2(file_des, STDOUT_FILENO);
	return file_des;
}

int INRedirection(char **argv, int i)
{

	int file_des = open(argv[i + 1], O_RDONLY | O_CREAT, 0644);//open file 
	//printf("%s",argv[i+1]);
        if (file_des == -1 || argv[i+1]  == NULL)
        {
            printf("Invalid Command!\nError: %s\n",strerror(errno));
            exit(1);
        }
        bool flag = false;
      	int z=i+2;
      	while(argv[z] != NULL)// if there are arguments at higher indeces move back 
      	{	
		if(argv[z])
		{
			argv[z-2]=argv[z];// move
			argv[z]=NULL;
			z++;	
		}
		flag=true;
	}
	if(!flag)
		//argv[i]=" ", argv[i+1]= " ";
	dup2(file_des, STDIN_FILENO);
	return file_des;
}



void make_child(int read, int write, char* argv[])//int to void
{
//	pid_t pid = fork ();
	/*if(pid < 0)
		printf("Error:\tFork Failed\nError: %s\n",strerror(errno));*/
	//if (pid == 0)
	//{
		if (read!= 0)
		{
			dup2 (read, 0);
			close (read);
		}
		if (write != 1)
		{
			dup2 (write, 1);
			close (write);
		}
		//execvp (argv [0],argv);
		if(execvp (argv [0],argv) < 0)
		{
			printf("Error:\tExecvp Failed\nError: %s\n",strerror(errno));
			exit(0);
		}
	/*}
	else
	{
		
		int status=0;
		wait(&status);	
	}*/
}

void for_pipe(int n, char* argv[])//n=number of pipes , argv = complete command abc | efg | lkm
{
	int i, in, fd [2] ,start=0,v=0,w=0;
	pid_t pid;
	in = 0;//for process one
	for (i = 0; i < n; ++i)
	{
		char* args[size2];
		for(v=start,w=0; (strcmp(argv[v], "|")!=0) ;v++,w++)
		{
			args[w]= argv[v];
			//	printf("%s\t",args[w]);	
		}
		start = v+1;
		args[w]=NULL;
		pipe (fd);
		int pid4=fork();
		  if(pid4 == 0){
           make_child(in, fd [1], args);
        }
        else if(pid4 < 0) {
            printf("forking failed\n");
        }
        else {
            wait(NULL);
        }
		//in = read fd[0] of prev pipe, fd[1] write end of current
		close (fd [1]);//close write end, the child will write here
		in = fd [0];//save read end of prev , next child will read from here
	}
	int isOut=0;
	int isIn=0;
	int file_des;
	char* args[size2];
	for(v=start,w=0; argv[v] != NULL ;v++,w++)
	{
		args[w]= argv[v];
		/*printf("%s",args[w]);
		if(strcmp(args[w], ">") == 0)
			isOut=w;
		else if(strcmp(args[w], "<") == 0)
			isIn=w;*/
	}
	args[w]=NULL;// yaha pe redirect wese check krna jese dusry redirect me kia
	/*
	if(isOut !=0 && isIn != 0)
	{
		file_des=INRedirection(args,isIn);
		int ff=0;
		for(;args[ff]!= NULL;ff++)
			if(strcmp(args[ff], ">") == 0)
				isOut=ff;
		file_des=OutRedirection(args,isOut);
	}
	else if(isOut !=0)
		file_des=OutRedirection(args,isOut);
	else if(isIn != 0)
		file_des=INRedirection(args,isIn);*/
	//______________________________________________
	/*
	if(strcmp(argv[v], ">") == 0)// check for append >>
	{ 
	file_des=OutRedirection(argv,i);
	break;
	}//Input Redirection
	else if(strcmp(argv[i], "<") == 0)// input redirection
	{ 
	file_des=INRedirection(argv,i);
	break;
	}
	*/
	//_________________________________________________

	/* Last stage of the pipeline - set stdin be the read end of the previous pipe
	and output to the original file descriptor 1. */  
	if (in != 0)
		dup2 (in, 0);
	//close(fd[0]);
	//close(fd[1]);
	//int pidd= fork();// in case of pipes last process & in case of 0 pipes 
	//if(pidd==0){
		execvp (args [0],args);//last process
	//	exit(0);	
	//}
	/*else if(pidd > 0)
	{
		int status =0;
		wait(&status);
	}
	else*/
		//printf("Error:\tFork Failed\nError: %s\n",strerror(errno));
	/*if(isOut != 0)
		 close(STDOUT_FILENO);
	if(isIn != 0)
		close(STDIN_FILENO);
	close (file_des);
	//dup(stdout);*/
	//return;
}

void Redirect(char * argv[])
{
	//int pid = fork();
	//if (pid == 0) 
	  {
		int redirectCase = 0;
		int file_des;
		int i=0;
		for (; i < argv - 1; i++)
		{
			
			//printf("%s",argv[i]);
			if(strcmp(argv[i], ">") == 0)// check for append >>
			{ 
				redirectCase = 2;
				file_des=OutRedirection(argv,i);
				break;
			}//Input Redirection
			else if(strcmp(argv[i], "<") == 0)// input redirection
			{ 
				redirectCase = 1;
				file_des=INRedirection(argv,i);
				//printf("%d",i);
				break;
			}
			else if(strcmp(argv[i], "2>") == 0)//error redirection
			{
				printf("%d",i);
				redirectCase = 3;
				file_des= open(argv[i+1], O_CREAT | O_WRONLY, 0644);
				if (file_des == -1 || argv[i+1]  == NULL)
				{
					printf("Invalid Command!\nError: %s\n",strerror(errno));
					exit(1);
				}
				dup2(file_des, 2);
		                argv[i] = NULL;
		                argv[i + 1] = NULL;
				break;
			}
		}
		
		//printf("%s",argv[0]);
		if (execvp(argv[0], argv) < 0) 
		{
			printf("Error:\tExecvp Failed\nError: %s\n",strerror(errno));
			exit(1);
		}
		if (redirectCase == 1)
			close(STDIN_FILENO);
		else if (redirectCase == 2)
			close(STDOUT_FILENO);
		else if (redirectCase == 3);
			close(STDOUT_FILENO);
		close(file_des);
		//exit(1);
	} 
	//else if (pid > 0) 
	//	wait(NULL);
	//else 
	//	printf("Error:\tFork Failed\nError: %s\n",strerror(errno));
}

// nulll argv 
int main()//inlcude pipe failure fork failure read write failure messages
{
	while(true)
	{
		char  str[size], *argv[size2], copy[size];
		
		printf("\nEnter the commands and arguments separated by space:$\t");
		gets(str);//take input
		strcpy(copy,str);
		int IsPipe =0, IsRedirect =0 ,errorRedierct=0;
		
		IsPipe = convert(str, argv, &IsRedirect, &errorRedierct);// traverse entire line and store in argv
		
		if (strcmp(argv[0], "exit") == 0)  //checks for exit
		{
			printf("\nQuitting Shell~!\n");
			exit(0);	
		}
		
		else if (strcmp(argv[0], "cd") == 0) // change directory
			chdir(argv);
			
		else if(strcmp(argv[0], "history") == 0)//isko b history me dalna
		{	
			display(); 
			//Update(copy);
		}
		
		else if(str[0] =='!')
		{
			//IsPipe=0;
			//IsRedirect=0;
			retreive(str, str);
			int pidd=fork();
			if(pidd==0)
			{
				IsPipe = convert(str, argv,&IsRedirect, &errorRedierct);
				if(IsRedirect != 0 && IsPipe == 0)//if pipe == 0
					Redirect(argv);
				else 
					for_pipe (IsPipe, argv);//idhr b child  lgao
			}
			else if(pidd>0)
				wait(NULL);
			else
				printf("Error:\tFork Failed\nError: %s\n",strerror(errno));
				
			
		}
		
		else if(IsRedirect != 0  && IsPipe == 0)//if pipe == 0
		{
			Update(copy);
			//printf("here ");
			int pidd=fork();
			if(pidd==0)
				Redirect(argv);
			else if(pidd>0)
				wait(NULL);
			else
				printf("Error:\tFork Failed\nError: %s\n",strerror(errno));
			
		}
		else// if(IsPipe == 0)// issi case me update()
		{
			//printf("here in pipe");
			Update(copy);
			int pidd=fork();
			if(pidd==0)
				for_pipe(IsPipe, argv);
			else if(pidd>0)
				wait(NULL);
			else
				printf("Error:\tFork Failed\nError: %s\n",strerror(errno));
		}
	}
	return 0;
}
