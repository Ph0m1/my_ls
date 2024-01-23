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
    char targetPath[MAX_PATH]; // 用于存储链接的目标路径
    time_t mTime;
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
// 比较函数
// 字典
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

// 时间
int compareFileInfoAscByTime(const void *a, const void *b)
{
    return ((FileInfo *)a)->mTime - ((FileInfo *)b)->mTime;
}

int compareFileInfoDescByTime(const void *a, const void *b)
{
    return ((FileInfo *)b)->mTime - ((FileInfo *)a)->mTime;
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
        // exit(EXIT_FAILURE);
        return;
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
    int t = 0;
    int tflag = 1;
    if (argc != 1)
    {
        for (t = 1; t < argc; t++)
        {
            if (argv[t][0] != '-')
            {
                tflag = 0;
                listFiles(argv[t], flags);
            }
        }
    }
    if (tflag && (!argv[t] && argc != 1) || argc == 1)
    {
        listFiles(".", flags);
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
            perror("Error opening directory!");
            return;
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
            if (lstat(fullPath, &st) == -1) // 使用 lstat 来获取链接信息
            {
                perror("Error getting file status");
                continue;
            }

            count++;
            fileInfo = (FFileInfo)realloc(fileInfo, count * sizeof(FileInfo));
            fileInfo[count - 1].inode = entry->d_ino;
            strcpy(fileInfo[count - 1].filename, entry->d_name);
            fileInfo[count - 1].st = st;
            fileInfo[count - 1].mTime = st.st_mtime;

            // 如果是链接，获取链接的目标路径
            if (S_ISLNK(st.st_mode))
            {
                ssize_t targetSize = readlink(fullPath, fileInfo[count - 1].targetPath, MAX_PATH - 1);
                if (targetSize != -1)
                {
                    fileInfo[count - 1].targetPath[targetSize] = '\0';
                }
                else
                {
                    perror("Error reading symlink target");
                }
            }
            else
            {
                fileInfo[count - 1].targetPath[0] = '\0';
            }
        }

        // 根据是否有 -r -t 标志选择比较函数
        int (*compareFunction)(const void *, const void *);

        if (flags & FLAG_R)
        {
            if (flags & FLAG_R2)
            {
                compareFunction = compareFileInfoAsc;
            }
            else
            {
                compareFunction = compareFileInfoDesc;
            }
        }
        else
        {
            if (flags & FLAG_R2)
            {
                compareFunction = compareFileInfoDesc;
            }
            else
            {
                compareFunction = compareFileInfoAsc;
            }
        }
        if (flags & FLAG_T)
        {
            if (compareFunction == compareFileInfoAsc)
            {
                compareFunction = compareFileInfoAscByTime;
            }
            else
                compareFunction = compareFileInfoDescByTime;
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
                printf("\n%s%s/%s%s:\n", BLUE, currentPath, fileInfo[i].filename, NONE);
                push(&stack, tPath);
                // printLs_R(tPath, flags);
            }
        // pop(&stack);
        free(fileInfo); // 释放动态分配的内存

        closedir(dir);
    }
}

void listFiles(const char *path, int flags)
{
    struct stat st;
    if (lstat(path, &st) == -1) // 使用 lstat 来获取链接信息
    {
        perror("Error getting file status");
        return;
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
        fileInfo.mTime = st.st_mtime;
        // 如果是链接，获取链接的目标路径
        if (S_ISLNK(st.st_mode))
        {
            ssize_t targetSize = readlink(path, fileInfo.targetPath, MAX_PATH - 1);
            if (targetSize != -1)
            {
                fileInfo.targetPath[targetSize] = '\0';
            }
            else
            {
                perror("Error reading symlink target");
            }
        }
        else
        {
            fileInfo.targetPath[0] = '\0';
        }

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
        printf(" %*luK", (int)maxSizeWidth, fileInfo->st.st_blocks);
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

    // 输出文件名称和链接目标路径
    const char *colorCode = (S_ISDIR(fileInfo->st.st_mode)) ? BLUE : (fileInfo->st.st_mode & S_IXUSR || fileInfo->st.st_mode & S_IXGRP || fileInfo->st.st_mode & S_IXOTH) ? GREEN
                                                                                                                                                                          : NONE;
    printf(" %s%s%s", colorCode, fileInfo->filename, NONE);

    // 如果是链接，输出链接目标路径
    if (S_ISLNK(fileInfo->st.st_mode))
    {
        printf(" -> %s", fileInfo->targetPath);
    }

    printf("\n");
}
