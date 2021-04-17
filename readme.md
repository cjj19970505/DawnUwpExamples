# DawnUwpExample

This repo shows how to use [Dawn](https://dawn.googlesource.com/dawn/) in UWP.  
Dawn with UWP support is developing [here](https://github.com/cjj19970505/dawn/tree/winrt)

# Getting Started

1. Use following commands to clone and build dawn targeting winuwp

```powershell
# Clone the repo and checkout to 'winrt' branch
git clone https://github.com/cjj19970505/dawn.git && cd dawn && git checkout winrt

# Bootstrap the gclient configuration
cp scripts/standalone.gclient .gclient

# Fetch external dependencies and toolchains with gclient
gclient sync

gn gen out/uwp/Release --args='is_debug=false target_os=\"winuwp\" is_clang=false'

ninja -C .\out\uwp\Release\ dawn_proc_shared dawncpp dawn_native_shared dawn_platform_shared dawn_utils common
```
2. Set environment variable `DawnBaseDir` to your Dawn repo location.
3. Clone this repo, open `DawnUwpExamples.sln` with Visual Studio.
4. Set Configuration to `Debug` and Platform to `x64`.
