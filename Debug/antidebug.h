#pragma once
#include <Windows.h>
#include <iostream>
#include <debugapi.h>
#pragma section(".CRT$XLY", long, read)

#if 0 //hide in tls - commented out for now, need to figure out insertion with no manual input
__declspec(thread) int var = 0xDEADBEEF;
VOID NTAPI TlsCallback(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    var = 0xB15BADB0; // This is required for TLS Callback call
    if (IsDebuggerPresent())
    {
        MessageBoxA(NULL, "Debugging detected, exiting", "Error", MB_OK | MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 0xBABEFACE);
    }
}
__declspec(allocate(".CRT$XLY"))PIMAGE_TLS_CALLBACK g_tlsCallback = TlsCallback;
#endif

//example of few our anti debug exported methods, to call them simple call these functions
// if DB = True, it says debugger detected before exiting

// this consist of all functions that might be needed but should be limited only to this interface
namespace {

    typedef NTSTATUS(NTAPI* pfnNtSetInformationThread)(
        _In_ HANDLE ThreadHandle,
        _In_ ULONG  ThreadInformationClass,
        _In_ PVOID  ThreadInformation,
        _In_ ULONG  ThreadInformationLength
        );
    const ULONG ThreadHideFromDebugger = 0x11;

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

    typedef NTSTATUS(NTAPI* pfnNtQueryInformationProcess)(
        _In_      HANDLE           ProcessHandle,
        _In_      UINT             ProcessInformationClass,
        _Out_     PVOID            ProcessInformation,
        _In_      ULONG            ProcessInformationLength,
        _Out_opt_ PULONG           ReturnLength
        );
    const UINT ProcessDebugPort = 7;
    // working under assumption that windows 8 or higher PCs are being used
    WORD GetVersionWord()
    {
        return TRUE;
    }
    BOOL IsWin8OrHigher() { return GetVersionWord(); }
    BOOL IsVistaOrHigher() { return GetVersionWord(); }

    PVOID GetPEB()
    {
#ifdef _WIN64
        return (PVOID)__readgsqword(0x0C * sizeof(PVOID));
#else

        PVOID pPeb = 0;
#ifndef _WIN64
        // 1. There are two copies of PEB - PEB64 and PEB32 in WOW64 process
        // 2. PEB64 follows after PEB32
        // 3. This is true for version less then Windows 8, else __readfsdword returns address of real PEB64
        if (IsWin8OrHigher())
        {
            BOOL isWow64 = FALSE;
            typedef BOOL(WINAPI* pfnIsWow64Process)(HANDLE hProcess, PBOOL isWow64);
            pfnIsWow64Process fnIsWow64Process = (pfnIsWow64Process)
                GetProcAddress(GetModuleHandleA("Kernel32.dll"), "IsWow64Process");
            if (fnIsWow64Process(GetCurrentProcess(), &isWow64))
            {
                if (isWow64)
                {
                    pPeb = (PVOID)__readfsdword(0x0C * sizeof(PVOID));
                    pPeb = (PVOID)((PBYTE)pPeb + 0x1000);
                }
            }
        }
        printf("%d", pPeb);
        return pPeb;
#else   
        return (PVOID)__readfsdword(0x0C * sizeof(PVOID));
#endif		
#endif
    }
}

void antidebugpresent(bool DB)
{
    if (IsDebuggerPresent()) {
        if (DB) {
            MessageBoxA(NULL, "Debugging detected, exiting", "antidebugpresent", MB_OK | MB_ICONERROR);
        }
        exit(-1);
    }
}

void antidebugPEB(bool DB)
{
    if (GetPEB()) {
        if (DB) {
            MessageBoxA(NULL, "Debugging detected, exiting", "antidebugPEB", MB_OK | MB_ICONERROR);
        }
        exit(-1);
    }
}

void antidebugpresent_tlshidden(bool DB)
{
    //left for now
}

void CheckNtGlobalFlag(bool DB)
{
    PVOID pPeb = GetPEB();
    PVOID pPeb64 = GetPEB();
    DWORD offsetNtGlobalFlag = 0;
#ifdef _WIN64
    offsetNtGlobalFlag = 0xBC;
#else
    offsetNtGlobalFlag = 0x68;
#endif
    DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
    if (NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
    {
        if (DB) {
            MessageBoxA(NULL, "Debugging detected, exiting", "CheckNtGlobalFlag", MB_OK | MB_ICONERROR);
        }
        exit(-1);
    }
    if (pPeb64)
    {
        DWORD NtGlobalFlagWow64 = *(PDWORD)((PBYTE)pPeb64 + 0xBC);
        if (NtGlobalFlagWow64 & NT_GLOBAL_FLAG_DEBUGGED)
        {
            if (DB) {
                MessageBoxA(NULL, "Debugging detected, exiting", "CheckNtGlobalFlag", MB_OK | MB_ICONERROR);
            }
            exit(-1);
        }
    }
}

#ifndef _WIN64
void trapflagcheck(bool DB)
{
    BOOL isDebugged = TRUE;
    __try
    {
        __asm
        {
            pushfd
            or dword ptr[esp], 0x100 // set the Trap Flag 
            popfd                    // Load the value into EFLAGS register
            nop
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // If an exception has been raised – debugger is not present
        isDebugged = FALSE;
    }
    if (isDebugged)
    {
        MessageBoxA(NULL, "Debugging detected, exiting", "trapflagcheck", MB_OK | MB_ICONERROR);
        //exit(-1);
    }
}
#endif // !_WIN64



void CheckRemoteDebuggerPresent(bool DB) {
    BOOL isDebuggerPresent = FALSE;
    if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent))
    {
        if (isDebuggerPresent)
        {
            MessageBoxA(NULL, "Debugging detected, exiting", "CheckRemoteDebuggerPresent", MB_OK | MB_ICONERROR);
            //exit(-1);
        }
    }
}


void CheckNtQueryInformationProcess(bool DB)
{
    pfnNtQueryInformationProcess NtQueryInformationProcess = NULL;
    NTSTATUS status;
    DWORD isDebuggerPresent = 0;
    HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));

    if (NULL != hNtDll)
    {
        NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
        if (NULL != NtQueryInformationProcess)
        {
            status = NtQueryInformationProcess(
                GetCurrentProcess(),
                ProcessDebugPort,
                &isDebuggerPresent,
                sizeof(DWORD),
                NULL);
            if (status == 0x00000000 && isDebuggerPresent != 0)
            {
                MessageBoxA(NULL, "Debugging detected, exiting", "CheckNtQueryInformationProcess", MB_OK | MB_ICONERROR);
                //exit(-1);
            }
        }
    }
}