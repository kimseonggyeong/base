/**
 * File Utils
 *
 * Author: KIM SEONG GYEONG < seonggyeong.kim@medithinq.com >
 */
#include "FileUtil.h"
#include "Log.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

namespace FileUtil
{

static int filecopy( const char  *psrc, const char  *pdest)
{
    int   ret = -1;
    FILE* fsrc = NULL;
    FILE* fdest = NULL;
    char  buf[1024];
    size_t size;
    struct stat fileInfo;

    if (!strcmp(psrc, pdest))
    {
        ret = -1;
        goto EXIT;
    }

    if (!(fsrc = fopen(psrc, "rb")))
    {
        ret = -2;
        goto EXIT;
    }

    if (!(fdest = fopen(pdest, "wb")))
    {
        ret = -3;
        goto EXIT;
    }

    while ((size = fread( buf, 1, sizeof(buf), fsrc)) > 0)
    {
        if (fwrite(buf, 1, size, fdest) == 0)
        {
            ret = -4;
            goto EXIT;
        }
    }
    stat(psrc, &fileInfo);
    chmod(pdest, fileInfo.st_mode);
    ret = 0;
EXIT:
    if (fsrc)
        fclose(fsrc);

    if (fdest)
        fclose(fdest);

    if (ret == -4)
        unlink(pdest);

    return ret;
}

int copy(const char* src, const char* dest)
{
    int  ret = -1;
	DIR* dir = NULL;
    struct dirent* file = NULL;
    struct stat buf;
    char filedest[1024];
    char filesrc[1024];

    stat(src, &buf);
    if (S_ISREG(buf.st_mode))
    {
        const char* point = strrchr(dest, '/');
        const char* srcpoint = strrchr(src, '/');
        int len = point - dest;
        if (len != 0)
        {
            if (strcmp(srcpoint, point) != 0)
            {
                strcpy(filedest, dest);
                mkdir(filedest, 0755);
                sprintf(filedest, "%s%s", dest, srcpoint);
            }
            else
            {
                strncpy(filedest, dest, len);
                filedest[len] = '\0';
                mkdir(filedest, 0755);
            }
        }

        ret = filecopy(src, filedest);
		return ret;
    }

    if ((dir = opendir(src)) == NULL)
    {
        LOGE("can't not open dir\n");
        return ret;
    }

    ::mkdir(dest, 0755);
    while((file = readdir(dir)) != NULL)
    {
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            continue;

        sprintf(filedest, "%s/%s", dest, file->d_name);
        sprintf(filesrc, "%s/%s", src, file->d_name);

        if((ret = stat(filesrc, &buf)) == -1)
        {
            LOGE("stat error : %d, %s\n", errno, strerror(errno));
            return ret;
        }
        if(S_ISDIR(buf.st_mode))
        {
            mkdir(filedest, 0755);
            ret = copy(filesrc, filedest);
            if (ret < 0)
            {
                return ret;
            }
        }
        else if(S_ISREG(buf.st_mode))
        {
            if ((ret = filecopy(filesrc, filedest)))
            {
                LOGE("copy error! %s\n", file->d_name);
                return ret;
            }
        }
    }
    closedir(dir);
    return 0;
}

int mkdir(const char* path, mode_t mode)
{
    char buf[4*1024];
    const char* point = path;
    int len = 0;
    int ret = -1;

    if(path == NULL || strlen(path) >= sizeof(buf))
        return ret;

    while((point = strchr(point, '/')) != NULL)
    {
        len = point - path;
        point++;

        if (len == 0)
            continue;

        strncpy(buf, path, len);
        buf[len] = '\0';

        if ((ret = ::mkdir(buf, mode)) == -1)
        {
            if (errno != EEXIST)
            {
                LOGE("mkdir error : %d, %s\n", errno, strerror(errno));
                return ret;
            }
        }
    }

    return ::mkdir(path, mode);
}

int rm(const char* path)
{
    int            ret  = -1;
    DIR*           dir  = NULL;
    struct dirent* file = NULL;
    struct stat    buf; 
    char           filedest[1024];

    stat(path, &buf);
    if (S_ISREG(buf.st_mode))
    {   
        ret = unlink(path);
        return ret;
    }
    
    if ((dir = opendir(path)) == NULL)
    {   
        ret = 0;
        return ret;
    }
    
    while((file = readdir(dir)) != NULL)
    {
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            continue;

        sprintf(filedest, "%s/%s", path, file->d_name);

        if((ret = stat(filedest, &buf)) == -1)
        {   
            LOGE("stat error : %d, %s\n", errno, strerror(errno));
            return ret;
        }

        if(S_ISDIR(buf.st_mode))
        {   
            ret = rm(filedest);
            if (ret)
            {
                LOGE("rmdir error : %d, %s\n", errno, strerror(errno));
                return ret;
			}
        }
        else if(S_ISREG(buf.st_mode))
        {    
            if ((ret = unlink(filedest)))
            {
                LOGE("unlink error! %s\n", file->d_name);
                return ret;
            }
        }
    }

    closedir(dir);
    return rmdir(path);
}

} // namespace FileUtil
