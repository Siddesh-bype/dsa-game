# Quick Build Guide - Dungeon Explorer

## Windows (Visual Studio)

### Prerequisites Check
```powershell
# Check if CMake is installed
cmake --version

# Check if Visual Studio is installed
where cl.exe
```

### Step 1: Install Dependencies

1. **Install SFML 2.6**
   - Download from: https://www.sfml-dev.org/download/sfml/2.6.0/
   - Choose: `SFML-2.6.0-windows-vc15-64-bit.zip` (for Visual Studio 2019/2022)
   - Extract to: `C:\SFML-2.6.0`

2. **Install TGUI 1.0+**
   - Download from: https://github.com/texus/TGUI/releases
   - Choose precompiled version matching your Visual Studio
   - Extract to: `C:\TGUI-1.0`

### Step 2: Set Environment Variables

```powershell
# Open PowerShell as Administrator
[System.Environment]::SetEnvironmentVariable('SFML_DIR', 'C:\SFML-2.6.0', 'User')
[System.Environment]::SetEnvironmentVariable('TGUI_DIR', 'C:\TGUI-1.0', 'User')

# Or manually:
# Right-click "This PC" → Properties → Advanced System Settings
# → Environment Variables → New (User Variable)
# Name: SFML_DIR, Value: C:\SFML-2.6.0
# Name: TGUI_DIR, Value: C:\TGUI-1.0
```

### Step 3: Build Project

```powershell
# Navigate to project
cd "C:\Users\sidde\OneDrive\Desktop\FSD\dsa game\DungeonExplorer"

# Create build directory
mkdir build
cd build

# Generate solution (Visual Studio 2019)
cmake .. -G "Visual Studio 16 2019" -A x64 ^
  -DSFML_DIR="C:/SFML-2.6.0/lib/cmake/SFML" ^
  -DTGUI_DIR="C:/TGUI-1.0/lib/cmake/TGUI"

# Or for Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DSFML_DIR="C:/SFML-2.6.0/lib/cmake/SFML" ^
  -DTGUI_DIR="C:/TGUI-1.0/lib/cmake/TGUI"

# Build
cmake --build . --config Release

# Copy DLLs to build directory
copy "C:\SFML-2.6.0\bin\*.dll" Release\
copy "C:\TGUI-1.0\bin\*.dll" Release\

# Run the game
cd Release
.\DungeonExplorer.exe
```

---

## Windows (MinGW)

### Prerequisites
```powershell
# Install MinGW-w64 via MSYS2
# Download from: https://www.msys2.org/

# After installing MSYS2, open MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-sfml
pacman -S mingw-w64-x86_64-tgui
```

### Build
```bash
# In MSYS2 MinGW64 terminal
cd /c/Users/sidde/OneDrive/Desktop/FSD/dsa\ game/DungeonExplorer

mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make

./DungeonExplorer.exe
```

---

## Linux (Ubuntu/Debian)

### Install Dependencies
```bash
# Update package list
sudo apt-get update

# Install build tools
sudo apt-get install -y build-essential cmake git

# Install SFML
sudo apt-get install -y libsfml-dev

# Install TGUI (if available)
sudo apt-get install -y libtgui-dev

# If TGUI not available, build from source:
cd /tmp
git clone https://github.com/texus/TGUI.git
cd TGUI
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

### Build Project
```bash
cd ~/DungeonExplorer
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./DungeonExplorer
```

---

## macOS

### Install Dependencies
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake
brew install sfml
brew install tgui
```

### Build Project
```bash
cd ~/DungeonExplorer
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

# Run
./DungeonExplorer
```

---

## Common Build Issues

### Issue: "SFML not found"
**Solution:**
```powershell
# Windows
cmake .. -DSFML_DIR="C:/SFML-2.6.0/lib/cmake/SFML"

# Linux/Mac
export SFML_DIR=/usr/local/lib/cmake/SFML
```

### Issue: "TGUI not found"
**Solution:**
```powershell
# Windows
cmake .. -DTGUI_DIR="C:/TGUI-1.0/lib/cmake/TGUI"

# Linux/Mac
export TGUI_DIR=/usr/local/lib/cmake/TGUI
```

### Issue: "Missing DLLs when running" (Windows)
**Solution:**
```powershell
# Copy all required DLLs to executable directory
copy "C:\SFML-2.6.0\bin\sfml-graphics-2.dll" build\Release\
copy "C:\SFML-2.6.0\bin\sfml-window-2.dll" build\Release\
copy "C:\SFML-2.6.0\bin\sfml-system-2.dll" build\Release\
copy "C:\SFML-2.6.0\bin\sfml-audio-2.dll" build\Release\
copy "C:\TGUI-1.0\bin\tgui.dll" build\Release\
```

### Issue: "C++17 features not available"
**Solution:**
```bash
# Ensure you have a modern compiler
gcc --version  # Should be 7.0 or higher
clang --version  # Should be 5.0 or higher
```

---

## Minimal Build Test

If you're having trouble, try this minimal test first:

```cpp
// test_sfml.cpp
#include <SFML/Graphics.hpp>
int main() {
    sf::RenderWindow window(sf::VideoMode(200, 200), "Test");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.display();
    }
    return 0;
}
```

Compile:
```bash
g++ test_sfml.cpp -o test -lsfml-graphics -lsfml-window -lsfml-system
./test
```

If this works, SFML is correctly installed.

---

## Alternative: Use vcpkg (Recommended for Windows)

```powershell
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install sfml:x64-windows
.\vcpkg install tgui:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install

# Build project
cd "C:\Users\sidde\OneDrive\Desktop\FSD\dsa game\DungeonExplorer"
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release
```

---

## Next Steps After Building

1. **Run the game**: Navigate to build directory and execute
2. **Check console**: You should see DSA operations logged
3. **Test controls**: Use WASD to move, B to backtrack
4. **Add assets**: Place graphics in `assets/` folders for better visuals

---

## Need Help?

- Check `README.md` for detailed information
- Review console output for error messages
- Ensure all paths are correct and libraries are found
- Try building in Debug mode first: `cmake --build . --config Debug`
