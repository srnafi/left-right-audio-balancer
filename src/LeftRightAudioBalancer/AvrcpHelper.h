#pragma once
#include <windows.h>

class AvrcpHelper {
public:
    static bool IsApplied();
    static bool Apply(bool enable);
    static int WriteRegistry(bool enable);
};
