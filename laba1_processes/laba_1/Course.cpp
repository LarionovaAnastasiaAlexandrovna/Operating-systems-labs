#include <iostream>
#include <fstream>
#include <Windows.h>
#include <conio.h>

int main()
{
  char name_bin_file[25];
  std::cout << "Enter the name of binary file and size: \n";
  std::cin >> name_bin_file;

  int size;
  std::cin >> size;

  char console[61];
  wsprintfA(console, "..\\Release\\Creator.exe %s %d", name_bin_file, size);

  STARTUPINFO si;
  PROCESS_INFORMATION piApp;

  ZeroMemory(&si, sizeof(STARTUPINFO));

  CreateProcessA(nullptr, (LPSTR)console, nullptr, nullptr, FALSE,
    CREATE_NEW_CONSOLE, nullptr, nullptr, (LPSTARTUPINFOA)&si, &piApp);
  WaitForSingleObject(piApp.hProcess, INFINITE);

  CloseHandle(piApp.hThread);
  CloseHandle(piApp.hProcess);

  std::ifstream bin_file(name_bin_file, std::ios::binary | std::ios::in);
  while (!bin_file.eof())
  {
    std::cout << bin_file.get();
  }

  char name_report[25];
  int cost;
  std::cout << "\nEnter the name of report and cost: \n";
  std::cin >> name_report >> cost;

  char console2[86];
  wsprintfA(console2, "..\\Release\\Reporter.exe %s %s %d", name_bin_file, name_report, cost);

  ZeroMemory(&si, sizeof(STARTUPINFO));

  CreateProcessA(nullptr, (LPSTR)console2, nullptr, nullptr, FALSE,
    CREATE_NEW_CONSOLE, nullptr, nullptr, (LPSTARTUPINFOA)&si, &piApp);
  WaitForSingleObject(piApp.hProcess, INFINITE);

  CloseHandle(piApp.hThread);
  CloseHandle(piApp.hProcess);

  std::ifstream file(name_report);
  char line[47];
  while (!file.eof())
  {
    file.getline(line, 47);
    std::cout << line << '\n';
  }

  return 0;
}
