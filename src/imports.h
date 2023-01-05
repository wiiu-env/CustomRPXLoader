/* coreinit */
IMPORT_BEGIN(coreinit);

IMPORT(OSReport);
IMPORT(OSScreenInit);
IMPORT(OSScreenGetBufferSizeEx);
IMPORT(OSScreenSetBufferEx);
IMPORT(OSScreenEnableEx);
IMPORT(OSScreenFlipBuffersEx);
IMPORT(OSScreenClearBufferEx);
IMPORT(OSScreenPutFontEx);
IMPORT(OSFatal);
IMPORT(OSDynLoad_Acquire);
IMPORT(OSDynLoad_FindExport);
IMPORT(OSDynLoad_Release);
IMPORT(OSSetExceptionCallback);
IMPORT(OSSavesDone_ReadyToRelease);
IMPORT(OSInitMutex);
IMPORT(OSLockMutex);
IMPORT(OSUnlockMutex);
IMPORT(OSInitCond);
IMPORT(OSWaitCond);
IMPORT(OSSignalCond);
IMPORT(OSInitSpinLock);
IMPORT(OSUninterruptibleSpinLock_Acquire);
IMPORT(OSUninterruptibleSpinLock_Release);
IMPORT(OSFastMutex_Init);
IMPORT(OSFastMutex_Lock);
IMPORT(OSFastMutex_Unlock);
IMPORT(OSSleepTicks);
IMPORT(OSGetTitleID);
IMPORT(OSIsThreadTerminated);
IMPORT(OSSetThreadPriority);
IMPORT(OSCreateThread);
IMPORT(OSSetThreadCleanupCallback);
IMPORT(OSResumeThread);
IMPORT(OSIsThreadSuspended);
IMPORT(OSSuspendThread);
IMPORT(OSGetCurrentThread);
IMPORT(OSExitThread);
IMPORT(OSJoinThread);
IMPORT(OSYieldThread);
IMPORT(OSGetCoreId);
IMPORT(OSIsMainCore);
IMPORT(OSGetSystemTime);
IMPORT(OSGetSystemTick);
IMPORT(OSGetTime);
IMPORT(OSGetSymbolName);
IMPORT(OSGetSharedData);
IMPORT(OSEffectiveToPhysical);
IMPORT(OSInitSemaphore);
IMPORT(OSInitSemaphoreEx);
IMPORT(OSGetSemaphoreCount);
IMPORT(OSSignalSemaphore);
IMPORT(OSWaitSemaphore);
IMPORT(OSTryWaitSemaphore);
IMPORT(OSCompareAndSwapAtomicEx);
IMPORT(OSCompareAndSwapAtomic);
IMPORT(OSGetThreadSpecific);
IMPORT(OSSetThreadSpecific);
IMPORT(FSTimeToCalendarTime);
IMPORT(OSSwapAtomic);
IMPORT(OSDynLoad_IsModuleLoaded);

IMPORT(exit);
IMPORT(_Exit);
IMPORT(__os_snprintf);
IMPORT(DisassemblePPCRange);

IMPORT(ICInvalidateRange);
IMPORT(DCInvalidateRange);
IMPORT(DCFlushRange);
IMPORT(DCStoreRange);
IMPORT(DCStoreRangeNoSync);

IMPORT(__gh_errno_ptr);

IMPORT(MEMGetBaseHeapHandle);
IMPORT(MEMCreateExpHeapEx);
IMPORT(MEMDestroyExpHeap);
IMPORT(MEMAllocFromExpHeapEx);
IMPORT(MEMFreeToExpHeap);
IMPORT(MEMGetSizeForMBlockExpHeap);
IMPORT(MEMAllocFromFrmHeapEx);
IMPORT(MEMFreeToFrmHeap);
IMPORT(MEMGetAllocatableSizeForFrmHeapEx);

IMPORT(FSInit);
IMPORT(FSShutdown);
IMPORT(FSAddClient);
IMPORT(FSAddClientEx);
IMPORT(FSDelClient);
IMPORT(FSInitCmdBlock);
IMPORT(FSChangeDir);
IMPORT(FSGetFreeSpaceSize);
IMPORT(FSGetStat);
IMPORT(FSRemove);
IMPORT(FSOpenFile);
IMPORT(FSOpenFileEx);
IMPORT(FSCloseFile);
IMPORT(FSOpenDir);
IMPORT(FSMakeDir);
IMPORT(FSReadDir);
IMPORT(FSRewindDir);
IMPORT(FSCloseDir);
IMPORT(FSGetStatFile);
IMPORT(FSReadFile);
IMPORT(FSWriteFile);
IMPORT(FSSetPosFile);
IMPORT(FSFlushFile);
IMPORT(FSTruncateFile);
IMPORT(FSRename);
IMPORT(FSGetMountSource);
IMPORT(FSMount);
IMPORT(FSUnmount);
IMPORT(FSChangeMode);
IMPORT(FSGetPosFile);
IMPORT(OSTicksToCalendarTime);
IMPORT(__rplwrap_exit);

IMPORT(OSMemoryBarrier);

IMPORT(OSInitMutexEx);

IMPORT(FSAMakeDir);
IMPORT(FSAInit);
IMPORT(FSAAddClient);
IMPORT(FSARewindDir);
IMPORT(FSAMount);
IMPORT(FSAGetDeviceInfo);
IMPORT(FSARename);
IMPORT(FSAChangeDir);
IMPORT(FSAUnmount);
IMPORT(FSADelClient);
IMPORT(FSAChangeMode);
IMPORT(FSAReadDir);
IMPORT(FSAOpenDir);
IMPORT(FSACloseDir);
IMPORT(FSAFlushFile);
IMPORT(FSAOpenFileEx);
IMPORT(FSACloseFile);
IMPORT(FSAGetStatFile);
IMPORT(FSAGetFreeSpaceSize);
IMPORT(FSASetPosFile);
IMPORT(FSATruncateFile);
IMPORT(FSARemove);
IMPORT(FSAReadFile);
IMPORT(FSAWriteFile);
IMPORT(FSAGetStat);
IMPORT(FSAGetStatusStr);

IMPORT(IOS_Open);
IMPORT(IOS_Close);
IMPORT(IOS_Ioctl);
IMPORT(IOS_IoctlAsync);

IMPORT(IMIsAPDEnabled);
IMPORT(IMIsDimEnabled);
IMPORT(IMEnableAPD);
IMPORT(IMEnableDim);
IMPORT(IMDisableAPD);
IMPORT(IMDisableDim);

IMPORT(OSGetSystemInfo);

IMPORT_END();

/* nsysnet */
IMPORT_BEGIN(nsysnet);

IMPORT(socket_lib_init);
IMPORT(getaddrinfo);
IMPORT(freeaddrinfo);
IMPORT(getnameinfo);
IMPORT(inet_ntoa);
IMPORT(inet_ntop);
IMPORT(inet_aton);
IMPORT(inet_pton);
IMPORT(ntohl);
IMPORT(ntohs);
IMPORT(htonl);
IMPORT(htons);
IMPORT(accept);
IMPORT(bind);
IMPORT(socketclose);
IMPORT(connect);
IMPORT(getpeername);
IMPORT(getsockname);
IMPORT(getsockopt);
IMPORT(listen);
IMPORT(recv);
IMPORT(recvfrom);
IMPORT(send);
IMPORT(sendto);
IMPORT(setsockopt);
IMPORT(shutdown);
IMPORT(socket);
IMPORT(select);
IMPORT(socketlasterr);
IMPORT(socket_lib_finish);

IMPORT_END();

/* gx2 */


/* nn_ac */
IMPORT_BEGIN(nn_ac);
IMPORT(ACInitialize);
IMPORT(ACFinalize);
IMPORT(ACConnect);
IMPORT(ACClose);
IMPORT(ACGetAssignedAddress);
IMPORT(ACGetAssignedSubnet);
IMPORT(Initialize__Q2_2nn3actFv);
IMPORT(GetSlotNo__Q2_2nn3actFv);
IMPORT(GetDefaultAccount__Q2_2nn3actFv);
IMPORT(Finalize__Q2_2nn3actFv);
IMPORT_END();

/* proc_ui */
IMPORT_BEGIN(proc_ui);

IMPORT(ProcUIInit);
IMPORT(ProcUIShutdown);
IMPORT(ProcUIDrawDoneRelease);
IMPORT(ProcUIProcessMessages);

IMPORT_END();


/* sysapp */
IMPORT_BEGIN(sysapp);

IMPORT(SYSRelaunchTitle);
IMPORT(_SYSGetSystemApplicationTitleId);
IMPORT(SYSLaunchMenu);
IMPORT(_SYSLaunchMenuWithCheckingAccount);

IMPORT_END();

/* vpad */
IMPORT_BEGIN(vpad);

IMPORT(VPADRead);
IMPORT(VPADInit);
IMPORT(VPADGetTPCalibratedPoint);

IMPORT_END();


/* zlib125 */
IMPORT_BEGIN(zlib125);

IMPORT(deflateInit_);
IMPORT(deflate);
IMPORT(deflateEnd);
IMPORT(inflateInit_);
IMPORT(inflate);
IMPORT(inflateEnd);

IMPORT_END();
