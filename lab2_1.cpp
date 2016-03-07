#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

using namespace std;

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

    // open source file
    fstream file(argv[1], ios::in);

    // check existence of file
    if (!file.is_open()) {
        file.close();
        return -1;
    }

    // create buffer file and fill it with contents of our file
    string buffer_str;
    fstream buffer_file(BUFFER_NAME, ios::out | ios::in | ios::trunc);

    while (getline(file, buffer_str)) {
        buffer_file << buffer_str << endl;
    }
    file.close();


    file.open(argv[1], fstream::out | fstream::trunc);
    if (stat(argv[1], &sb) == 0 && sb.st_mode & S_IXGRP) {

        // add list of environment variables to beginning of the file
        for (int i = 0; envp[i] != NULL; i++) {
            buffer_str = string(envp[i]);
            unsigned long pos = buffer_str.find('=');
            file << buffer_str.substr(0, pos) << ", ";
        }

    } else {

        // add list of values of the environment variables to beginning of the file
        for (int i = 0; envp[i] != NULL; i++) {
            buffer_str = string(envp[i]);
            unsigned long pos = buffer_str.find('=');
            file << buffer_str.substr(pos + 1, buffer_str.size()) << ", ";
        }

    }
    file << endl;

    buffer_file.seekg(buffer_file.beg);
    // put source data from buffer to our file
    while (getline(buffer_file, buffer_str)) {
        file << buffer_str << endl;
    }

    // close streams and remove buffer file
    file.close();
    buffer_file.close();
    remove(BUFFER_NAME);
    return 0;
}