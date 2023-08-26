# ClipboardActions

This is a Windows system tray program that monitors what you copy to your clipboard and opens programs based on regexes you set.
For example, when you copy a YouTube url, you can open VLC or any video player of your choice and play that video.

This is alpha software, so there will be bugs. Please create an issue.

# Usage

1. Open ClipboardActions.exe
2. Right click its icon in the system tray
3. Click settings
4. Fill out the required information

You can use regex groups 0-9 like so: {0}, {1}, {2}, etc.

![Screenshot 2023-08-26 125526](https://github.com/FaithBeam/ClipboardActions/assets/32502411/93a2e28b-54f9-41a1-b09e-0338744fdfdc)


# Compiling

**Requirements**

- cmake
- vcpkg
- msvc 2022 (From Visual Studio or the Build Tools)

$VCPKG_INSTALLATION_ROOT is the path to your vcpkg installation

```
git clone https://github.com/FaithBeam/ClipboardActions
cd ClipboardActions
cmake -B build -S . "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```
