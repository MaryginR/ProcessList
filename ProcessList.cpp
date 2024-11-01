#include "ProcessListHeader.h"

std::unordered_set<std::string> GlobalProcessList;

static void GetAllProcessNamesByHandle()
{
    DWORD processes[1024]{}, cbNeeded = 0, cProcesses;

    // Получение всех процессов
    if (!K32EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        throw "failed to get processes pid's from K32EnumProcesses";
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    // Перебор всех процессов
    for (DWORD i = 0; i < cProcesses; i++)
    {
        if (processes[i] != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess)
            {
                char szProcessName[MAX_PATH];
                if (K32GetModuleBaseNameA(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(char)))
                {
                    GlobalProcessList.insert(szProcessName);
                }
                CloseHandle(hProcess);
            }
        }
    }
}

static void GetAllProcessNamesByToolhelp32Snapshot()
{
    // Создаем снимок всех процессов в системе
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        throw "Failed to create snapshot of processes";
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Проходим по списку процессов
    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
#ifdef UNICODE
            std::wstring wstr(pe32.szExeFile);
            std::string str(wstr.begin(), wstr.end());
            GlobalProcessList.insert(str);
#else
            GlobalProcessList.insert(pe32.szExeFile);
#endif
        } while (Process32Next(hSnapshot, &pe32));
    }
    else
    {
        // Ошибка при получении первого процесса
        CloseHandle(hSnapshot);
        throw "Failed to iterate through processes";
    }

    CloseHandle(hSnapshot);
}