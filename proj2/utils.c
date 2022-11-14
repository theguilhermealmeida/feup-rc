#include <string.h>
#include <stdio.h>

#include "utils.h"

// ftp://[<user>:<password>@]<host>/<url-path>
int parseURL(url *url, const char *argument)
{
    

    int user = 0;
    int pass = 0;

    if (strstr(argument, "ftp://") == NULL)
    {
        printf("%s\n", "Invalid protocol, use 'ftp://'");
        return -1;
    }

    if (strchr(argument, '@') != NULL) // user mode
    {
        user = 1;
    }

    int url_index = 0;
    int i = 6;
    int state = 1;
    if (user)
        state = 0;
    int length = strlen(argument);

    while (i < length)
    {
        switch (state)
        {
        case 0: // username and password
            if (argument[i] == ':')
            {
                pass = 1;
                url_index = 0;
            }
            else if (argument[i] == '@')
            {
                state = 1;
                url_index = 0;
            }
            else
            {
                if (pass)
                {
                    url->password[url_index] = argument[i];
                    url_index++;
                }
                else
                {
                    url->user[url_index] = argument[i];
                    url_index++;
                }
            }
            break;

        case 1: // host
            if (argument[i] == '/')
            {
                state = 2;
                url_index = 0;
            }
            else
            {
                url->host[url_index] = argument[i];
                url_index++;
            }
            break;

        case 2: // path
            url->path[url_index] = argument[i];
            url_index++;
            break;
        }
        i++;
    }

    // retrieving name of file 
    char * ptr = strrchr(argument, '/');
    ptr++; // ignoring '/'
    strcpy(url->filename, ptr);




    return 0;
}
