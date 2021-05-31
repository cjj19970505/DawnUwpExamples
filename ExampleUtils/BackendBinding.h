#pragma once
#include <webgpu/webgpu_cpp.h>
#include <dawn/webgpu.h>
class BackendBinding
{
public:
	BackendBinding(WGPUDevice backendDevice);
private:
	WGPUDevice mBackendDevice{ nullptr };
};

