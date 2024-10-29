#include "ProcessListHeader.h"

std::unordered_set<std::string> GlobalProcessList;

static void GetAllProcessNames()
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