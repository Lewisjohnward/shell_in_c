#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


#define CLEAR_TERMINAL() printf("\033c")
#define BLUE() printf("\033[36m")
#define RED() printf("\033[91m")
#define DEFAULT() printf("\033[0m")

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
            printf("error");
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

void populate_ENV(char *PATH[], int *PATH_COUNT, char USER[], char HOSTNAME[], char **envp)
{
    gethostname(HOSTNAME, 20);
    for(char **env = envp; *env != 0; env++)
    {
        char *thisEnv = *env;
        char newStr[5];
        int i;
        for(i = 0; i < 4; i++)
            newStr[i] = thisEnv[i];
        newStr[i] = '\0';

        if(!strcmp(newStr, "USER"))
        {
            char str_buffer[20];
            int copy = 0;
            int j = 0;
            for(int i = 0; i < strlen(thisEnv); i++)
            {
                if(thisEnv[i] == '=')
                {
                    copy = 1;
                    i++;
                }
                if(copy)
                {
                    str_buffer[j] = thisEnv[i];
                    j++;
                }
            }
            

            strcpy(USER, str_buffer);
        }

        //if newstring starts with path
        if (!strcmp(newStr, "PATH"))
        {
            char delimiter[2] = ":";
            char *tok = strtok(thisEnv, delimiter);
            int print = 0;
            char *pC = malloc(50);
            int j = 0;
            for(int i = 0; i < strlen(tok); i++)
            {
                if(tok[i] == '=')
                {
                    print = 1;
                    i++;
                }
                if(print)
                {
                    pC[j] = tok[i];
                    j++;
                }
            }
            pC[j] = '\0';
            j = 0;
            PATH[j] = pC;
            *PATH_COUNT = *PATH_COUNT + 1;
            j++;
            tok = strtok(NULL, delimiter);
            while(tok != NULL)
            {
                PATH[j] = tok;
                *PATH_COUNT = *PATH_COUNT + 1;
                j++;
                tok = strtok(NULL, delimiter);
            }
        }
    }
}

void print_command_line(char USER[], char HOSTNAME[], char cwd[])
{
        BLUE();
        printf("%s", USER);
        printf("@");
        printf("%s", HOSTNAME);
        printf(":");
        RED();
        printf("%s", cwd);
        printf("$ ");
        DEFAULT();
}

void handle_cd(char cwd[])
{
    strcpy(cwd, "~");
}

void handler(int num)
{
}

int locate_binary(char command[], char *PATH[], int PATH_COUNT)
{
    DIR *d;
    struct dirent *dir;
    char name[128];
    int found = 0;
    int i;
    for(i = 0; i < PATH_COUNT; i++)
    {
        d = opendir(PATH[i]);
        while((dir = readdir(d)) != NULL)
        {
            strcpy(name, dir->d_name);
            if(strcmp(name, ".") || strcmp(name, ".."))
            {
                if(!strcmp(name, command))
                {
                    char temp[100];
                    strcpy(temp, command);
                    strcpy(command, PATH[i]);
                    strcat(command, "/");
                    strcat(command, temp);
                    found = 1;
                    return found;
                }
            }
        }
        closedir(d);
    }


    return found;
}

int main (int argc, char **argv, char **envp)
{
    CLEAR_TERMINAL();


    char USER[20];
    char HOSTNAME[20];
    char *PATH[20];
    int PATH_COUNT = 0;
    char cwd[200];

    getcwd(cwd, 200);

    populate_ENV(PATH, &PATH_COUNT, USER, HOSTNAME, envp);

//    for(int i = 0; i < 13; i++)
//        printf("%s\n", PATH[i]);

    char command[256];
    int res;

    signal(SIGINT, handler);
      
    while(1)
    {
        print_command_line(USER, HOSTNAME, cwd);
        get_command(command, &res);
        //find_command(command);
        if(res == EOF)
        {
            printf("\n");
            return 0;
        }
        else if(!strcmp(command, "cd"))
        {
            handle_cd(cwd);
        }
        else
        {
            if(locate_binary(command, PATH, PATH_COUNT))
                spawn_child(command);
            else
                printf("%s not found\n", command);
        }
    }
    return 0;
}
