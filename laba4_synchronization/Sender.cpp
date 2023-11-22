#include <fstream>
#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>

void CloseAllHandlesS( HANDLE& hSemaphoreForReceiver,
    HANDLE& hSemaphoreForSender, HANDLE& hSemaphoreForConcoles, HANDLE& hEvent)
{
    CloseHandle(hSemaphoreForReceiver);
    CloseHandle(hSemaphoreForSender);
    CloseHandle(hSemaphoreForConcoles);
    CloseHandle(hEvent);
}


int main(int argc, char* argv[])
{
    std::ofstream os;
    std::ifstream is;
    HANDLE hSemaphoreForReceiver, hSemaphoreForSender, hSemaphoreForConcoles, hEvent;

    hSemaphoreForReceiver = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForReceiver");
    hSemaphoreForSender = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForSender");
    hSemaphoreForConcoles = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"hSemaphoreForConcoles"); //
    hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, std::to_wstring(atoi(argv[2])).c_str());

    SetEvent(hEvent);

    int size_;
    char choice;
    char message[20];
    while (true) {
        std::cout << "enter 'S' on the console to send the message, or enter 'T' to stop working: ";
        std::cin >> choice;
        if (choice == 's' || choice == 'S')
        {
            is.open(argv[1], std::ios::binary | std::ios::in);
            is.seekg(0, std::ios::end);
            size_ = is.tellg();
            is.close();
            if (size_ == atoi(argv[3]) * 20)
            {
                if (!ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL))
                {
                    CloseAllHandlesS(hSemaphoreForReceiver,
                        hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
                    ExitProcess(0);
                }
                WaitForSingleObject(hSemaphoreForConcoles, INFINITE);
                WaitForSingleObject(hSemaphoreForSender, INFINITE);
                ReleaseSemaphore(hSemaphoreForSender, 1, NULL);
                continue;
            }
            os.open(argv[1], std::ios::binary | std::ios::in | std::ios::app);
            std::cout << "enter message: ";
            std::cin >> message;
            os.write(reinterpret_cast<const char*>(&message), sizeof(char[20]));
            os.close();

            ReleaseSemaphore(hSemaphoreForReceiver, 1, NULL);
            WaitForSingleObject(hSemaphoreForSender, INFINITE);
        }
        else if (choice == 't' || choice == 'T')
        {
            ReleaseSemaphore(hSemaphoreForConcoles, 1, NULL);
            CloseAllHandlesS( hSemaphoreForReceiver,
                hSemaphoreForSender, hSemaphoreForConcoles, hEvent);
            ExitProcess(0);
        }
    }
}
