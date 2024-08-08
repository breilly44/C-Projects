#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 100
#define MAX_LINE 1000

void execute_command(char **args, int in, int out);
int chdir(const char *path);

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    if (argc == 1) {
        printf("Welcome to mysh!\n");
        printf("mysh> ");
        while (fgets(line, MAX_LINE, stdin)) {
            printf("mysh> ");
            char *args[MAX_ARGS];
            int n = 0;

            char *token = strtok(line, " \n");
            while (token && n < MAX_ARGS - 1) {
                args[n++] = token;
                token = strtok(NULL, " \n");
            }
            args[n] = NULL;

            if (n == 0) {
                continue;
            }

            if (strcmp(args[0], "exit") == 0) {
                break;
            } else if (strcmp(args[0], "cd") == 0) {
                if (args[1] == NULL) {
                    fprintf(stderr, "Expected argument to \"cd\"\n");
                } else {
                    if (chdir(args[1]) != 0) {
                        perror("cd failed");
                    }
                }
            } else {
               execute_command(args, 0, 1);
            }
        }
        printf("Good-bye!\n");
    } else if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        while (fgets(line, MAX_LINE, file)) {
            char *args[MAX_ARGS];
            int n = 0;
            char *token = strtok(line, " \n");
            while (token && n < MAX_ARGS - 1) {
                args[n++] = token;
                token = strtok(NULL, " \n");
            }
            args[n] = NULL;

            if (n == 0) {
                continue;
            }


            if (strcmp(args[0], "exit") == 0) {
                break;
            } else if (strcmp(args[0], "cd") == 0) {
                if (args[1] == NULL) {
                    fprintf(stderr, "Expected argument to \"cd\"\n");
                } else {
                    if (chdir(args[1]) != 0) {
                        perror("cd failed");
                    }
                }
            } else if (strcmp(args[0], "pwd") == 0) {
                char cwd[MAX_LINE];
                if (getcwd(cwd, sizeof(cwd))!= NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("getcwd() error");
                }
            } else {
               execute_command(args, 0, 1);
            }
        }
        fclose(file);
    } else {
        fprintf(stderr, "Usage: %s [script]\n", argv[0]);
        return 1;
    }
    return 0;
}

void execute_command(char **args, int in, int out) {
    int i = 0;
    int fd[2];
    pid_t pid;

    while (args[i]!= NULL) {
        if (strcmp(args[i], ">") == 0) {
            int output_fd = open(args[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (output_fd < 0) {
                perror("open() error");
                exit(1);
            }
            out = output_fd;
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], ">>") == 0) {
            int output_fd = open(args[i+1], O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (output_fd < 0) {
                perror("open() error");
                exit(1);
            }
            out = output_fd;
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], "<") == 0) {
            int input_fd = open(args[i+1], O_RDONLY);
            if (input_fd < 0) {
                perror("open() error");
                exit(1);
            }
            in = input_fd;
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], "|") == 0) {
            if (pipe(fd) < 0) {
                perror("pipe() error");
                exit(1);
            }
            pid = fork();
            if (pid < 0) {
                perror("fork() error");
                exit(1);
            } else if (pid == 0) {
                if (dup2(fd[1], STDOUT_FILENO) < 0) {
                    perror("dup2() error");
                    exit(1);
                }
                close(fd[0]);
                close(fd[1]);
                execute_command(args + i + 1, in, fd[1]);
                exit(0);
            } else {
                if (waitpid(pid, NULL, 0) < 0) {
                    perror("waitpid() error");
                    exit(1);
                }
                if (dup2(fd[0], STDIN_FILENO) < 0) {
                    perror("dup2() error");
                    exit(1);
                }
                close(fd[0]);
                close(fd[1]);
                args[i] = NULL;
            }
        }
        i++;
    }

    pid = fork();
    if (pid == 0) {
        if (in!= 0) {
            if (dup2(in, STDIN_FILENO) < 0) {
                perror("dup2() error");
                exit(1);
            }
            close(in);
        }
        if (out!= 1) {
            if (dup2(out, STDOUT_FILENO) < 0) {
                perror("dup2() error");
                exit(1);
            }
            close(out);
        }
        execvp(args[0], args);
        perror("Error executing command");
        exit(1);
    } else if (pid < 0) {
        perror("Error forking");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}
