#include <stdio.h>
#include <stdlib.h> // getenv()
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> // mkdir()
#include <sys/types.h> // ssize_t
#include <sys/wait.h> // wait()
#include <dirent.h> // readdir()
#include <errno.h> // errno

void expand_path(char** path){
    if (!*path) return; // check for NULL

    int offset;
    if (*path[0]=='~') offset=1;
    else if (strstr(*path, "$HOME")) offset=5;
    else if (strstr(*path, "${HOME}")) offset=7;
    else return; // nothing to expand

    static char full_path[100];
    strcpy(full_path, getenv("HOME"));
    *path = *path+offset; // erase "~", "$HOME" or "${HOME}"

    strcat(full_path, *path); // full_path = $HOME+path
    *path = full_path;
}

/*  Commands  */
void ls(char* path, _Bool show_all){
    if (!path) path = ".";
    expand_path(&path);
    DIR *directory = opendir(path);
    if (!directory) perror("[ERROR] opendir() returned NULL");
    
    struct dirent *element;
    while (element = readdir(directory))
        if (element->d_name[0]!='.' || show_all)
            printf("  %s\n", element->d_name);
    closedir(directory);
}

void pwd(){ 
    char cur_path[256];
    getcwd(cur_path, sizeof(cur_path));
    if (!cur_path) perror("[ERROR] getwd() returned NULL");
    printf("%s\n", cur_path);
}
void cd(char* path){
    /* Check for an empty path or with '~' ($HOME) */
    if (!path) path = "~";
    expand_path(&path);

    if (chdir(path)) perror("[ERROR] chdir() returned -1");
    pwd();
}
void makedir(char* foldername){
    if (!foldername) {perror("[ERROR] mkdir requires an operand"); return; }

    expand_path(&foldername);
    if (mkdir(foldername, 0777)) perror("[ERROR] chdir() returned -1");
}

void rm(char* filename, _Bool recursive); // declaration

void rm_fulldir(char* dirpath){
    DIR *directory = opendir(dirpath);
    if (!directory) perror("[ERROR] opendir() returned NULL");

    struct dirent *element;
    while (element = readdir(directory)){
        if (!strcmp(element->d_name, ".") || !strcmp(element->d_name, "..")) continue;
        char dirpathcopy[1000];
        strcpy(dirpathcopy, dirpath);
        strcat(dirpathcopy, "/");
        strcat(dirpathcopy, element->d_name);
        rm(dirpathcopy, 1);
    }
    closedir(directory);
}

void rm(char* filename, _Bool recursive){
    if (!filename) { perror("[ERROR] rm requires an operand"); return; }
    expand_path(&filename);
    if (remove(filename)){
        // ENOTEMPTY = dir not empty
        if (errno==ENOTEMPTY && recursive){
            rm_fulldir(filename);
            remove(filename);
        }
        else perror("[ERROR] remove() returned -1");
    }
}

int get_code(char* com){
    char* command_codes[] = {"ls", "pwd", "cd", "mkdir", "rm", "cat", "nano", "exit"};
    for (int i=0; i<sizeof(command_codes)/sizeof(command_codes[0]); ++i) if (!strcmp(command_codes[i], com)) return i;
    return -1;
}

int main(int argc, char const *argv[])
{    
    while (1){
        /* Prompt */
        printf("%sSO@andreavelasquez$%s ","\x1b[34;1m", "\x1b[0m");

        /* Get input */
        ssize_t buffer = 0;
        char *input = NULL;
        getline(&input, &buffer, stdin);

        /* Parse input */
        char* args[32] = {NULL};
        const char delimiter[] = " \n";
        char* token = strtok(input, delimiter);
        
        // Divide input into tokens
        for (int i=0; token; ++i){
            // printf("%s\n", token); // debug
            args[i] = token;
            token = strtok(NULL, delimiter);
        }

        if (!args[0]) continue; // empty input

        /* Run commands */
        switch (get_code(args[0])){
            case 0:
                // ls [NULL] -a
                if (args[1] && args[1][0]=='-') ls(NULL, !strcmp(args[1],"-a"));
                else ls(args[1], args[2] && !strcmp(args[2],"-a"));   
                break;
            case 1:
                pwd();
                break;
            case 2:
                cd(args[1]);
                break;
            case 3:
                makedir(args[1]);
                break;
            case 4:
                // rm -r path
                if (args[2]){
                    if (strcmp(args[1],"-r")) perror("[ERROR] Invalid option");
                    else (rm(args[2], !strcmp(args[1],"-r")));
                }
                // rm path
                else rm(args[1], 0);
                break;
            case 5: // cat
            case 6: // nano
                if (fork()) wait(NULL);
                else execvp(args[0], args);
                break;
            case 7:
                exit()
            default:
                perror("[ERROR] Invalid command");
        }
        errno = 0; // clear errno
    }

    return 0;
}