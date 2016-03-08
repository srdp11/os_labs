#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

const int BUFF_SIZE = 100;
const char* BUFFER_NAME = "_temp.txt";

/*
 * В качестве параметров командной строки программа получает имя файла, если файл имеет
разрешения на выполнение, в его начало добавляется список имен всех переменных
окружения через запятую, не имеет – список их значений.
 */

int main(int argc, char *argv[], char *envp[]) {
    struct stat sb;

    // check for args
    if (argc < 1) {
        return -1;
    }

    //open source file
    int source_file = open(argv[1], O_RDONLY);
    if (source_file == -1) {
        return -1;
    }

    // create buffer file and fill it with contents of our file
    char buffer[BUFF_SIZE];
    int buffer_file = open(BUFFER_NAME, O_RDWR | O_CREAT, S_IRWXU);
    ssize_t num_readed_sym;
    while ((num_readed_sym = read(source_file, &buffer, BUFF_SIZE)) > 0) {
        write(buffer_file, &buffer, num_readed_sym);
    }
    close(source_file);

    source_file = open(argv[1], O_WRONLY);
    mode_t mode = S_IXGRP; // execute permission for group

    int j;
    const size_t delimiter_size = 3;
    char delimiter[delimiter_size] = ", ";
    if (stat(argv[1], &sb) == 0 && sb.st_mode & mode) {

        // add list of environment variables to beginning of the file
        for (int i = 0; envp[i] != NULL; i++) {
            bool is_find_sym_equal = false;
            for (j = 0; j < strlen(envp[i]) ; j++) {
                if (envp[i][j] != '=' && !is_find_sym_equal) {
                    continue;
                } else if(is_find_sym_equal) {
                    write(source_file, &envp[i][j], sizeof(char));
                    continue;
                }
                is_find_sym_equal = true;
            }
            write(source_file, &delimiter, delimiter_size * sizeof(char));
        }

    } else {

        // add list of values of the environment variables to beginning of the file
        for (int i = 0; envp[i] != NULL; i++) {
            for (j = 0; j < strlen(envp[i]); j++) {
                if (envp[i][j] == '=') {
                    break;
                }
            }
            write(source_file, envp[i] + j + 1, strlen(envp[i] + j + 1) * sizeof(char));
            write(source_file, &delimiter, delimiter_size * sizeof(char));
        }

    }

    // line break
    write(source_file, (char *)"\n", sizeof(char));

    // put source data from buffer to our file
    lseek(buffer_file, 0, SEEK_SET);
    while((num_readed_sym = read(buffer_file, &buffer, BUFF_SIZE)) > 0) {
        write(source_file, &buffer, num_readed_sym);
    }

    // close files and remove buffer file
    close(source_file);
    close(buffer_file);
    unlink(BUFFER_NAME);
}