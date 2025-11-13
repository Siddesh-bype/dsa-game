# Asset Guidelines

## Required Assets

This game needs the following assets to run with full visual features:

### Tiles (assets/tiles/)
- `floor.png` - Floor tile (32x32 pixels)
- `wall.png` - Wall tile (32x32 pixels)
- `door.png` - Door tile (32x32 pixels)

### Characters (assets/characters/)
- `player.png` - Player sprite (32x32 pixels)
- `enemy_goblin.png` - Goblin enemy sprite (32x32 pixels)
- `enemy_orc.png` - Orc enemy sprite (32x32 pixels)

### UI (assets/ui/)
- `button.png` - UI button background
- `panel.png` - UI panel background
- `health_bar.png` - Health bar sprite

### Fonts (assets/fonts/)
- `PressStart2P.ttf` - Retro pixel font
- Or any .ttf font file

### Sounds (assets/sounds/)
- `step.wav` - Footstep sound
- `attack.wav` - Attack sound
- `pickup.wav` - Item pickup sound
- `level_up.wav` - Level up sound

## Recommended Sources

### Free Asset Packs:
1. **0x72's Dungeon Pack**: https://0x72.itch.io/dungeontileset-ii
   - High-quality pixel art tiles and characters
   - Perfect for dungeon crawler games

2. **Kenney Assets**: https://kenney.nl/
   - UI packs, sounds, and more
   - All assets are public domain

3. **OpenGameArt**: https://opengameart.org/
   - Community-created game assets
   - Various licenses available

4. **Press Start 2P Font**: https://fonts.google.com/specimen/Press+Start+2P
   - Free retro gaming font
   - Perfect for pixel art games

## Creating Your Own Assets

### Tile Specifications:
- Size: 32x32 pixels (or 16x16, 64x64)
- Format: PNG with transparency
- Color: Match your game's palette

### Character Sprites:
- Size: 32x32 pixels (same as tiles)
- Format: PNG with transparency
- Animation: Multiple frames in sprite sheet (optional)

### Sound Effects:
- Format: WAV or OGG
- Sample Rate: 44.1 kHz
- Bit Depth: 16-bit
- Length: 0.5-2 seconds for SFX

## Placeholder Graphics

The game will work without custom assets using colored rectangles:
- Floor: Gray rectangles
- Walls: Dark gray rectangles
- Player: Green rectangle
- Enemies: Red rectangles
- Treasures: Yellow rectangles

To add custom graphics, simply replace the rendering code in:
- `src/Renderer.cpp`
- `src/Player.cpp`
- `src/Dungeon.cpp`
- `src/Enemy.cpp`

## Asset Loading Example

```cpp
// In your code:
sf::Texture texture;
if (!texture.loadFromFile("assets/tiles/floor.png")) {
    // Fallback to colored rectangle
    std::cout << "Using placeholder graphics" << std::endl;
}
```

## License Note

Make sure to:
1. Check the license of any assets you use
2. Credit the creators appropriately
3. Follow redistribution guidelines
4. Keep licenses in your project if required
