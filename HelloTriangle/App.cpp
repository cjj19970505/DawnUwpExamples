#include "pch.h"
#include <webgpu/webgpu_cpp.h>
#include <utils/WGPUHelpers.h>
#include "SampleUtils.h"
#include <utils/ComboRenderPipelineDescriptor.h>
using namespace winrt;

using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;

struct Example
{
    wgpu::Device device;

    wgpu::Buffer indexBuffer;
    wgpu::Buffer vertexBuffer;

    wgpu::Texture texture;
    wgpu::Sampler sampler;

    wgpu::Queue queue;
    wgpu::SwapChain swapchain;
    wgpu::TextureView depthStencilView;
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroup bindGroup;

    CoreWindow mCoreWindow{ nullptr };

    Example(CoreWindow coreWindow) :mCoreWindow(coreWindow) {}
    void initBuffers() {
        static const uint32_t indexData[3] = {
            0,
            1,
            2,
        };
        indexBuffer =
            utils::CreateBufferFromData(device, indexData, sizeof(indexData), wgpu::BufferUsage::Index);

        static const float vertexData[12] = {
            0.0f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1.0f,
        };
        vertexBuffer = utils::CreateBufferFromData(device, vertexData, sizeof(vertexData),
            wgpu::BufferUsage::Vertex);
    }

    void initTextures() {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size.width = 1024;
        descriptor.size.height = 1024;
        descriptor.size.depthOrArrayLayers = 1;
        descriptor.sampleCount = 1;
        descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
        descriptor.mipLevelCount = 1;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::Sampled;
        texture = device.CreateTexture(&descriptor);

        sampler = device.CreateSampler();

        // Initialize the texture with arbitrary data until we can load images
        std::vector<uint8_t> data(4 * 1024 * 1024, 0);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>(i % 253);
        }

        wgpu::Buffer stagingBuffer = utils::CreateBufferFromData(
            device, data.data(), static_cast<uint32_t>(data.size()), wgpu::BufferUsage::CopySrc);
        wgpu::ImageCopyBuffer imageCopyBuffer =
            utils::CreateImageCopyBuffer(stagingBuffer, 0, 4 * 1024);
        wgpu::ImageCopyTexture imageCopyTexture = utils::CreateImageCopyTexture(texture, 0, { 0, 0, 0 });
        wgpu::Extent3D copySize = { 1024, 1024, 1 };

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

        wgpu::CommandBuffer copy = encoder.Finish();
        queue.Submit(1, &copy);
    }


    void init() {
        device = CreateCppDawnDevice();

        queue = device.GetQueue();
        swapchain = GetSwapChain(device, mCoreWindow);
        /*swapchain.Configure(wgpu::TextureFormat::RGBA8Unorm, wgpu::TextureUsage::RenderAttachment,
            640, 480);*/

        initBuffers();
        initTextures();

        wgpu::ShaderModule vsModule = utils::CreateShaderModule(device, R"(
        [[stage(vertex)]] fn main([[location(0)]] pos : vec4<f32>)
                               -> [[builtin(position)]] vec4<f32> {
            return pos;
        })");

        wgpu::ShaderModule fsModule = utils::CreateShaderModule(device, R"(
        [[group(0), binding(0)]] var mySampler: sampler;
        [[group(0), binding(1)]] var myTexture : texture_2d<f32>;

        [[stage(fragment)]] fn main([[builtin(frag_coord)]] FragCoord : vec4<f32>)
                                 -> [[location(0)]] vec4<f32> {
            return textureSample(myTexture, mySampler, FragCoord.xy / vec2<f32>(640.0, 480.0));
        })");

        auto bgl = utils::MakeBindGroupLayout(
            device, {
                        {0, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Filtering},
                        {1, wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float},
            });

        wgpu::PipelineLayout pl = utils::MakeBasicPipelineLayout(device, &bgl);

        depthStencilView = CreateDefaultDepthStencilView(device);

        utils::ComboRenderPipelineDescriptor2 descriptor;
        descriptor.layout = utils::MakeBasicPipelineLayout(device, &bgl);
        descriptor.vertex.module = vsModule;
        descriptor.vertex.bufferCount = 1;
        descriptor.cBuffers[0].arrayStride = 4 * sizeof(float);
        descriptor.cBuffers[0].attributeCount = 1;
        descriptor.cAttributes[0].format = wgpu::VertexFormat::Float32x4;
        descriptor.cFragment.module = fsModule;
        descriptor.cTargets[0].format = wgpu::TextureFormat::BGRA8Unorm;
        descriptor.EnableDepthStencil(wgpu::TextureFormat::Depth24PlusStencil8);

        pipeline = device.CreateRenderPipeline2(&descriptor);

        wgpu::TextureView view = texture.CreateView();

        bindGroup = utils::MakeBindGroup(device, bgl, { {0, sampler}, {1, view} });
    }

    struct {
        uint32_t a;
        float b;
    } s;

    void frame() {
        s.a = (s.a + 1) % 256;
        s.b += 0.02f;
        if (s.b >= 1.0f) {
            s.b = 0.0f;
        }

        wgpu::TextureView backbufferView = swapchain.GetCurrentTextureView();
        utils::ComboRenderPassDescriptor renderPass({ backbufferView }, depthStencilView);

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        {
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
            pass.SetPipeline(pipeline);
            pass.SetBindGroup(0, bindGroup);
            pass.SetVertexBuffer(0, vertexBuffer);
            pass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint32);
            pass.DrawIndexed(3);
            pass.EndPass();
        }

        wgpu::CommandBuffer commands = encoder.Finish();
        queue.Submit(1, &commands);
        swapchain.Present();
    }
};

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    std::unique_ptr<Example> mExample{ nullptr };
    IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(CoreApplicationView const &)
    {
    }

    void Load(hstring const&)
    {
    }

    void Uninitialize()
    {
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();

        CoreDispatcher dispatcher = window.Dispatcher();
        while (true)
        {
            dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessOneIfPresent);
            mExample->frame();
        }
    }

    void SetWindow(CoreWindow const & window)
    {
        mExample = std::make_unique<Example>(window);
        mExample->init();
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    init_apartment(apartment_type::multi_threaded);
    CoreApplication::Run(make<App>());
}
