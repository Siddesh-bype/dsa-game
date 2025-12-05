#include "UIVisualEffects.h"
#include "GameUtils.h"
#include <iostream>
#include <algorithm>

// Static member initialization
std::map<std::string, sf::VertexArray> UIVisualEffects::gradientCache;

void UIVisualEffects::drawPanelWithShadow(
    sf::RenderWindow& window,
    const sf::FloatRect& rect,
    const sf::Color& fillColor,
    const sf::Vector2f& shadowOffset,
    float shadowBlur,
    ShadowType shadowType
) {
    // Draw shadow layers
    if (shadowType == ShadowType::Soft) {
        // Multi-layer soft shadow with decreasing alpha
        const int layers = 4;
        for (int i = layers; i > 0; --i) {
            float layerBlur = shadowBlur * (static_cast<float>(i) / layers);
            float alpha = 40.f / i;  // Decrease alpha for outer layers
            
            sf::RectangleShape shadowLayer(sf::Vector2f(rect.size.x + layerBlur * 2,
                                                          rect.size.y + layerBlur * 2));
            shadowLayer.setPosition(sf::Vector2f(
                rect.position.x + shadowOffset.x - layerBlur,
                rect.position.y + shadowOffset.y - layerBlur
            ));
            shadowLayer.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
            window.draw(shadowLayer);
        }
    } else if (shadowType == ShadowType::Hard) {
        // Single sharp shadow
        sf::RectangleShape shadow(sf::Vector2f(rect.size));
        shadow.setPosition(sf::Vector2f(rect.position.x + shadowOffset.x,
                                        rect.position.y + shadowOffset.y));
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);
    } else if (shadowType == ShadowType::Ambient) {
        // Ambient darkening around edges
        sf::RectangleShape ambient(sf::Vector2f(rect.size.x + 10, rect.size.y + 10));
        ambient.setPosition(sf::Vector2f(rect.position.x - 5, rect.position.y - 5));
        ambient.setFillColor(sf::Color(0, 0, 0, 30));
        window.draw(ambient);
    }
    
    // Draw panel
    sf::RectangleShape panel(sf::Vector2f(rect.size));
    panel.setPosition(rect.position);
    panel.setFillColor(fillColor);
    window.draw(panel);
}

void UIVisualEffects::drawStyledPanel(
    sf::RenderWindow& window,
    const sf::FloatRect& rect,
    PanelStyle style,
    const sf::Color& baseColor,
    const sf::Color& borderColor
) {
    switch (style) {
        case PanelStyle::Solid: {
            // Solid panel with shadow
            drawPanelWithShadow(window, rect, baseColor, {4.f, 4.f}, 8.f, ShadowType::Soft);
            break;
        }
        
        case PanelStyle::Glass: {
            // Glassmorphism effect
            // Background blur approximation (dark overlay)
            sf::RectangleShape blurOverlay(sf::Vector2f(rect.size));
            blurOverlay.setPosition(rect.position);
            blurOverlay.setFillColor(sf::Color(0, 0, 0, 40));
            window.draw(blurOverlay);
            
            // Semi-transparent panel
            sf::RectangleShape glassPanel(sf::Vector2f(rect.size));
            glassPanel.setPosition(rect.position);
            sf::Color glassColor = baseColor;
            glassColor.a = 180;  // Semi-transparent
            glassPanel.setFillColor(glassColor);
            glassPanel.setOutlineThickness(1.f);
            glassPanel.setOutlineColor(sf::Color(255, 255, 255, 40));  // Subtle white edge
            window.draw(glassPanel);
            
            // Highlight on top edge (glass reflection)
            sf::RectangleShape highlight(sf::Vector2f(rect.size.x, 2.f));
            highlight.setPosition(sf::Vector2f(rect.position.x, rect.position.y));
            highlight.setFillColor(sf::Color(255, 255, 255, 25));
            window.draw(highlight);
            break;
        }
        
        case PanelStyle::Bordered: {
            // Shadow
            drawPanelWithShadow(window, rect, baseColor, {3.f, 3.f}, 6.f, ShadowType::Soft);
            
            // Thick border
            sf::RectangleShape border(sf::Vector2f(rect.size));
            border.setPosition(rect.position);
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(3.f);
            border.setOutlineColor(borderColor);
            window.draw(border);
            break;
        }
        
        case PanelStyle::Carved: {
            // Inset effect with top-left dark, bottom-right light
            // Dark top-left edge
            sf::RectangleShape topEdge(sf::Vector2f(rect.size.x, 2.f));
            topEdge.setPosition(rect.position);
            topEdge.setFillColor(sf::Color(0, 0, 0, 60));
            window.draw(topEdge);
            
            sf::RectangleShape leftEdge(sf::Vector2f(2.f, rect.size.y));
            leftEdge.setPosition(rect.position);
            leftEdge.setFillColor(sf::Color(0, 0, 0, 60));
            window.draw(leftEdge);
            
            // Main panel
            sf::RectangleShape panel(sf::Vector2f(rect.size));
            panel.setPosition(rect.position);
            panel.setFillColor(baseColor);
            window.draw(panel);
            
            // Light bottom-right edge
            sf::RectangleShape bottomEdge(sf::Vector2f(rect.size.x, 2.f));
            bottomEdge.setPosition(sf::Vector2f(rect.position.x, rect.position.y + rect.size.y - 2));
            bottomEdge.setFillColor(sf::Color(255, 255, 255, 30));
            window.draw(bottomEdge);
            
            sf::RectangleShape rightEdge(sf::Vector2f(2.f, rect.size.y));
            rightEdge.setPosition(sf::Vector2f(rect.position.x + rect.size.x - 2, rect.position.y));
            rightEdge.setFillColor(sf::Color(255, 255, 255, 30));
            window.draw(rightEdge);
            break;
        }
    }
}

void UIVisualEffects::drawTextWithShadow(
    sf::RenderWindow& window,
    sf::Text& text,
    const sf::Vector2f& shadowOffset,
    const sf::Color& shadowColor
) {
    // Draw shadow
    sf::Text shadow = text;
    shadow.setPosition(sf::Vector2f(
        text.getPosition().x + shadowOffset.x,
        text.getPosition().y + shadowOffset.y
    ));
    shadow.setFillColor(shadowColor);
    shadow.setOutlineColor(sf::Color::Transparent);
    window.draw(shadow);
    
    // Draw main text
    window.draw(text);
}

void UIVisualEffects::drawAnimatedBar(
    sf::RenderWindow& window,
    const sf::FloatRect& rect,
    float& progress,
    float targetProgress,
    float deltaTime,
    const sf::Color& colorGradientStart,
    const sf::Color& colorGradientEnd,
    bool animated
) {
    if (animated) {
        // Smooth interpolation
        smoothLerp(progress, targetProgress, deltaTime, 8.0f);
    } else {
        progress = targetProgress;
    }
    
    // Clamp progress
    progress = std::max(0.0f, std::min(1.0f, progress));
    
    // Background (empty bar)
    sf::RectangleShape bg(sf::Vector2f(rect.size));
    bg.setPosition(rect.position);
    bg.setFillColor(sf::Color(50, 50, 50));
    bg.setOutlineThickness(2.f);
    bg.setOutlineColor(sf::Color(30, 30, 30));
    window.draw(bg);
    
    // Filled bar with gradient
    if (progress > 0.0f) {
        float filledWidth = rect.size.x * progress;
        sf::VertexArray gradient = createVerticalGradient(
            sf::FloatRect(sf::Vector2f(rect.position.x, rect.position.y), sf::Vector2f(filledWidth, rect.size.y)),
            colorGradientStart,
            colorGradientEnd
        );
        window.draw(gradient);
        
        // Shine effect on top
        sf::RectangleShape shine(sf::Vector2f(filledWidth, 2.f));
        shine.setPosition(sf::Vector2f(rect.position.x, rect.position.y + 1));
        sf::Color shineColor = colorGradientStart;
        shineColor.a = 80;
        shine.setFillColor(shineColor);
        window.draw(shine);
    }
}

void UIVisualEffects::drawGlow(
    sf::RenderWindow& window,
    const sf::Shape& shape,
    const sf::Color& glowColor,
    float glowRadius,
    float glowIntensity
) {
    sf::FloatRect bounds = shape.getGlobalBounds();
    sf::Vector2f center(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    );
    
    sf::Color fadeColor = glowColor;
    fadeColor.a = static_cast<std::uint8_t>(glowIntensity * 255);
    
    sf::VertexArray glow = createRadialGradient(
        center,
        glowRadius,
        fadeColor,
        sf::Color(glowColor.r, glowColor.g, glowColor.b, 0),
        48  // segments
    );
    
    window.draw(glow, sf::BlendAdd);  // Additive blending for glow
}

void UIVisualEffects::smoothLerp(float& current, float target, float deltaTime, float smoothness) {
    float diff = target - current;
    current += diff * (deltaTime * smoothness);
    
    // Snap to target if very close (avoid infinite interpolation)
    if (std::abs(diff) < 0.001f) {
        current = target;
    }
}

sf::VertexArray UIVisualEffects::createVerticalGradient(
    const sf::FloatRect& rect,
    const sf::Color& topColor,
    const sf::Color& bottomColor
) {
    // SFML 3.0: Use TriangleStrip instead of Quads
    sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
    
    // Order: Top-Left, Bottom-Left, Top-Right, Bottom-Right
    gradient[0].position = sf::Vector2f(rect.position.x, rect.position.y);
    gradient[0].color = topColor;
    
    gradient[1].position = sf::Vector2f(rect.position.x, rect.position.y + rect.size.y);
    gradient[1].color = bottomColor;
    
    gradient[2].position = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y);
    gradient[2].color = topColor;
    
    gradient[3].position = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
    gradient[3].color = bottomColor;
    
    return gradient;
}

sf::VertexArray UIVisualEffects::createRadialGradient(
    const sf::Vector2f& center,
    float radius,
    const sf::Color& centerColor,
    const sf::Color& edgeColor,
    int segments
) {
    // SFML 3.0: Use Triangles instead of TriangleFan
    sf::VertexArray gradient(sf::PrimitiveType::Triangles, segments * 3);
    
    for (int i = 0; i < segments; ++i) {
        float angle1 = (static_cast<float>(i) / segments) * 2.0f * 3.14159265f;
        float angle2 = (static_cast<float>(i + 1) / segments) * 2.0f * 3.14159265f;
        
        float x1 = center.x + std::cos(angle1) * radius;
        float y1 = center.y + std::sin(angle1) * radius;
        
        float x2 = center.x + std::cos(angle2) * radius;
        float y2 = center.y + std::sin(angle2) * radius;
        
        // Center vertex
        gradient[i * 3].position = center;
        gradient[i * 3].color = centerColor;
        
        // Edge vertex 1
        gradient[i * 3 + 1].position = sf::Vector2f(x1, y1);
        gradient[i * 3 + 1].color = edgeColor;
        
        // Edge vertex 2
        gradient[i * 3 + 2].position = sf::Vector2f(x2, y2);
        gradient[i * 3 + 2].color = edgeColor;
    }
    
    return gradient;
}
