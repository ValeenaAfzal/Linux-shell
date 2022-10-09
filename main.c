#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<stdbool.h>
#include<string.h>
#define size 100
#define size2 32
void  convert(char str[], char *argv[])//traverse entire line and store strings in argv
{
     while (*str != '\0') //while end not reached
     {
          while (*str == ' ' || *str=='\t' *str=='\n')	//place NUll character at every white space
               *str++ = '\0';    
          *argv++ = str;          //make argv point to start of each new string in str starting after '\0'
          while (*str != '\0' && *str != ' '&& *str!='\t' *str!='\n')//ignore till start of next string
               str++;            
     }
     *argv = NULL;                 //NULL character at end
}
int main()
{
	while(true)
	{
		char  str[size], *argv[size2];
		printf("Enter the commands and arguments separated by space\t");
		gets(str);//take input
		convert(str, argv);// traverse entire line and store in argv
		if (strcmp(argv[0], "exit") == 0)  //checks for exit
			exit(0);
		pid_t pid = fork();
		if(pid < 0)//failed fork
			exit(1);
		else if(pid == 0)//create child and execute
		{
			execvp(argv[0],argv);
			exit(0);
		}
		else//parent
			wait(NULL);  
	}
	return 0;
}
