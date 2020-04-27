#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <coreinit/filesystem.h>
#include "utils/logger.h"
#include "FSOSUtils.h"

#define FS_MOUNT_SOURCE_SIZE            0x300
#define FS_MAX_MOUNTPATH_SIZE           128
#define FS_SOURCETYPE_EXTERNAL          0

int32_t FSOSUtils::MountFS(FSClient *pClient, FSCmdBlock *pCmd, char **mount_path) {
    int32_t result = -1;
    DEBUG_FUNCTION_LINE("\n");
    FSMountSource *mountSrc = (FSMountSource*) malloc(sizeof(FSMountSource));
    if(!mountSrc) {
        return -3;
    }

    DEBUG_FUNCTION_LINE("\n");
    char* mountPath = (char*) malloc(FS_MAX_MOUNTPATH_SIZE);
    if(!mountPath) {
        free(mountSrc);
        mountSrc = NULL;
        return -4;
    }
    DEBUG_FUNCTION_LINE("\n");
    memset(mountSrc, 0, FS_MOUNT_SOURCE_SIZE);
    memset(mountPath, 0, FS_MAX_MOUNTPATH_SIZE);


    // Mount sdcard
    if (FSGetMountSource(pClient, pCmd, FS_MOUNT_SOURCE_SD, mountSrc, -1) == 0) {
        DEBUG_FUNCTION_LINE("\n");
        result = FSMount(pClient, pCmd, mountSrc, mountPath, FS_MAX_MOUNTPATH_SIZE, -1);
        if((result == 0) && mount_path) {
            *mount_path = (char*)malloc(strlen(mountPath) + 1);
            if(*mount_path)
                strcpy(*mount_path, mountPath);
        }
    }

    DEBUG_FUNCTION_LINE("\n");
    free(mountPath);
    free(mountSrc);

    mountPath = NULL;
    mountSrc = NULL;

    return result;
}

int32_t FSOSUtils::UmountFS(FSClient *pClient, FSCmdBlock *pCmd, const char *mountPath) {
    int32_t result = -1;
    result = FSUnmount(pClient, pCmd, mountPath, -1);

    return result;
}
