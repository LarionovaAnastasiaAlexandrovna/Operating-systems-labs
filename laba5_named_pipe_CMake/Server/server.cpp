#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include "employee.h"

CRITICAL_SECTION cs;

employee* arr;
bool* mod;
int numOfEntries;

DWORD WINAPI mes(LPVOID p)
{
HANDLE hPipe = (HANDLE)p;
employee* error = new employee;
while (true)
{
bool isRead = false;
char message[10];
DWORD readBytes;
isRead = ReadFile(hPipe, message, 10, &readBytes, NULL);
if (!isRead)
{
if (ERROR_BROKEN_PIPE == GetLastError())
{
std::cout << "Client disconnected." << std::endl;
return 0;
}
else
{
std::cout << "Error in reading." << std::endl;
return 0;
}
}
if (strlen(message) > 0)
{
char command = message[0];
message[0] = ' ';
int id = atoi(message);
DWORD bytesWritten;
EnterCriticalSection(&cs);
employee* toSend = new employee;
for (int i = 0; i < numOfEntries; i++)
{
if (id == arr[i].num)
{
toSend->hours = arr[i].hours;
toSend->num = arr[i].num;
for(int j = 0; j < 10; j++)
{
toSend->name[j] = arr[i].name[j];
}
break;
}
}
LeaveCriticalSection(&cs);
if (NULL == toSend)
{
toSend = error;
}
else
{
int ind = toSend - arr;
switch (command)
{
case '2':
std::cout << "ID for modifying: " << id << std::endl;
mod[ind] = true;
break;
case '1':
std::cout << "ID for reading: " << id << std::endl;
break;
default:
std::cout << "Error ID." << std::endl;
}
}
bool isSent = WriteFile(hPipe, toSend, sizeof(employee), &bytesWritten, NULL);
if (!isSent)
{
std::cout << "Error in sending answer." << std::endl;
}
if ('2' == command && toSend != error)
{
DWORD readBytes;
isRead = ReadFile(hPipe, message, numOfEntries, &readBytes, NULL);
if (!isRead)
{
if (ERROR_BROKEN_PIPE == GetLastError())
{
std::cout << "Disconnected." << std::endl;
return 0;
}
else
{
std::cout << "Error in reading." << std::endl;
return 0;
}
}
if (isRead)
{
mod[toSend - arr] = false;
EnterCriticalSection(&cs);
LeaveCriticalSection(&cs);
}
else
{
std::cout << "Error in reading." << std::endl;
}
break;
}
}
}
FlushFileBuffers(hPipe);
DisconnectNamedPipe(hPipe);
CloseHandle(hPipe);
delete error;
return 0;
}

int main()
{
    char fileName[66];
    std::cout << "Enter the file name:" << std::endl;
    std::cin >> fileName;
    std::cout << "Enter the number of entries: " << std::endl;
    std::cin >> numOfEntries;
    std::cout << "Enter the ID, name, and number of hours worked:" << std::endl;
    arr = new employee[numOfEntries];
    for (int i = 0; i < numOfEntries; i++)
    {
        std::cout << "Enter information about " << i + 1 << " worker:" << std::endl;
        std::cin >> arr[i].num >> arr[i].name >> arr[i].hours;
    }
    std::ifstream in;
    std::ofstream out;
    out.open(fileName, std::ofstream::binary);
    out.write((const char*)arr, sizeof(employee) * numOfEntries);
    out.close();
    std::cout << "Initial data:" << std::endl;
    for (int i = 0; i < numOfEntries; i++)
    {
        std::cout << arr[i].num << " " << arr[i].name << " " << arr[i].hours << std::endl;
    }
    std::cout << "Input number of processes: " << std::endl;
    int numOfProcesses;
    std::cin >> numOfProcesses;
    InitializeCriticalSection(&cs);
    HANDLE hStartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    mod = new bool[numOfEntries];
    for (int i = 0; i < numOfEntries; i++)
    {
        mod[i] = false;
    }
    HANDLE* hEvents = new HANDLE[numOfProcesses];
    for (int i = 0; i < numOfProcesses; i++)
    {
        std::string processName = "Client.exe";
        std::string eventName = "EVENT_";
        eventName = eventName + std::to_string(i + 1);
        processName = processName + " " + eventName;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        char* _eventName = new char(eventName.size()+1);
        _eventName = strcpy(_eventName, eventName.c_str());
        char* _processName = new char(processName.size() + 1);
        _processName = strcpy(_processName, processName.c_str());
        hEvents[i] = CreateEvent(NULL, TRUE, FALSE, _eventName);
        if (!CreateProcess(NULL, _processName, NULL, NULL, FALSE,
                           CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
        {
            GetLastError();
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    WaitForMultipleObjects(numOfProcesses, hEvents, TRUE, INFINITE);
    SetEvent(hStartALL);
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[numOfProcesses];
    for (int i = 0; i < numOfProcesses; i++)
    {
        hPipe = CreateNamedPipe("\\\\.\\pipe\\nPipe", PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe)
        {
            std::cout << "The named channel could not be created." << std::endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, mes, (LPVOID)hPipe, 0, NULL);
    }
    WaitForMultipleObjects(numOfProcesses, hThreads, TRUE, INFINITE);
    std::cout << "The file after making changes:" << std::endl;
    for (int i = 0; i < numOfEntries; i++)
    {
        std::cout << arr[i].num << " " << arr[i].name << " " << arr[i].hours << std::endl;
    }
    std::cout << "Press any key to complete: "<< std::endl;
    _getch();
    DeleteCriticalSection(&cs);
    delete[] hThreads;
    delete[] hEvents;
    delete[] mod;
    delete[] arr;
    return 0;
}