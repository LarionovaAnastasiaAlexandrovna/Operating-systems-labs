#include <boost/thread.hpp>
#include <iostream>

int min_element = INT_MAX, max_element = INT_MIN, average_element = 0, size;
int* mass;
boost::mutex mutex;

void min_max()
{
    for (int i = 0; i < size; ++i)
    {
        boost::lock_guard<boost::mutex> lock(mutex);
        if (min_element > mass[i])
        {
            min_element = mass[i];
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(7));
        if (max_element < mass[i])
        {
            max_element = mass[i];
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(7));
    }

    std::cout << "Thread min_max is finished." << std::endl;
    std::cout << "\nmin_element = " << min_element
              << "\nmax_element = " << max_element << std::endl;
}

void average()
{
    for (int i = 0; i < size; ++i)
    {
        boost::lock_guard<boost::mutex> lock(mutex);
        average_element += mass[i];
        boost::this_thread::sleep(boost::posix_time::milliseconds(12));
    }

    average_element /= size;
    std::cout << "Thread average is finished.\n" << std::endl;
    std::cout << "average_element = " << average_element << "\n";
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

    boost::thread thread1(min_max);
    boost::thread thread2(average);

    thread1.join();
    thread2.join();

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
