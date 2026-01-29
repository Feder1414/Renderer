// A simple program that computes the square root of a number

// TODO3: Include <format>

#include <format>
#include <iostream>
#include <string>

#include <MathFunctions.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        // TODO4: Convert the print to use std::format
        std::string formattedString = std::format("Usage: {} number", argv[0]);
        std::cout << formattedString << std::endl;

        return 1;
    }

    // convert input to double
    double const inputValue = std::stod(argv[1]);

    // calculate square root
    double const outputValue = mathfunctions::sqrt(inputValue);
    // TODO5: Convert the print to use std::format
    std::string formattedSquareRoot = std::format("The square root of {} is {}", inputValue, outputValue);
    std::cout << formattedSquareRoot
        << std::endl;
}
