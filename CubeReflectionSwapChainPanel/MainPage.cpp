#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"


using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::CubeReflectionSwapChainPanel::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
        
        
    }

    Windows::Foundation::IAsyncAction MainPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
        mExample = std::make_unique<Example>(GpuView());
        mExample->init();
        co_await resume_background();
        while (true)
        {
            mExample->frame();
        }
    }
}