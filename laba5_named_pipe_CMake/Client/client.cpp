#include <iostream>
#include <windows.h>
#include <string>
#include "..\Server\employee.h"

int main(int argc, char* argv[]) {
    HANDLE EvReady = OpenEventA(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE EvStart = OpenEventA(SYNCHRONIZE, FALSE, "START_ALL");
    SetEvent(EvReady);
    WaitForSingleObject(EvStart, INFINITE);
    HANDLE hPipe;
    while (true)
    {
        hPipe = CreateFile("\\\\.\\pipe\\nPipe", GENERIC_WRITE | GENERIC_READ,
                           FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE)
        {
            break;
        }
    }
    while (true)
    {
        std::string choice;
        std::cout << "Input 'r' if you want to read, "
                     "'w' if you want to write ID or "
                     "'e' if you want to exit" << std::endl;
        std::cin >> choice;
        if (choice == "e")
        {
            break;
        }
        DWORD writeByte;
        bool isSend;
        isSend = WriteFile(hPipe, choice.c_str(), 10, &writeByte, NULL);
        if (!isSend)
        {
            std::cout << "The message cannot be sent" << std::endl;
            return 0;
        }
        employee employer;
        if (employer.num < 0)
        {
            std::cout << "The employee was not found" << std::endl;
            continue;
        }
        else
        {
            std::cout << employer.num << " " << employer.name << " " << employer.hours << std::endl;
            if (choice == "w")
            {
                std::cout << "Input ID, name and hours" << std::endl;
                std::cin >> employer.num >> employer.name >> employer.hours;
                isSend = WriteFile(hPipe, &employer, sizeof(employer), &writeByte, NULL);
                if (!isSend)
                {
                    std::cout << "Error sending." << std::endl;
                }
            }
        }
    }
    return 0;
}
