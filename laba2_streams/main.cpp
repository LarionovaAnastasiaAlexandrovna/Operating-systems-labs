#include <windows.h>
#include <iostream>

int min_element = INT_MAX, max_element = INT_MIN, average_element = 0, size;
int* mass;


DWORD WINAPI min_max()
{
  for (int i = 0; i < size; ++i)
  {
    if (min_element > mass[i])
    {
      min_element = mass[i];
    }
    Sleep(7);
    if (max_element < mass[i])
    {
      max_element = mass[i];
    }    
    Sleep(7);
  }

  std::cout << "Thread min_max is finished." << std::endl;
  std::cout << "\nmin_element = " << min_element
    << "\nmax_element = " << max_element;
  return 0;
}

DWORD WINAPI average()
{
  for (int i = 0; i < size; ++i)
  {
    average_element += mass[i];
    Sleep(12);
  }
  
  average_element /= size;
  std::cout << "Thread average is finished.\n" << std::endl;
  std::cout << "average_element = " << average_element << "\n";
  return 0;
}


int main()
{
  std::cout << "Enter size and array: \n";
  std::cin >> size;
  mass = new int[size];
  for (int i = 0; i < size; ++i)
  {
    std::cin >> mass[i];
  }
  HANDLE hThread;
  DWORD IDThread;
  HANDLE hThread2;
  DWORD IDThread2;

  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, 0, 0, &IDThread); 
  if (hThread == NULL)
    return GetLastError();
  
  hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)average, 0, 0, &IDThread2); 
  if (hThread2 == NULL)
    return GetLastError();

  WaitForSingleObject(hThread2, INFINITE);
  CloseHandle(hThread2);

  WaitForSingleObject(hThread, INFINITE);
  CloseHandle(hThread);

  for (int i = 0; i < size; ++i)
  {
    if (mass[i] == min_element)
      mass[i] = average_element;
    if (mass[i] == max_element)
      mass[i] = average_element;
  }

  std::cout << "\n";
  for (int i = 0; i < size; ++i)
  {
    std::cout << mass[i] << " ";
  }

  delete[] mass;
  return 0;
}
