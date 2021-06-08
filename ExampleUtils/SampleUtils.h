#pragma once
#include <webgpu/webgpu_cpp.h>

wgpu::Device CreateCppDawnDevice();
wgpu::SwapChain GetSwapChain(wgpu::Device device, winrt::Windows::UI::Core::CoreWindow const& coreWindow);
wgpu::SwapChain GetSwapChain(wgpu::Device device, winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& swapChainPanel);
wgpu::TextureView CreateDefaultDepthStencilView(const wgpu::Device& device);
//wgpu::SwapChain CreateSurfaceForCoreWindow(wgpu::Instance instance, winrt::Windows::UI::Core::CoreWindow const& coreWindow);