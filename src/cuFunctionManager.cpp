#include "cuFunctionManager.h"

#include <cassert>
#include <fstream>

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
    CUjit_option options[6];
    void *optionVals[6];
    char error_log[8192], info_log[8192];
    unsigned int logSize = 8192;
    void *cuOut;
    size_t outSize;
    std::string ptx_source;

    // Setup linker options
    // Pass a buffer for info messages
    options[0] = CU_JIT_INFO_LOG_BUFFER;
    optionVals[0] = (void *)info_log;
    // Pass the size of the info buffer
    options[1] = CU_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
    optionVals[1] = (void *)(long)logSize;
    // Pass a buffer for error message
    options[2] = CU_JIT_ERROR_LOG_BUFFER;
    optionVals[2] = (void *)error_log;
    // Pass the size of the error buffer
    options[3] = CU_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
    optionVals[3] = (void *)(long)logSize;
    // Make the linker verbose
    options[4] = CU_JIT_LOG_VERBOSE;
    optionVals[4] = (void *)1;

    CUlinkState lState;

    // Create a pending linker invocation
    cuLinkCreate(5, options, optionVals, &lState);

    // load ptx
    std::string filename = name + ".ptx";
    std::ifstream ifs(filename, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    int file_size = ifs.tellg();
    ifs.seekg(0);

    char *buf = new char[file_size + 1];
    ifs.read(buf, file_size);

    buf[file_size] = '\0';
    ptx_source = buf;
    delete[] buf;

    // Load the PTX from the ptx file
    printf("Loading %s kernel program\n", filename.c_str());
    CUresult res = cuLinkAddData(lState, CU_JIT_INPUT_PTX, (void *)ptx_source.c_str(),
                            strlen(ptx_source.c_str()) + 1, 0, 0, 0, 0);

    if (res != CUDA_SUCCESS) {
        // Errors will be put in error_log, per CU_JIT_ERROR_LOG_BUFFER option
        // above.
        fprintf(stderr, "PTX Linker Error:\n%s\n", error_log);
    }

    // Complete the linker step
    cuLinkComplete(lState, &cuOut, &outSize);

    // info_log were requested in options above.
    printf("%s\n", info_log);

    CUmodule cu_module;
    res = cuModuleLoadData(&cu_module, cuOut);
    assert(res == CUDA_SUCCESS && "Can't load module");
    
    // Destroy the linker invocation
    cuLinkDestroy(lState);

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
