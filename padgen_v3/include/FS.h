#ifndef FS_H
#define FS_H

#include "3dstypes.h"
u32  FileOpen(wchar_t *fname);
void FileRead(u32 file_handle, void* in_buf, u32 size);
void FileWrite(u32 file_handle, void* write_buf, u32 size);
void FileWriteOffset(u32 file_handle, void* write_buf, u32 size, u32 offset);

#endif
