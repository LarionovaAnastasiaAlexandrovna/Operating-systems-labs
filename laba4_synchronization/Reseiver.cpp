#include <iostream>
#include <windows.h>
#include <fstream>
#include <conio.h>
#include <string>

void CloseAllHandlesR(HANDLE& hMutex, HANDLE& hSemaphoreForReceiver,
    HANDLE& hSemaphoreForSender, HANDLE& hSemaphoreForConcoles, HANDLE* hEvent)
{
    CloseHandle(hMutex);
    CloseHandle(hSemaphoreForReceiver);
    CloseHandle(hSemaphoreForSender);
    CloseHandle(hSemaphoreForConcoles);
    CloseHandle(hEvent);
}

int main() {
    char bin[100];  
    int size, n;
    std::cout << "Enter name of binary file: "; 
    std::cin >> bin;
    std::cout << "\nEnter number of notes: ";
    std::cin >> size;
    std::cout << "\nEnter quantity of sender processes: ";
    std::cin >> n;
    std::ifstream fin;
    std::ofstream fout(bin, std::ios::binary | std::ios::out | std::ios::trunc);
    fout.close();
    HANDLE hMutex, hSemaphoreForReceiver, hSemaphoreForSender, hSemaphoreForConcoles;
    HANDLE* hEvent = new HANDLE[n];

    STARTUPINFO* si = new STARTUPINFO[n];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[n];

    hMutex = CreateMutex(NULL, FALSE, L"mutex"); 
    hSemaphoreForReceiver = CreateSemaphore(NULL, 0, size, L"hSemaphoreForReceiver");
    hSemaphoreForSender = CreateSemaphore(NULL, size, size, L"hSemaphoreForSender");
    hSemaphoreForConcoles = CreateSemaphore(NULL, 0, n, L"hSemaphoreForConcoles");

    char for_senders[100];
    for (int i = 0; i < n; i++) {
        wsprintfA(for_senders, "Sender.exe %s %d %d", bin, i, size);
        hEvent[i] = CreateEvent(NULL, TRUE, FALSE, std::to_wstring(i).c_str());
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
        CreateProcessA(nullptr, for_senders, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, (LPSTARTUPINFOA)&si[i], &pi[i]);
    }
    WaitForMultipleObjects(n, hEvent, TRUE, INFINITE);

    int number, size_;
    char message[20];
    char* str;
    while (true) {
        std::cout << "enter '1' to read message or '2' to interrupt work: "; std::cin >> number;
        if (number == 1) {
            fin.open(bin, std::ios::binary | std::ios::in);
            fin.seekg(0, std::ios::end);
            size_ = fin.tellg();
            if (!size_) {
                fin.close();
                if (!ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL)) {
                    CloseAllHandlesR(hMutex, hSemaphoreForReceiver,
                        hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
                    CloseHandle((*pi).hThread);
                    CloseHandle((*pi).hProcess);
                    return 0;
                }
                WaitForSingleObject(hSemaphoreForConcoles, INFINITE);
                WaitForSingleObject(hSemaphoreForReceiver, INFINITE);
                ReleaseSemaphore(hSemaphoreForReceiver, 1, NULL);
                continue;
            }
            fin.seekg(0, std::ios::beg);
            fin.read(reinterpret_cast<char*>(&message), sizeof(char[20]));
            std::cout << message << "\n";

            str = new char[size_ - 20];
            fin.read(str, size_ - 20);
            fin.close();
            fout.open(bin, std::ios::binary | std::ios::out);
            fout.write(str, size_ - 20);
            fout.close();
            delete str;

            WaitForSingleObject(hSemaphoreForReceiver, INFINITE);
            ReleaseSemaphore(hSemaphoreForSender, 1, NULL);
        }
        else if (number == 2) {
            ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL);
            CloseAllHandlesR(hMutex, hSemaphoreForReceiver,
                hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
            CloseHandle((*pi).hThread);
            CloseHandle((*pi).hProcess);
            return 0;
        }
    }
}
