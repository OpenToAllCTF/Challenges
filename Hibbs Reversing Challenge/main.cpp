#include <windows.h>
#include "internals.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#if defined(UNICODE)
    #error Please disable UNICODE from the project settings
#endif

const unsigned char IdaDebugServer32[] = { 0xDD, 0xC3, 0xC4, 0x99, 0x98, 0xF5, 0xD8, 0xCF, 0xC7, 0xC5, 0xDE, 0xCF, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char OllyDbg[] = { 0xC5, 0xC6, 0xC6, 0xD3, 0xCE, 0xC8, 0xCD, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char Idag[] = { 0xC3, 0xCE, 0xCB, 0xCD, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char Idaq[] = { 0xC3, 0xCE, 0xCB, 0xDB, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char Idag64[] = { 0xC3, 0xCE, 0xCB, 0xCD, 0x9C, 0x9E, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char Idaq64[] = { 0xC3, 0xCE, 0xCB, 0xDB, 0x9C, 0x9E, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };
const unsigned char ImmunityDebugger[] = { 0xE3, 0xC7, 0xC7, 0xDF, 0xC4, 0xC3, 0xDE, 0xD3, 0xEE, 0xCF, 0xC8, 0xDF, 0xCD, 0xCD, 0xCF, 0xD8, 0x84, 0xCF, 0xD2, 0xCF, 0x00 };

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR command_line, INT cmd_show);
const char *DecodeString(const unsigned char *encoded_string);
extern "C" void __stdcall Challenge();
extern "C" int __stdcall CustomIsDebuggerPresent();
void DebuggerMain();

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR command_line, INT cmd_show)
{
    bool attach_to_parent = false;
    bool start_debugger = false;

    if (!InitializeInternalAPI())
        return 1;

    DWORD parent_process_id = GetParentProcessId();
    if (parent_process_id == (DWORD) -1)
        return 1;

    const char *parent_process_path = GetExecutablePathFromPid(parent_process_id);
    if (parent_process_path == NULL)
        return 1;

    if (StrStrI(parent_process_path, DecodeString(IdaDebugServer32)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(OllyDbg)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(Idag)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(Idaq)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(Idag64)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(Idaq64)) != NULL)
        attach_to_parent = true;
    else if (StrStrI(parent_process_path, DecodeString(ImmunityDebugger)) != NULL)
        attach_to_parent = true;

    if (attach_to_parent)
    {
        if (DebugActiveProcess(parent_process_id) == FALSE)
            return 1;

        DEBUG_EVENT debug_event;
        if (!WaitForDebugEvent(&debug_event, INFINITE))
            return 1;

        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, debug_event.dwThreadId);
        if (thread == NULL)
            return 1;

        CONTEXT thread_context;
        thread_context.ContextFlags = CONTEXT_ALL;

        if (!GetThreadContext(thread, &thread_context))
            return 1;

        thread_context.Eip = 0;
        if (!SetThreadContext(thread, &thread_context))
            return 1;

        CloseHandle(thread);

        if (!DebugActiveProcessStop(parent_process_id))
            return 1;

        while (true)
            Sleep(1);
    }

    if (CustomIsDebuggerPresent())
        Challenge();
    else
        DebuggerMain();

    ExitProcess(1);
}

const char *DecodeString(const unsigned char *encoded_string)
{
    static char buffer[MAX_PATH + 1];

    size_t string_length = strlen((const char *) encoded_string);
    for (size_t i = 0; i < string_length; i++)
    {
        buffer[i] = (encoded_string[i] ^ 0xAA);
        buffer[i + 1] = 0;
    }

    return (const char *) buffer;
}

void DebuggerMain()
{
    char *executable_name = "Hibbs.exe";

    STARTUPINFO startup_info;
    startup_info.cb = sizeof (STARTUPINFO);
    GetStartupInfo(&startup_info);

    PROCESS_INFORMATION process_info;
    if (!CreateProcess(NULL, executable_name, NULL, NULL, FALSE, DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &startup_info, &process_info))
        return;

    DEBUG_EVENT debug_event;
    while (true)
    {
        if (!WaitForDebugEvent(&debug_event, INFINITE))
            return;

        if (debug_event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
            return;

        if (debug_event.dwDebugEventCode != EXCEPTION_DEBUG_EVENT)
        {
            if (!ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE))
                return;

            continue;
        }

        if (debug_event.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT)
        {
            if (!ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_EXCEPTION_NOT_HANDLED))
                return;

            continue;
        }

        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, debug_event.dwThreadId);
        if (thread == NULL)
            return;

        CONTEXT thread_context;
        thread_context.ContextFlags = CONTEXT_ALL;
        if (!GetThreadContext(thread, &thread_context))
            return;

        if (thread_context.Ecx != 0xDEADC0DE)
        {
            CloseHandle(thread);

            if (!ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_EXCEPTION_NOT_HANDLED))
                return;

            continue;
        }

        thread_context.Edx += 1;
        switch (thread_context.Eax)
        {
            case 0x12:
            {
                thread_context.Eax = 'T';
                break;
            }
            case 0x71:
            {
                 thread_context.Eax = '0';
                 break;
            }

            case 0x13:
            {
                thread_context.Eax = 'p';
                break;
            }

            case 0x14:
            {
                thread_context.Eax = '!';
                break;
            }

            case 0x62:
            {
                thread_context.Eax = 'n';
                break;
            }

            case 0x55:
            {
                thread_context.Eax = 'T';
                break;
            }

            case 0x61:
            {
                thread_context.Eax = '0';
                break;
            }

            case 0x49:
            {
                thread_context.Eax = 'L';
                break;
            }

            case 0x11:
            {
                thread_context.Eax = 'c';
                break;
            }

            case 0x24:
            {
                thread_context.Eax = 'f';
                break;
            }

            case 0x73:
            {
                thread_context.Eax = '3';
                break;
            }

            case 0x57:
            {
                thread_context.Eax = '4';
                break;
            }

            case 0x27:
            {
                thread_context.Eax = '\0';
                break;
            }

            default:
                return;
        }

        if (!SetThreadContext(thread, &thread_context))
            return;

        CloseHandle(thread);

        if (!ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE))
            return;
    }
}
