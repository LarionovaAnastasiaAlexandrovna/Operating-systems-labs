#include <iostream>
#include <Windows.h> 
HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
int n;
HANDLE* hThread;
int count_of_marker;
CRITICAL_SECTION criticalSection;

struct countThread
{
    int* mass;
    int count;
    HANDLE stop_process = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE* event = new HANDLE[2];
};

DWORD WINAPI label(LPVOID massL_)
{
    WaitForSingleObject(event, INFINITE);
    countThread massL = *(static_cast<countThread*>(massL_));
    srand(massL.count);
    bool flag = false;
    int count = 0;
    while (!flag)
    {
        int temp = rand();
        temp = temp % n;
        EnterCriticalSection(&criticalSection);
        if (massL.mass[temp] == 0)
        {
            Sleep(5);
            massL.mass[temp] = massL.count;
            Sleep(5);
            count += 1;
            LeaveCriticalSection(&criticalSection);
        }
        else
        {
            std::cout << massL.count << " " << count << " " << temp << "\n";
            LeaveCriticalSection(&criticalSection);
            SetEvent(massL.stop_process);
            int k = WaitForMultipleObjects(2, massL.event, FALSE, INFINITE) - WAIT_OBJECT_0;
            if (k == 0)
            {
                flag = true;
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (massL.mass[i] == massL.count)
        {
            massL.mass[i] = 0;
        }
    }
    return 0;
}


int main()
{
    InitializeCriticalSection(&criticalSection);
    int* mass;
    DWORD* dwThread;
    HANDLE* hThread;
    std::cout << "Enter size:\nn = ";
    std::cin >> n;
    mass = new int[n];
    for (int i = 0; i < n; i++)
    {
        mass[i] = 0;
    }
    std::cout << "Enter count of markets:\n";
    std::cin >> count_of_marker;
    hThread = new HANDLE[count_of_marker];
    dwThread = new DWORD[count_of_marker];
    countThread* massL = new countThread[count_of_marker];
    bool* check = new bool[count_of_marker];
    HANDLE* stop = new HANDLE[count_of_marker];
    for (int i = 0; i < count_of_marker; i++)
    {
        massL[i].mass = mass;
        massL[i].count = i + 1;
        stop[i] = massL[i].stop_process;
        massL[i].event[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        massL[i].event[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hThread[i] = CreateThread(NULL, 0, label, static_cast<LPVOID>(&massL[i]), 0, &dwThread[i - 1]);
        if (hThread[i] == NULL) return GetLastError();
        check[i] = false;
    }
    SetEvent(event);
    bool exit = true;
    while (exit)
    {
        exit = false;
        int index;
        WaitForMultipleObjects(count_of_marker, stop, TRUE, INFINITE);
        for (int i = 0; i < n; i++)
        {
            std::cout << mass[i] << " ";
        }
        std::cout << "\nEnter the number of the thread to be completed: ";
        std::cin >> index;
        --index;
        if (index >= count_of_marker || index < 0)
        {
            std::cout << "Error input. In next time, enter a number between 1 and " << count_of_marker << "\n";
        }
        else if (check[index])
        {
            std::cout << "This thread has already been conpleted.\n";
        }
        else
        {
            SetEvent(massL[index].event[0]);
            WaitForSingleObject(hThread[index], INFINITE);
            for (int i = 0; i < n; i++)
            {
                std::cout << mass[i] << " ";
            }
            std::cout << "\n";
            check[index] = true;
        }

        for (int i = 0; i < count_of_marker; i++)
        {
            if (!check[i])
            {
                ResetEvent(massL[i].stop_process);
                SetEvent(massL[i].event[1]);
                exit = true;
            }
        }
    }
    for (int i = 0; i < count_of_marker; i++)
    {
        CloseHandle(hThread[i]);
        CloseHandle(stop[i]);
        CloseHandle(massL[i].event[0]);
        CloseHandle(massL[i].event[1]);
    }
    DeleteCriticalSection(&criticalSection);
    return 0;
}
