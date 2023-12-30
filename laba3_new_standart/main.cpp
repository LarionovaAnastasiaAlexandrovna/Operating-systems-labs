#include <iostream>
#include <Windows.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
int n;
std::vector<int> mass;
int count_of_marker;
std::mutex criticalSection;

struct countThread
{
    std::vector<int>* mass;
    int count;
    HANDLE stop_process = CreateEvent(NULL, TRUE, FALSE, NULL);
    std::vector<std::shared_ptr<std::condition_variable>> event;
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
        std::unique_lock<std::mutex> lock(criticalSection);
        if (massL.mass->at(temp) == 0)
        {
            Sleep(5);
            massL.mass->at(temp) = massL.count;
            Sleep(5);
            count += 1;
        }
        else
        {
            std::cout << massL.count << " " << count << " " << temp << "\n";
            lock.unlock();
            SetEvent(massL.stop_process);
            WaitForSingleObject(massL.event[1]->native_handle(), INFINITE);
            flag = true;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (massL.mass->at(i) == massL.count)
        {
            massL.mass->at(i) = 0;
        }
    }
    return 0;
}

int main()
{
    std::vector<std::thread> hThread;
    std::vector<DWORD> dwThread;
    std::vector<countThread> massL;
    std::vector<std::shared_ptr<std::condition_variable>> stop;
    std::cout << "Enter size:\nn = ";
    std::cin >> n;
    mass.resize(n, 0);
    std::cout << "Enter count of markets:\n";
    std::cin >> count_of_marker;
    stop.resize(count_of_marker);
    for (int i = 0; i < count_of_marker; i++)
    {
        countThread threadData;
        threadData.mass = &mass;
        threadData.count = i + 1;
        stop[i] = threadData.stop_process;
        threadData.event.push_back(std::make_shared<std::condition_variable>());
        threadData.event.push_back(std::make_shared<std::condition_variable>());
        hThread.emplace_back(label, static_cast<LPVOID>(&threadData));
        massL.push_back(threadData);
    }
    SetEvent(event);
    bool exit = true;
    while (exit)
    {
        exit = false;
        int index;
        for (auto& s : stop)
        {
            s->wait(lock, [s]() { return WaitForSingleObject(s->native_handle(), 0) == WAIT_OBJECT_0; });
        }
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
        else if (massL[index].event[0]->wait_for(lock, std::chrono::milliseconds(0)) == std::cv_status::no_timeout)
        {
            std::cout << "This thread has already been completed.\n";
        }
        else
        {
            massL[index].event[0]->notify_one();
            hThread[index].join();
            for (int i = 0; i < n; i++)
            {
                std::cout << mass[i] << " ";
            }
            std::cout << "\n";
        }

        for (size_t i = 0; i < count_of_marker; i++)
        {
            if (massL[i].event[0]->wait_for(lock, std::chrono::milliseconds(0)) == std::cv_status::timeout)
            {
                massL[i].event[1]->notify_one();
                exit = true;
            }
        }
    }
    for (size_t i = 0; i < count_of_marker; i++)
    {
        CloseHandle(stop[i]);
    }
    return 0;
}
