#include <iostream>

int main()
{
    // Set the foreground color to red
    std::cout << "\x1B[31mRed Text\x1B[0m" << std::endl;

    // Set the foreground color to green
    std::cout << "\x1B[32mGreen Text\x1B[0m" << std::endl;

    // Set the foreground color to yellow
    std::cout << "\x1B[33mYellow Text\x1B[0m" << std::endl;

    // Reset the color to default
    std::cout << "Back to default color" << std::endl;

    return 0;
}