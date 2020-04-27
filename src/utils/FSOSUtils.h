#ifndef __FS_OS_UTILS_H_
#define __FS_OS_UTILS_H_

#include <stdint.h>
#include <coreinit/filesystem.h>

class FSOSUtils {
public:
    static int32_t MountFS(FSClient *pClient, FSCmdBlock *pCmd, char **mount_path);
    static int32_t UmountFS(FSClient *pClient, FSCmdBlock *pCmd, const char *mountPath);
};

#endif // __FS_OS_UTILS_H_
