#pragma once
#include <stdexcept>
#include <windows.h>
#include <stdio.h>

class ComInitializer {
public:
    void Initialize()
    {
        LRESULT hr;
        // Initialize COM
        hr = CoInitialize(NULL);
        if (FAILED(hr)) {
            printf("CoInitialize error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    ~ComInitializer()
    {
        CoUninitialize();
    }
};
