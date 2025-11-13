# 🚀 Quick Start Guide - Dungeon Explorer

## For Absolute Beginners

### What You Need

1. **A Computer** running Windows, Linux, or macOS
2. **C++ Compiler** (comes with Visual Studio on Windows)
3. **SFML Library** (for graphics)
4. **TGUI Library** (for UI)
5. **CMake** (for building)

---

## Windows Quick Start (Easiest Method)

### Step 1: Install Visual Studio 2022 (Free)
1. Download from: https://visualstudio.microsoft.com/downloads/
2. Choose "Community Edition" (Free)
3. During install, select "Desktop development with C++"
4. Wait for installation (15-30 minutes)

### Step 2: Install vcpkg (Package Manager)
Open PowerShell and run:
```powershell
# Navigate to C drive
cd C:\

# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git

# Build vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat

# Install SFML and TGUI
.\vcpkg install sfml:x64-windows
.\vcpkg install tgui:x64-windows

# This will take 10-20 minutes
```

### Step 3: Build the Game
```powershell
# Navigate to project
cd "C:\Users\sidde\OneDrive\Desktop\FSD\dsa game\DungeonExplorer"

# Create build folder
mkdir build
cd build

# Generate project with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"

# Build (takes 2-5 minutes)
cmake --build . --config Release

# Run!
cd Release
.\DungeonExplorer.exe
```

---

## Alternative: Manual Installation (Windows)

### If vcpkg doesn't work, try this:

1. **Download SFML**: 
   - Go to https://www.sfml-dev.org/download.php
   - Download "Visual C++ 15 (2017) - 64-bit"
   - Extract to `C:\SFML-2.6.0`

2. **Download TGUI**:
   - Go to https://github.com/texus/TGUI/releases
   - Download Windows version
   - Extract to `C:\TGUI-1.0`

3. **Build**:
```powershell
cd "C:\Users\sidde\OneDrive\Desktop\FSD\dsa game\DungeonExplorer"
mkdir build
cd build

cmake .. -DSFML_DIR="C:/SFML-2.6.0/lib/cmake/SFML" ^
         -DTGUI_DIR="C:/TGUI-1.0/lib/cmake/TGUI"

cmake --build . --config Release

# Copy DLLs
copy C:\SFML-2.6.0\bin\*.dll Release\
copy C:\TGUI-1.0\bin\*.dll Release\

cd Release
.\DungeonExplorer.exe
```

---

## Linux Quick Start (Ubuntu/Debian)

### One Command Install & Build:
```bash
# Install everything
sudo apt-get update && \
sudo apt-get install -y build-essential cmake git libsfml-dev libtgui-dev

# Navigate to project
cd ~/DungeonExplorer

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./DungeonExplorer
```

---

## macOS Quick Start

### Using Homebrew:
```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake sfml tgui

# Build
cd ~/DungeonExplorer
mkdir build && cd build
cmake ..
make

# Run
./DungeonExplorer
```

---

## What You Should See

When you run the game successfully:

### Console Output:
```
========================================
   DUNGEON EXPLORER - DSA Game
========================================

[Dungeon] Generating dungeon with 8 rooms...
[DSA-Graph] Added vertex. Total vertices: 1
[Player] Initialized at position (2, 2)
[DSA-Stack] Pushed element. Stack size: 1
...
[Game] Initialization complete!
```

### Game Window:
- 800x600 window titled "Dungeon Explorer"
- Main menu with "Start Game" and "Exit" buttons
- Click "Start Game" to begin

### Gameplay:
- Gray grid representing the dungeon
- Green square = your player
- Red squares = enemies
- Blue tiles = start room
- Yellow tiles = exit room

---

## Controls Reminder

| Key | Action |
|-----|--------|
| W/↑ | Move Up |
| A/← | Move Left |
| S/↓ | Move Down |
| D/→ | Move Right |
| B | Backtrack (undo movement) |
| I | Open Inventory |
| K | Open Skill Tree |
| ESC | Return to Menu |

---

## Troubleshooting

### "CMake not found"
```powershell
# Install CMake
# Windows: Download from https://cmake.org/download/
# Linux: sudo apt-get install cmake
# macOS: brew install cmake
```

### "SFML not found" or "TGUI not found"
**Solution**: Use vcpkg method (easiest) or follow manual installation

### "Cannot open window"
- Check if graphics drivers are updated
- Try running in compatibility mode (Windows)

### "Missing DLL" (Windows)
```powershell
# Copy DLLs to same folder as .exe
copy C:\SFML-2.6.0\bin\*.dll Release\
copy C:\TGUI-1.0\bin\*.dll Release\
```

---

## Next Steps After Successfully Running

1. ✅ **Play the Game**: Explore the dungeon!
2. ✅ **Watch Console**: See DSA operations logged
3. ✅ **Read Code**: Check `src/` and `include/` folders
4. ✅ **Modify**: Try changing dungeon size in `Game.cpp`
5. ✅ **Add Assets**: Place graphics in `assets/` folders

---

## Learning Path

1. **Week 1**: Understand Stack and Queue (Player movement & enemies)
2. **Week 2**: Study Linked List and Hash Table (Inventory & items)
3. **Week 3**: Learn Heap and Tree (Loot system & skill tree)
4. **Week 4**: Master Graph algorithms (BFS, DFS, Dijkstra)

---

## Getting Help

If stuck:
1. Check `BUILD_GUIDE.md` for detailed instructions
2. Check `README.md` for comprehensive documentation
3. Read `DSA_REPORT.md` to understand data structures
4. Review console output for error messages

---

## Success Checklist

- [ ] Visual Studio / GCC / Clang installed
- [ ] CMake installed and in PATH
- [ ] SFML library found by CMake
- [ ] TGUI library found by CMake
- [ ] Project builds without errors
- [ ] Game window opens
- [ ] Can see main menu
- [ ] Can move player with WASD
- [ ] Console shows DSA logs

---

**Congratulations! You're now ready to explore dungeons and data structures!** 🎮✨
