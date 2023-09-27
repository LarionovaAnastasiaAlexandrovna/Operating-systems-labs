#include <iostream>
#include <fstream>
#include <algorithm>
#include "..\Creator\header.h"

#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char* argv[])
{
  /*char* creator = new char[12]; 
  strcpy(creator, "..\\Creator\\");*/
  std::ifstream bin_file(/*strcat(creator,*/ argv[1], std::ios::binary | std::ios::in);
  bin_file.seekg(0, bin_file.std::ios::end);
  int size = bin_file.tellg() / sizeof(employee);
  bin_file.seekg(bin_file.std::ios::beg);
  employee* E = new employee[size];
  bin_file.read(reinterpret_cast<char*>(E), sizeof(employee) * size);
  std::sort(E, E + size, [](employee& e1, employee& e2) -> bool {return e1.num < e2.num;});
  std::ofstream file(argv[2]);
  file << "Report on the file \"" << argv[1] << "\" :\n";
  for (int i = 0; i < size; ++i)
  {
    file << E[i].num << '\t' << E[i].name << '\t' << E[i].hours << '\t' << E[i].hours * atoi(argv[3]) << '\n';
  }
  
  return 0;
}
