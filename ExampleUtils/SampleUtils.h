#pragma once
#include <webgpu/webgpu_cpp.h>

wgpu::Device CreateCppDawnDevice();
wgpu::SwapChain GetSwapChain(wgpu::Device& device, winrt::Windows::UI::Core::CoreWindow const& coreWindow);
wgpu::TextureView CreateDefaultDepthStencilView(const wgpu::Device& device);