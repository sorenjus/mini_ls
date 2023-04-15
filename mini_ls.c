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

// ./a.out -n /Users/justin/Documents

// Function to determine permissions
char *permissionString(struct stat fileStat);

// Main driver function to simulate ls
int main(int argc, char *argv[])
{
    // Set up structs
    DIR *dirPtr;
    struct dirent *entryPtr;
    struct stat statBuf;
    struct passwd *pwd;
    struct group *grp;
    struct tm *t;

    // Do some error checking
    if (argc < 2)
    {
        printf("Filename required\n");
        exit(1);
    }
    if ((dirPtr = opendir(argv[argc - 1])) == NULL)
    {
        perror("Error");
        exit(1);
    }
    if (stat(argv[argc - 1], &statBuf) < 0)
    {
        perror("huh? there is ");
        exit(1);
    }

    // Change current working directory
    chdir(argv[argc - 1]);

    // More error checking
    if (argc == 2 && !strstr(argv[1], "/"))
    {
        printf("Add a directory name\n");
        exit(1);
    }
    // When there are no modifiers
    else if (argc == 2)
    {
        while ((entryPtr = readdir(dirPtr)))
        {
            if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                continue;
            printf("%-10s ", entryPtr->d_name);
        }
        printf("\n");
    }
    // When all modifiers are included
    else if (argc >= 5)
    {
        if (((argv[1][1] != 'i') && (argv[1][1] != 'n') && (argv[1][1] != 's')) ||
            ((argv[2][1] != 'i') && (argv[2][1] != 'n') && (argv[2][1] != 's')) ||
            ((argv[3][1] != 'i') && (argv[3][1] != 'n') && (argv[3][1] != 's')))
        {
            printf("Invalid modifier\n");
            exit(1);
        }
        while ((entryPtr = readdir(dirPtr)))
        {

            if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                continue;

            stat(entryPtr->d_name, &statBuf);
            char *str = permissionString(statBuf);
            char time[26];

            t = localtime(&(statBuf.st_mtime));
            pwd = getpwuid(statBuf.st_uid);
            grp = getgrgid(statBuf.st_gid);
            printf("%-10llu%-2lld%-5s%-3d%-10s%-8s%-5lld", statBuf.st_ino, statBuf.st_blocks, str, statBuf.st_nlink, pwd->pw_name, grp->gr_name, statBuf.st_size);
            strftime(time, 26, "%b %d %H:%M  ", t);
            printf("%-5s%-5s\n", time, entryPtr->d_name);
            free(str);
        }
    }
    // When one modifier is included
    else if (argc == 3)
    {
        if ((argv[1][1] != 'i') && (argv[1][1] != 'n') && (argv[1][1] != 's'))
        {
            printf("Invalid modifier\n");
            exit(1);
        }
        char input = argv[1][1];
        switch (input)
        {
        case 'n':
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];

                t = localtime(&(statBuf.st_mtime));
                printf("%-3s", str);
                printf("%-3d", statBuf.st_nlink);
                printf("%-5u", statBuf.st_uid);
                printf("%-4u", statBuf.st_gid);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
            break;
        case 'i':
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];

                t = localtime(&(statBuf.st_mtime));
                printf("%-10llu", statBuf.st_ino);
                printf("%-3s", str);
                printf("%-2d", statBuf.st_nlink);
                pwd = getpwuid(statBuf.st_uid);
                printf("%-10s", pwd->pw_name);
                grp = getgrgid(statBuf.st_gid);
                printf("%-8s", grp->gr_name);
                printf("%-5lld", statBuf.st_size);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
            break;
        case 's':
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];

                t = localtime(&(statBuf.st_mtime));
                printf("%-4lld", statBuf.st_blocks);
                printf("%-3s", str);
                printf("%-2d", statBuf.st_nlink);
                pwd = getpwuid(statBuf.st_uid);
                printf("%-10s", pwd->pw_name);
                grp = getgrgid(statBuf.st_gid);
                printf("%-8s", grp->gr_name);
                printf("%-5lld", statBuf.st_size);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
            break;
        }
    }
    // When two modifiers are included
    else if (argc == 4)
    {
        if (((argv[1][1] != 'i') && (argv[1][1] != 'n') && (argv[1][1] != 's')) ||
            ((argv[2][1] != 'i') && (argv[2][1] != 'n') && (argv[2][1] != 's')))
        {
            printf("Invalid modifier\n");
            exit(1);
        }
        char command_1 = argv[1][1];
        char command_2 = argv[2][1];

        if ((command_1 == 'n' && command_2 == 'i') || (command_1 == 'i' && command_2 == 'n'))
        {
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];

                t = localtime(&(statBuf.st_mtime));
                printf("%-10llu", statBuf.st_ino);
                printf("%-3s", str);
                printf("%-2d", statBuf.st_nlink);
                printf("%-5u", statBuf.st_uid);
                printf("%-4u", statBuf.st_gid);
                printf("%-5lld", statBuf.st_size);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
        }
        else if ((command_1 == 'n' && command_2 == 's') || (command_1 == 's' && command_2 == 'n'))
        {
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];

                t = localtime(&(statBuf.st_mtime));
                printf("%-3lld", statBuf.st_blocks);
                printf("%-3s", str);
                printf("%-2d", statBuf.st_nlink);
                printf("%-5u", statBuf.st_uid);
                printf("%-4u", statBuf.st_gid);
                printf("%-5lld", statBuf.st_size);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
        }
        else
        {
            while ((entryPtr = readdir(dirPtr)))
            {
                if (strcmp(entryPtr->d_name, ".") == 0 || strcmp(entryPtr->d_name, "..") == 0)
                    continue;
                stat(entryPtr->d_name, &statBuf);
                char *str = permissionString(statBuf);
                char time[26];
                printf("got here");

                t = localtime(&(statBuf.st_mtime));
                pwd = getpwuid(statBuf.st_uid);
                grp = getgrgid(statBuf.st_gid);
                printf("%-10llu%-3lld%-3s%-3d%-10s%-8s%-5lld", statBuf.st_ino, statBuf.st_blocks, str, statBuf.st_nlink, pwd->pw_name, grp->gr_name, statBuf.st_size);
                strftime(time, 26, "%b %d %H:%M  ", t);
                printf("%-5s", time);
                printf("%-10s\n", entryPtr->d_name);
                free(str);
            }
        }
    }

    closedir(dirPtr);
    return 0;
}

// Function to return a string of user permissions
char *permissionString(struct stat fileStat)
{
    char *str = malloc(14 * sizeof(char));
    (S_ISDIR(fileStat.st_mode)) ? strncat(str, "d", 1) : strncat(str, "-", 1);
    // User Permissions
    (fileStat.st_mode & S_IRUSR) ? strncat(str, "r", 1) : strncat(str, " -", 1);
    (fileStat.st_mode & S_IWUSR) ? strncat(str, "w", 1) : strncat(str, "-", 1);
    (fileStat.st_mode & S_IXUSR) ? strncat(str, "x", 1) : strncat(str, "-", 1);

    // Group permissions
    (fileStat.st_mode & S_IRGRP) ? strncat(str, "r", 1) : strncat(str, "-", 1);
    (fileStat.st_mode & S_IWGRP) ? strncat(str, "w", 1) : strncat(str, "-", 1);
    (fileStat.st_mode & S_IXGRP) ? strncat(str, "x", 1) : strncat(str, "-", 1);

    // Other permissions
    (fileStat.st_mode & S_IROTH) ? strncat(str, "r", 1) : strncat(str, "-", 1);
    (fileStat.st_mode & S_IWOTH) ? strncat(str, "w", 1) : strncat(str, "-", 1);
    (fileStat.st_mode & S_IXOTH) ? strncat(str, "x   ", 4) : strncat(str, "-   ", 4);
    return str;
}