#include "internals.h"

NtQueryInformationProcessPtr NtQueryInformationProcess;

bool InitializeInternalAPI()
{
    HMODULE module = GetModuleHandle("ntdll.dll");
    if (module == NULL)
        return false;

    NtQueryInformationProcess = (NtQueryInformationProcessPtr) GetProcAddress(module, "NtQueryInformationProcess");
    if (NtQueryInformationProcess == NULL)
        return false;

    return true;
}

DWORD GetParentProcessId()
{
    unsigned long process_basic_information[6];
    unsigned long bytes_returned;

    if (FAILED(NtQueryInformationProcess(GetCurrentProcess(), 0, &process_basic_information, sizeof (process_basic_information), &bytes_returned)))
        return (DWORD) -1;

    if (bytes_returned == sizeof (process_basic_information))
        return (DWORD) process_basic_information[5];

    return (DWORD) -1;
}

const char *GetExecutablePathFromPid(DWORD pid)
{
    static char path[MAX_PATH + 1];
    unsigned long int path_size = MAX_PATH;

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process == NULL)
        return NULL;

    bool error = QueryFullProcessImageName(process, 0, path, &path_size) == 0;
    CloseHandle(process);
    if (error)
        return NULL;

    return path;
}
