#pragma once

#include "llbc.h"
using namespace llbc;

#ifndef EXAMPLE_EXPORT
 #define EXAMPLE_EXPORT __declspec(dllimport)
#endif

EXAMPLE_EXPORT int Add(int a, int b);

