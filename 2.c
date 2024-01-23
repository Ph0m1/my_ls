#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void listFiles(const char *directoryPath) {
    DIR *directory = opendir(directoryPath);
    if (directory == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(directory);
}

int main(int argc, char *argv[]) {
    const char *directoryPath;

    if (argc == 2) {
        // 如果提供了命令行参数，使用命令行参数作为目录路径
        directoryPath = argv[1];
    } else {
        // 否则使用当前目录 
        directoryPath = ".";
    }

    listFiles(directoryPath);

    return EXIT_SUCCESS;
}
