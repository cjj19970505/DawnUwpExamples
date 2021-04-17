#include "pch.h"
#include "BackendBinding.h"
#include <dawn_native/D3D12Backend.h>

BackendBinding::BackendBinding(WGPUDevice backendDevice):mBackendDevice(backendDevice)
{
	
}

wgpu::SwapChain BackendBinding::GetSwapChain(wgpu::Device& device, IUnknown* coreWindow)
{
	auto swapChainImpl = dawn_native::d3d12::CreateNativeSwapChainForCoreWindowImpl(mBackendDevice, coreWindow);
	wgpu::SwapChainDescriptor swapChainDesc{};
	swapChainDesc.implementation = reinterpret_cast<uint64_t>(&swapChainImpl);
	auto swapChain = device.CreateSwapChain(nullptr, &swapChainDesc);
	return std::move(swapChain);
}