#include "cuFunctionManager.h"

#include <cassert>

CuFunctionsManager* CuFunctionsManager::instance_ = nullptr;
CUcontext CuFunctionsManager::cu_context = NULL;

CuFunctionsManager& CuFunctionsManager::getInstance()
{
    return *instance_;
}

void CuFunctionsManager::create(const CUdevice device)
{
    if(!instance_) 
    {
        instance_ = new CuFunctionsManager;
    }
    if(!cu_context)
    {
        CUresult res = cuCtxCreate(&cu_context, CU_CTX_SCHED_AUTO, device);
        assert(res == CUDA_SUCCESS && "Can't create context");
    }
}

void CuFunctionsManager::destroy()
{
    if(cu_context)
    {
        CUresult res = cuCtxDestroy(cu_context);
        assert(res == CUDA_SUCCESS && "Can't destroy context");
    }
    if(instance_) 
    {
        delete instance_;
        instance_ = nullptr;
    }
}

void CuFunctionsManager::loadModule(const std::string& name)
{
    CUmodule cu_module;

    std::string filename = name + ".ptx";
    CUresult res = cuModuleLoad(&cu_module, filename.c_str());
    assert(res == CUDA_SUCCESS && "Can't load module");

    modules_[name] = cu_module;
}

CUfunction& CuFunctionsManager::getCuFunction(const std::string& module_name, const std::string& function_name)
{
    const std::string function_key = function_name + '@' + module_name;

    auto fun_itr = functions_.find(function_key);
    if(fun_itr == functions_.end())
    {
        auto mod_itr = modules_.find(module_name);
        assert(mod_itr != modules_.end() && "Can't find such a Module");

        CUfunction cu_function;
        CUresult res = cuModuleGetFunction(&cu_function, mod_itr->second, function_name.c_str());
        assert(res == CUDA_SUCCESS && "Can't get CUFunction");

        fun_itr = functions_.insert(std::make_pair(function_key, cu_function)).first;
    }

    return fun_itr->second;
}
