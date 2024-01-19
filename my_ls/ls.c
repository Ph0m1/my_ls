#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>

#define MAX_PATH 1024

#define FLAG_L 0b00000001
#define FLAG_R 0b00000010
#define FLAG_T 0b00000100
#define FLAG_R2 0b00001000
#define FLAG_S 0b00010000
#define FLAG_I 0b00100000
#define FLAG_A 0b01000000

#define NONE "\033[m"
#define GREEN "\033[0;32;32m"
#define BLUE "\033[0;32;34m"

typedef struct
{
    __ino_t inode;
    char filename[MAX_PATH];
    struct stat st;
} FileInfo, *FFileInfo;

typedef struct Node
{
    char path[MAX_PATH];
    struct Node *next;
} Node, *Stack;

size_t maxFilenameWidth = 0;
size_t maxSizeWidth = 0;

void printLs_R(const char *dirPath, int flags);
void listFiles(const char *path, int flags);
void printFileInfo(FileInfo *fileInfo, int flags);

int compareFileInfoAsc(const void *a, const void *b)
{
    setlocale(LC_COLLATE, ""); // 设置本地化环境
    return strcoll(((FileInfo *)a)->filename, ((FileInfo *)b)->filename);
}

int compareFileInfoDesc(const void *a, const void *b)
{
    setlocale(LC_COLLATE, ""); // 设置本地化环境
    return strcoll(((FileInfo *)b)->filename, ((FileInfo *)a)->filename);
}

void push(Stack *stack, const char *path)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        perror("Error allocating memory for stack node");
        exit(EXIT_FAILURE);
    }

    strncpy(newNode->path, path, MAX_PATH);
    newNode->next = *stack;
    *stack = newNode;
}

void pop(Stack *stack)
{
    if (*stack == NULL)
    {
        fprintf(stderr, "Error: Trying to pop from an empty stack.\n");
        exit(EXIT_FAILURE);
    }

    Node *temp = *stack;
    *stack = temp->next;
    free(temp);
}

int isStackEmpty(const Stack stack)
{
    return stack == NULL;
}

int main(int argc, char *argv[])
{
    int flags = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                switch (argv[i][j])
                {
                case 'a':
                    flags |= FLAG_A;
                    break;
                case 'l':
                    flags |= FLAG_L;
                    break;
                case 'R':
                    flags |= FLAG_R;
                    break;
                case 't':
                    flags |= FLAG_T;
                    break;
                case 'r':
                    flags |= FLAG_R2;
                    break;
                case 'i':
                    flags |= FLAG_I;
                    break;
                case 's':
                    flags |= FLAG_S;
                    break;
                }
            }
        }
    }

    if (argc == 1)
    {
        listFiles(".", flags);
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] != '-')
            {
                listFiles(argv[i], flags);
            }
        }
    }

    return 0;
}


void printLs_R(const char *dirPath, int flags)
{
    Stack stack = NULL;
    push(&stack, dirPath);

    while (!isStackEmpty(stack))
    {
        char currentPath[MAX_PATH];
        strncpy(currentPath, stack->path, MAX_PATH);
        pop(&stack);

        DIR *dir = opendir(currentPath);
        if (dir == NULL)
        {
            perror("Error opening directory");
            exit(EXIT_FAILURE);
        }

        struct dirent *entry;
        int count = 0;
        FFileInfo fileInfo = (FFileInfo)malloc(sizeof(FileInfo));

        while ((entry = readdir(dir)) != NULL)
        {
            if (!(flags & FLAG_A) && entry->d_name[0] == '.')
            {
                continue;
            }

            char fullPath[MAX_PATH];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", currentPath, entry->d_name);

            struct stat st;
            if (stat(fullPath, &st) == -1)
            {
                // perror("Error getting file status");
                // exit(EXIT_FAILURE);
            }

            count++;
            fileInfo = (FFileInfo)realloc(fileInfo, count * sizeof(FileInfo));
            fileInfo[count - 1].inode = entry->d_ino;
            strcpy(fileInfo[count - 1].filename, entry->d_name);
            fileInfo[count - 1].st = st;
        }

        // 根据是否有 -r 标志选择比较函数
        int (*compareFunction)(const void *, const void *);

        if (flags & FLAG_R2)
        {
            compareFunction = compareFileInfoDesc;
        }
        else
        {
            compareFunction = compareFileInfoAsc;
        }

        // 排序文件信息数组
        qsort(fileInfo, count, sizeof(FileInfo), compareFunction);

        // 打印排序后的文件信息
        for (int i = 0; i < count; i++)
        {
            printFileInfo(&fileInfo[i], flags);
        }

        for (int i = 0; i < count; i++)
            if ((flags & FLAG_R) && S_ISDIR(fileInfo[i].st.st_mode) && strcmp(fileInfo[i].filename, "..") != 0 && strcmp(fileInfo[i].filename, ".") != 0)
            {
                char tPath[MAX_PATH];
                snprintf(tPath, sizeof(tPath), "%s/%s", currentPath, fileInfo[i].filename);
                printf("\n%s%s%s:\n", BLUE, fileInfo[i].filename, NONE);
                push(&stack, tPath);
            }

        free(fileInfo); // 释放动态分配的内存
        closedir(dir);
    }
}

void listFiles(const char *path, int flags)
{
    struct stat st;
    if (stat(path, &st) == -1)
    {
        perror("Error getting file status");
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(st.st_mode))
    {
        printLs_R(path, flags);
    }
    else
    {
        FileInfo fileInfo;
        fileInfo.inode = st.st_ino;
        strcpy(fileInfo.filename, path);
        fileInfo.st = st;
        printFileInfo(&fileInfo, flags);
    }
}

void printFileInfo(FileInfo *fileInfo, int flags)
{
    if (!(flags & FLAG_A) && fileInfo->filename[0] == '.')
    {
        return;
    }

    if (flags & FLAG_I)
    {
        printf("%lu ", fileInfo->inode);
    }

    if (flags & FLAG_S)
    {
        printf(" %*lu", (int)maxSizeWidth, (fileInfo->st.st_blocks * 512) / 1024);
    }

    if (flags & FLAG_L)
    {
        char perms[11];
        perms[0] = (S_ISDIR(fileInfo->st.st_mode)) ? 'd' : '-';
        perms[1] = (fileInfo->st.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (fileInfo->st.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (fileInfo->st.st_mode & S_IXUSR) ? 'x' : '-';
        perms[4] = (fileInfo->st.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (fileInfo->st.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (fileInfo->st.st_mode & S_IXGRP) ? 'x' : '-';
        perms[7] = (fileInfo->st.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (fileInfo->st.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (fileInfo->st.st_mode & S_IXOTH) ? 'x' : '-';
        perms[10] = '\0';

        printf("%s %2lu", perms, fileInfo->st.st_nlink);
        printf(" %5u", fileInfo->st.st_uid);
        printf(" %5u", fileInfo->st.st_gid);
        printf(" %*lld", (int)maxSizeWidth, (long long)fileInfo->st.st_size);
        printf(" %.12s", ctime(&fileInfo->st.st_mtime) + 4);
    }
    //输出文件名称
    const char *colorCode = (S_ISDIR(fileInfo->st.st_mode)) ? BLUE : (fileInfo->st.st_mode & S_IXUSR || fileInfo->st.st_mode & S_IXGRP || fileInfo->st.st_mode & S_IXOTH) ? GREEN
                                                                                                                                                                          : NONE;

    printf(" %s%s%s", colorCode, fileInfo->filename, NONE);
    printf("\n");
}
