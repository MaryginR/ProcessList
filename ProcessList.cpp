#include "ProcessListHeader.h"

std::vector<std::string> GlobalProcessList;

static void GetAllProcessNames()
{
    // Получение всех процессов
    DWORD processes[1024]{}, cbNeeded = 0, cProcesses;
    EnumProcesses(processes, sizeof(processes), &cbNeeded);
    if (!K32EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        throw "failed to get processes pid's from K32EnumProcesses";
    }

    // Определение количества процессов
    cProcesses = cbNeeded / sizeof(DWORD);

    // Перебор всех процессов
    for (DWORD i = 0; i < cProcesses; i++)
    {
        if (processes[i] != 0)
        {
            // Открытие дескриптора процесса
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess)
            {
                bool inList = false;
                // Получение имени процесса
                char szProcessName[MAX_PATH];
                if (GetModuleBaseNameA(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(char)))
                {
                    for (int i = 0; i < GlobalProcessList.size(); i++)
                    {
                        if (szProcessName == GlobalProcessList[i])
                        {
                            inList = true;
                            break;
                        }
                    }

                    if (!inList)
                        GlobalProcessList.push_back(szProcessName);
                }
                CloseHandle(hProcess);
            }
        }
    }
}
