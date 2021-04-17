#pragma once
#include <webgpu/webgpu_cpp.h>
#include <dawn/webgpu.h>
class BackendBinding
{
public:
	BackendBinding(WGPUDevice backendDevice);
	wgpu::SwapChain GetSwapChain(wgpu::Device& device, IUnknown* coreWindow);
	WGPUTextureFormat GetPreferredSwapChainTextureFormat(wgpu::SwapChain swapChain);
private:
	WGPUDevice mBackendDevice{ nullptr };
};

