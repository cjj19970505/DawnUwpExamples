# DawnUwpExample

This repo shows how to use [Dawn](https://dawn.googlesource.com/dawn/) in UWP.  
Related issue can be tracked [here](https://bugs.chromium.org/p/dawn/issues/detail?id=766).

# Getting Started

1. Get the source as follows:

```powershell
# Clone the repo as "dawn"
git clone https://dawn.googlesource.com/dawn dawn && cd dawn

# Bootstrap the gclient configuration
cp scripts/standalone.gclient .gclient

# Fetch external dependencies and toolchains with gclient
gclient sync
```

2. Generate build files and build:
```powershell
# Debug build isn't supported currently
# Can optionally add "--ide='vs'" to generate visual studio (default 2019)
# https://gn.googlesource.com/gn/+/refs/heads/main/docs/reference.md#ide-options
gn gen out/uwp/Release --args='is_debug=false target_os=\"winuwp\" is_clang=false'

# These targets are required
ninja -C .\out\uwp\Release\ common dawn_proc_shared dawncpp dawn_native_shared dawn_platform_shared dawn_utils
```
3. Set environment variable `DawnBaseDir` to your Dawn repo location.
4. Clone this repo, open `DawnUwpExamples.sln` with Visual Studio.
5. Set Configuration to `Release` and Platform to `x64`.
