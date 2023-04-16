#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#define GetCurrentDir getcwd

// ./a.out -n /Users/justin/Documents

// Function to determine permissions
char *permissionString(struct stat fileStat)
{
    char *str = malloc(14 * sizeof(char));
    if (S_ISDIR(fileStat.st_mode))
    {
        strncat(str, "d", 1);
    }
    else
    {
        strncat(str, "-", 1);
    }
    // User Permissions
    if (fileStat.st_mode & S_IRUSR)
        strncat(str, "r", 1);
    else
        strncat(str, " -", 1);
    if (fileStat.st_mode & S_IWUSR)
        strncat(str, "w", 1);
    else
        strncat(str, "-", 1);
    if (fileStat.st_mode & S_IXUSR)
        strncat(str, "x", 1);
    else
        strncat(str, "-", 1);

    // Group permissions
    if (fileStat.st_mode & S_IRGRP)
        strncat(str, "r", 1);
    else
        strncat(str, "-", 1);
    if (fileStat.st_mode & S_IWGRP)
        strncat(str, "w", 1);
    else
        strncat(str, "-", 1);
    if (fileStat.st_mode & S_IXGRP)
        strncat(str, "x", 1);
    else
        strncat(str, "-", 1);

    // Other permissions
    if (fileStat.st_mode & S_IROTH)
        strncat(str, "r", 1);
    else
        strncat(str, "-", 1);
    if (fileStat.st_mode & S_IWOTH)
        strncat(str, "w", 1);
    else
        strncat(str, "-", 1);
    if (fileStat.st_mode & S_IXOTH)
        strncat(str, "x   ", 4);
    else
        strncat(str, "-   ", 4);
    return str;
}

// Main driver function to simulate ls
int main(int argc, char *argv[])
{
    // Set up structs
    DIR *dirPtr = NULL;
    struct dirent *entryPtr;
    struct stat statBuf;
    struct passwd *pwd;
    struct group *grp;
    struct tm *t;
    char *target = ".";
    int iCommand, nCommand, sCommand;
    iCommand = 0;
    nCommand = 0;
    sCommand = 0;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-i"))
        {
            fflush(NULL);
            iCommand = 1;
        }
        else if (!strcmp(argv[i], "-n"))
        {
            fflush(NULL);
            nCommand = 1;
        }
        else if (!strcmp(argv[i], "-s"))
        {
            fflush(NULL);
            sCommand = 1;
        }
        else
        {
            target = argv[i];
        }
    }

    if (strcmp(&target[0], "/"))
    {
        char buff[FILENAME_MAX];
        GetCurrentDir(buff, FILENAME_MAX);
        chdir(buff);
        if ((dirPtr = opendir(target)) == NULL)
    {
        perror("Error");
        exit(1);
    }
    }
    else if (strcmp(target, ""))
    {
        chdir(target);
        if ((dirPtr = opendir(target)) == NULL)
    {
        perror("Error");
        exit(1);
    }
    }
    
    while ((entryPtr = readdir(dirPtr)))
    {
        if (!strcmp(&entryPtr->d_name[0], ".git") || strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
            continue;

        stat(entryPtr->d_name, &statBuf);
        if (iCommand)
        {
            printf("%-9llu", statBuf.st_ino);
        }
        if (sCommand == 1)
        {
            printf("%-4lld", statBuf.st_blocks);
        }
        if (nCommand)
        {
            char *str = permissionString(statBuf);
            char time[26];

            t = localtime(&(statBuf.st_mtime));
            printf("%-3s", str);
            pwd = getpwuid(statBuf.st_uid);
            grp = getgrgid(statBuf.st_gid);
            // printf("%-3d", statBuf.st_nlink);
            // printf("%-5u ", statBuf.st_uid);
            // printf("%-4u", statBuf.st_gid);
            printf("%-10s%-8s", pwd->pw_name, grp->gr_name);
            printf("%-5lld", statBuf.st_size);
            strftime(time, 26, " %b %d %H:%M  ", t);
            printf("%-5s", time);

            free(str);
        }

        printf("%-10s\n", entryPtr->d_name);
    }
    closedir(dirPtr);
    return 0;
}