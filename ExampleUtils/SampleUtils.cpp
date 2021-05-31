#include "pch.h"
#include "SampleUtils.h"
#include <dawn_native/DawnNative.h>
#include <dawn/dawn_proc.h>

static std::unique_ptr<dawn_native::Instance> instance;
static wgpu::Surface surface;
wgpu::Device CreateCppDawnDevice()
{
	instance = std::make_unique<dawn_native::Instance>();
    //instance->SetBackendValidationLevel(dawn_native::BackendValidationLevel::Partial);
	instance->DiscoverDefaultAdapters();
    dawn_native::Adapter backendAdapter;
    {
        std::vector<dawn_native::Adapter> adapters = instance->GetAdapters();
        auto adapterIt = std::find_if(adapters.begin(), adapters.end(),
            [](const dawn_native::Adapter adapter) -> bool {
                wgpu::AdapterProperties properties;
                adapter.GetProperties(&properties);
                return properties.backendType == wgpu::BackendType::D3D12;
            });
        assert(adapterIt != adapters.end());
        backendAdapter = *adapterIt;
    }
    WGPUDevice backendDevice = backendAdapter.CreateDevice();
    DawnProcTable backendProcs = dawn_native::GetProcs();
    dawnProcSetProcs(&backendProcs);
    return wgpu::Device::Acquire(backendDevice);
}

wgpu::SwapChain GetSwapChain(wgpu::Device device, winrt::Windows::UI::Core::CoreWindow const& coreWindow)
{
    auto desc = std::make_unique<wgpu::SurfaceDescriptorFromWindowsCoreWindow>();
    desc->coreWindow = coreWindow.as<IUnknown>().get();
    std::unique_ptr<wgpu::ChainedStruct> chainedDescriptor = std::move(desc);
    wgpu::SurfaceDescriptor descriptor;
    descriptor.nextInChain = chainedDescriptor.get();
    wgpu::Instance wgpuInstance = instance->Get();
    surface = wgpuInstance.CreateSurface(&descriptor);
    wgpu::SwapChainDescriptor swapChainDesc{};
    swapChainDesc.format = wgpu::TextureFormat::BGRA8Unorm;
    swapChainDesc.usage = wgpu::TextureUsage::RenderAttachment;
    swapChainDesc.width = 640;
    swapChainDesc.height = 480;
    swapChainDesc.presentMode = wgpu::PresentMode::Fifo;
    auto swapChain = device.CreateSwapChain(surface, &swapChainDesc);
    return std::move(swapChain);

}

wgpu::TextureView CreateDefaultDepthStencilView(const wgpu::Device& device) {
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = 640;
    descriptor.size.height = 480;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto depthStencilTexture = device.CreateTexture(&descriptor);
    return depthStencilTexture.CreateView();
}
