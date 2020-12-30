#define EXAMPLE_EXPORT __declspec(dllexport)

#include "Example.h"

int Add(int a, int b)
{
    return a + b;
}