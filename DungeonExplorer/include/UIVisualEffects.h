#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

/**
 * UIVisualEffects - Modern UI rendering utilities
 * Provides drop shadows, glassmorphism, smooth animations, and depth effects
 */
class UIVisualEffects {
public:
    // Shadow types
    enum class ShadowType {
        Soft,        // Blurred shadow (multisampled)
        Hard,        // Sharp shadow
        Ambient      // Ambient occlusion-like darkening
    };
    
    // Panel styles
    enum class PanelStyle {
        Solid,           // Standard opaque panel
        Glass,           // Glassmorphism (semi-transparent with blur)
        Bordered,        // Thick border with gradient
        Carved           // Inset/embossed look
    };
    
    /**
     * Render a panel with drop shadow
     */
    static void drawPanelWithShadow(
        sf::RenderWindow& window,
        const sf::FloatRect& rect,
        const sf::Color& fillColor,
        const sf::Vector2f& shadowOffset = {4.f, 4.f},
        float shadowBlur = 8.f,
       ShadowType shadowType = ShadowType::Soft
    );
    
    /**
     * Render a styled panel with modern effects
     */
    static void drawStyledPanel(
        sf::RenderWindow& window,
        const sf::FloatRect& rect,
        PanelStyle style,
        const sf::Color& baseColor,
        const sf::Color& borderColor = sf::Color(100, 100, 120)
    );
    
    /**
     * Draw text with drop shadow
     */
    static void drawTextWithShadow(
        sf::RenderWindow& window,
        sf::Text& text,
        const sf::Vector2f& shadowOffset = {2.f, 2.f},
        const sf::Color& shadowColor = sf::Color(0, 0, 0, 120)
    );
    
    /**
     * Draw a progress bar with smooth animation
     */
    static void drawAnimatedBar(
        sf::RenderWindow& window,
        const sf::FloatRect& rect,
        float& progress,              // Current anim progress (modified)
        float targetProgress,          // Target value
        float deltaTime,
        const sf::Color& colorGradientStart,
        const sf::Color& colorGradientEnd,
        bool animated = true
    );
    
    /**
     * Draw a glow effect around a shape
     */
    static void drawGlow(
        sf::RenderWindow& window,
        const sf::Shape& shape,
        const sf::Color& glowColor,
        float glowRadius = 20.f,
        float glowIntensity = 0.5f
    );
    
    /**
     * Interpolate smoothly between current and target values
     */
    static void smoothLerp(float& current, float target, float deltaTime, float smoothness = 5.0f);
    
    /**
     * Create a vertical gradient fill
     */
    static sf::VertexArray createVerticalGradient(
        const sf::FloatRect& rect,
        const sf::Color& topColor,
        const sf::Color& bottomColor
    );
    
    /**
     * Create a radial gradient (for glows, vignettes)
     */
    static sf::VertexArray createRadialGradient(
        const sf::Vector2f& center,
        float radius,
        const sf::Color& centerColor,
        const sf::Color& edgeColor,
        int segments = 64
    );
    
private:
    // Cached gradients for performance
    static std::map<std::string, sf::VertexArray> gradientCache;
    
    // ═══════════════════════════════════════════════════════════════════════
    // UI VISUAL EFFECTS CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    static constexpr int SHADOW_LAYERS = 4;
    static constexpr float BASE_SHADOW_ALPHA = 40.0f;
    static constexpr int HARD_SHADOW_ALPHA = 100;
    static constexpr int AMBIENT_ALPHA = 30;
    static constexpr float AMBIENT_EXPAND = 5.0f;
    static constexpr int GLASS_ALPHA = 180;
    static constexpr int GLASS_EDGE_ALPHA = 40;
    static constexpr int GLASS_HIGHLIGHT_ALPHA = 25;
    static constexpr float HIGHLIGHT_HEIGHT = 2.0f;
    static constexpr float BORDER_THICKNESS = 3.0f;
    static constexpr float CARVED_EDGE = 2.0f;
    static constexpr int CARVED_DARK_ALPHA = 60;
    static constexpr int CARVED_LIGHT_ALPHA = 30;
    static constexpr float BAR_OUTLINE = 2.0f;
    static constexpr int BAR_BG_GRAY = 50;
    static constexpr int BAR_OUTLINE_GRAY = 30;
    static constexpr float SHINE_HEIGHT = 2.0f;
    static constexpr int SHINE_ALPHA = 80;
    static constexpr float LERP_SNAP_THRESHOLD = 0.001f;
    static constexpr int DEFAULT_GLOW_SEGMENTS = 48;
    static constexpr float TWO_PI = 6.28318f;
};
