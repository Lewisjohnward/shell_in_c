#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void spawn_child(char *command)
{
    pid_t pid;
    int exec;
    pid = fork();
    
    //Child process has pid of 0
    if(pid == 0)
    {
        //printf("I am the child\n");
        char *args[] = {command, NULL};
        exec = execv(args[0], args);
        if (exec == -1)
        {
            printf("%s not found\n", command);
            exit(0);
        }
    }
    else if(pid == -1)
    {
        printf("Unable to fork process\n");
    }
    //Parent process has a non zero pid
    else 
    {
        wait(0);
        //printf("I am the parent\n");
    }
}
void get_command(char *command, int *res)
{
    *res = scanf("%s", command);
}

int main (void)
{
    char command[256];
    int res;
    while(1)
    {
        printf("$ ");
        get_command(command, &res);
        if(res == EOF)
        {
            printf("\n");
            return 0;
        }
        else
            spawn_child(command);
    }
    return 0;
}
