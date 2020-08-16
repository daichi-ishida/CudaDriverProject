#pragma once
#include <string>
#include <unordered_map>

#include <cuda.h>

// Singleton

class CuFunctionsManager
{
public:
    CuFunctionsManager(const CuFunctionsManager&) = delete;
    CuFunctionsManager& operator=(const CuFunctionsManager&) = delete;
    CuFunctionsManager(CuFunctionsManager&&) = delete;
    CuFunctionsManager& operator=(CuFunctionsManager&&) = delete;

    static CuFunctionsManager& getInstance();

    static void create(const CUdevice device);
    static void destroy();

    void loadModule(const std::string& name);

    CUfunction& getCuFunction(const std::string& module_name, const std::string& function_name);
    static CUcontext cu_context;

private:
    CuFunctionsManager() = default;
    ~CuFunctionsManager() = default;

    static CuFunctionsManager* instance_;

    std::unordered_map<std::string, CUmodule> modules_;
    std::unordered_map<std::string, CUfunction> functions_;
};