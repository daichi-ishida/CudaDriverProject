#include "display.h"
#include "cuFunctionManager.h"

#include <cuda.h>

#include "helper_cuda_drvapi.h"

#include <cassert>
#include <iostream>


int main()
{
    CUdevice device = findCudaDeviceDRV(0, nullptr);

    Display display;
    display.createWindow();

    CuFunctionsManager::create(device);

    // --- Test ---

    CuFunctionsManager::getInstance().loadModule("kernel.ptx");
    CUfunction addFunc = CuFunctionsManager::getInstance().getCuFunction("kernel.ptx", "addKernel");

    constexpr size_t length = 5;
    CUdeviceptr arrayAd, arrayBd, arrayCd;
    cuMemAlloc(&arrayAd, sizeof(int) * length);
    cuMemAlloc(&arrayBd, sizeof(int) * length);
    cuMemAlloc(&arrayCd, sizeof(int) * length);

    constexpr int arrayAh[length] = {5, 6, 7, 8, 9};
    constexpr int arrayBh[length] = {0, 1, 2, 3, 4};
    int arrayCh[length] = {};

    cuMemcpyHtoD(arrayAd, arrayAh, sizeof(int) * length);
    cuMemcpyHtoD(arrayBd, arrayBh, sizeof(int) * length);

    void* kernelArgs[] = { &arrayCd, &arrayAd, &arrayBd };
    CUresult result = cuLaunchKernel(addFunc, 1, 1, 1, length, 1, 1, 0, NULL, kernelArgs, 0);
    assert(result == CUDA_SUCCESS && "Can't launch kernel");

    cuMemcpyDtoH(arrayCh, arrayCd, sizeof(int) * length);

    for (int i = 0; i < length; i++)
    {
        std::cout << arrayCh[i] << ",";
    }
    std::cout << std::endl;

    cuMemFree(arrayAd);
    cuMemFree(arrayBd);
    cuMemFree(arrayCd);

    // --- Test ---

    while(!display.isClosing())
    {
        // implement

        display.updateWindow();
    }

    CuFunctionsManager::destroy();

    display.close();

    return 0;
}

