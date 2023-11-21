#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>

void CloseAllHandlesS(HANDLE& hMutex, HANDLE& hSemaphoreForReceiver,
    HANDLE& hSemaphoreForSender, HANDLE& hSemaphoreForConcoles, HANDLE hEvent)
{
    CloseHandle(hMutex);
    CloseHandle(hSemaphoreForReceiver);
    CloseHandle(hSemaphoreForSender);
    CloseHandle(hSemaphoreForConcoles);
    CloseHandle(hEvent);
}


int main(int argc, char* argv[])
{
    std::ofstream os;
    std::ifstream is;
    HANDLE hMutex, hSemaphoreForReceiver, hSemaphoreForSender, hSemaphoreForConcoles;

    HANDLE hEvent;

    hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"mutex"); 
    hSemaphoreForReceiver = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForReceiver");
    hSemaphoreForSender = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForSender");
    hSemaphoreForConcoles = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForConcoles.");
    hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, std::to_wstring(atoi(argv[2])).c_str());

    SetEvent(hEvent);

    int number, size_;
    char message[20];
    while (true) {
        std::cout << "enter '1' to send message or '2' to interrupt work: "; std::cin >> number;
        if (number == 1) {
            is.open(argv[1], std::ios::binary | std::ios::in);
            is.seekg(0, std::ios::end);
            size_ = is.tellg();
            is.close();
            if (size_ == atoi(argv[3]) * 20) {
                if (!ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL)) {
                    CloseAllHandlesS(hMutex, hSemaphoreForReceiver,
                        hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
                    ExitProcess(0);
                }
                WaitForSingleObject(hSemaphoreForConcoles, INFINITE);
                WaitForSingleObject(hSemaphoreForSender, INFINITE);
                ReleaseSemaphore(hSemaphoreForSender, 1, NULL);
                continue;
            }
            os.open(argv[1], std::ios::binary | std::ios::in | std::ios::app);
            std::cout << "enter message: "; std::cin >> message;
            os.write(reinterpret_cast<const char*>(&message), sizeof(char[20]));
            os.close();

            ReleaseSemaphore(hSemaphoreForReceiver, 1, NULL);
            WaitForSingleObject(hSemaphoreForSender, INFINITE);
        }
        else if (number == 2) {
            ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL);
            CloseAllHandlesS(hMutex, hSemaphoreForReceiver,
                hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
            ExitProcess(0);
        }
    }
}
