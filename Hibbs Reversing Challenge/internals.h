#include <windows.h>

typedef unsigned long int PROCESSINFOCLASS;
typedef NTSTATUS(WINAPI *NtQueryInformationProcessPtr)(HANDLE process, PROCESSINFOCLASS information_class, PVOID information, ULONG information_length, PULONG bytes_returned);
extern NtQueryInformationProcessPtr NtQueryInformationProcess;

bool InitializeInternalAPI();
DWORD GetParentProcessId();
const char *GetExecutablePathFromPid(DWORD pid);
