#include <fstream>
#include <iostream>
#include "header.h"

int main(int argc, char* argv[])
{
  std::ofstream file(argv[1], std::ios::binary | std::ios::out);
  employee* E = new employee[atoi(argv[2])];
  std::cout << "Enter num, name and hours, please\n";
  for (int i = 0; i < atoi(argv[2]); ++i)
  {
    std::cin >> E[i].num >> E[i].name >> E[i].hours;
  }
  file.write(reinterpret_cast<const char*>(E), sizeof(employee) * atoi(argv[2]));
  file.close();
  return 0;
}
