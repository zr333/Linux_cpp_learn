#include <iostream>

int add(int a , int b)
{
    return a + b;
}

int main()
{
    int sum[5] {0, 0, 0, 0, 0};
    int arr1[5] {1, 2, 3, 4, 5};
    int arr2[5] {1, 1, 1, 1, 1};

    for(int i = 0; i < 5; ++i)
    {
        sum[i] = add(arr1[i], arr2[i]);
        std::cout << sum[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}