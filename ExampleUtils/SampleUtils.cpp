#include "pch.h"
#include "SampleUtils.h"
#include <dawn_native/DawnNative.h>
#include <dawn/dawn_proc.h>
#include "BackendBinding.h"

static std::unique_ptr<dawn_native::Instance> instance;
static BackendBinding* binding = nullptr;

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
    binding = new BackendBinding(backendDevice);
    dawnProcSetProcs(&backendProcs);
    return wgpu::Device::Acquire(backendDevice);
}

wgpu::SwapChain GetSwapChain(wgpu::Device& device, winrt::Windows::UI::Core::CoreWindow const& coreWindow)
{
    return binding->GetSwapChain(device, coreWindow.as<IUnknown>().get());

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
