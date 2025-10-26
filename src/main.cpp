#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <memory>
#include <iostream>

// Particle system for magical effects
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color color;
    float size;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    std::mt19937 rng;
    std::uniform_real_distribution<float> angleDist;
    std::uniform_real_distribution<float> speedDist;
    
public:
    ParticleSystem() : rng(std::random_device{}()), angleDist(0, 2 * 3.14159f), speedDist(20, 50) {}
    
    void emit(sf::Vector2f position, sf::Color baseColor) {
        Particle p;
        p.position = position;
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.lifetime = 0;
        p.maxLifetime = 2.0f;
        p.color = baseColor;
        p.size = 3.0f;
        particles.push_back(p);
    }
    
    void update(float dt) {
        for (auto it = particles.begin(); it != particles.end();) {
            it->lifetime += dt;
            if (it->lifetime >= it->maxLifetime) {
                it = particles.erase(it);
            } else {
                it->position += it->velocity * dt;
                it->velocity.y += 30.0f * dt; // gravity
                it->color.a = static_cast<sf::Uint8>(255 * (1 - it->lifetime / it->maxLifetime));
                ++it;
            }
        }
    }
    
    void draw(sf::RenderWindow& window) {
        for (const auto& p : particles) {
            sf::CircleShape circle(p.size);
            circle.setPosition(p.position);
            circle.setFillColor(p.color);
            circle.setOrigin(p.size, p.size);
            window.draw(circle);
        }
    }
};

// Tab class for the menu
class MenuTab {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    std::string label;
    bool isHovered;
    bool isActive;
    float hoverAnimation;
    float glowIntensity;
    sf::Color baseColor;
    sf::Color glowColor;
    
public:
    MenuTab(sf::Vector2f pos, sf::Vector2f sz, const std::string& lbl, sf::Color color)
        : position(pos), size(sz), label(lbl), isHovered(false), 
          isActive(false), hoverAnimation(0), glowIntensity(0),
          baseColor(color), glowColor(sf::Color(150, 200, 255, 100)) {}
    
    void update(float dt, sf::Vector2f mousePos, float time) {
        // Check hover
        bool wasHovered = isHovered;
        isHovered = mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
                    mousePos.y >= position.y && mousePos.y <= position.y + size.y;
        
        // Animate hover effect
        if (isHovered) {
            hoverAnimation = std::min(1.0f, hoverAnimation + dt * 3.0f);
            glowIntensity = std::sin(time * 3.0f) * 0.3f + 0.7f;
        } else {
            hoverAnimation = std::max(0.0f, hoverAnimation - dt * 3.0f);
            glowIntensity = 0;
        }
    }
    
    void draw(sf::RenderWindow& window, sf::Font& font, float time) {
        // Draw glow effect when hovered - with softer edges
        if (hoverAnimation > 0) {
            // Draw multiple layers for soft glow
            for (int i = 3; i > 0; i--) {
                sf::RectangleShape glow(size + sf::Vector2f(i * 8.0f, i * 8.0f));
                glow.setPosition(position - sf::Vector2f(i * 4.0f, i * 4.0f));
                sf::Color gc = glowColor;
                gc.a = static_cast<sf::Uint8>((gc.a * hoverAnimation * glowIntensity) / (i * 2));
                glow.setFillColor(gc);
                window.draw(glow);
            }
        }
        
        // Draw tab background with rounded appearance (simulate with multiple overlapping shapes)
        // Draw main body
        sf::RectangleShape tab(size - sf::Vector2f(0, 10));
        tab.setPosition(position + sf::Vector2f(0, 5));
        
        // Animated color based on time and hover
        float wave = std::sin(time + position.y * 0.01f) * 0.15f + 0.85f;
        sf::Color color = baseColor;
        if (isActive) {
            color.r = static_cast<sf::Uint8>(std::min(255, static_cast<int>(color.r * 1.3f)));
            color.g = static_cast<sf::Uint8>(std::min(255, static_cast<int>(color.g * 1.3f)));
            color.b = static_cast<sf::Uint8>(std::min(255, static_cast<int>(color.b * 1.3f)));
        }
        color.r = static_cast<sf::Uint8>(color.r * wave);
        color.g = static_cast<sf::Uint8>(color.g * wave);
        color.b = static_cast<sf::Uint8>(color.b * wave);
        
        tab.setFillColor(color);
        window.draw(tab);
        
        // Draw rounded corners (circles)
        sf::CircleShape topCorner(5);
        topCorner.setPosition(position);
        topCorner.setFillColor(color);
        window.draw(topCorner);
        
        sf::CircleShape topCornerRight(5);
        topCornerRight.setPosition(position.x + size.x - 10, position.y);
        topCornerRight.setFillColor(color);
        window.draw(topCornerRight);
        
        sf::CircleShape bottomCorner(5);
        bottomCorner.setPosition(position.x, position.y + size.y - 10);
        bottomCorner.setFillColor(color);
        window.draw(bottomCorner);
        
        sf::CircleShape bottomCornerRight(5);
        bottomCornerRight.setPosition(position.x + size.x - 10, position.y + size.y - 10);
        bottomCornerRight.setFillColor(color);
        window.draw(bottomCornerRight);
        
        // Draw top and bottom rectangles to complete rounded look
        sf::RectangleShape topRect(sf::Vector2f(size.x - 10, 5));
        topRect.setPosition(position.x + 5, position.y);
        topRect.setFillColor(color);
        window.draw(topRect);
        
        sf::RectangleShape bottomRect(sf::Vector2f(size.x - 10, 5));
        bottomRect.setPosition(position.x + 5, position.y + size.y - 5);
        bottomRect.setFillColor(color);
        window.draw(bottomRect);
        
        // Subtle border for sleek look
        sf::RectangleShape border(size - sf::Vector2f(4, 4));
        border.setPosition(position + sf::Vector2f(2, 2));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1);
        border.setOutlineColor(sf::Color(100, 150, 255, static_cast<sf::Uint8>(80 + hoverAnimation * 100)));
        window.draw(border);
        
        // Draw text with subtle shadow for depth
        // Shadow
        sf::Text shadow;
        shadow.setFont(font);
        shadow.setString(label);
        shadow.setCharacterSize(18);
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        sf::FloatRect shadowBounds = shadow.getLocalBounds();
        shadow.setPosition(
            position.x + (size.x - shadowBounds.width) / 2 - shadowBounds.left + 2,
            position.y + (size.y - shadowBounds.height) / 2 - shadowBounds.top + 2
        );
        if (hoverAnimation > 0) {
            shadow.move(hoverAnimation * 5, 0);
        }
        window.draw(shadow);
        
        // Main text
        sf::Text text;
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 + hoverAnimation * 55)));
        
        // Center text
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            position.x + (size.x - textBounds.width) / 2 - textBounds.left,
            position.y + (size.y - textBounds.height) / 2 - textBounds.top
        );
        
        // Slight animation on hover
        if (hoverAnimation > 0) {
            text.move(hoverAnimation * 5, 0);
        }
        
        window.draw(text);
    }
    
    bool isClicked(sf::Vector2f mousePos) {
        return isHovered;
    }
    
    void setActive(bool active) {
        isActive = active;
    }
    
    bool getActive() const {
        return isActive;
    }
};

// Feature display area
class FeatureDisplay {
private:
    int activeTab;
    sf::Vector2f position;
    sf::Vector2f size;
    float animationTime;
    
public:
    FeatureDisplay(sf::Vector2f pos, sf::Vector2f sz)
        : activeTab(-1), position(pos), size(sz), animationTime(0) {}
    
    void setActiveTab(int tab) {
        activeTab = tab;
        animationTime = 0;
    }
    
    void update(float dt) {
        animationTime += dt;
    }
    
    void draw(sf::RenderWindow& window, sf::Font& font) {
        if (activeTab == -1) return;
        
        // Background panel with rounded corners simulation
        // Main panel body
        sf::RectangleShape panelBody(sf::Vector2f(size.x, size.y - 10));
        panelBody.setPosition(position.x, position.y + 5);
        panelBody.setFillColor(sf::Color(20, 20, 40, 200));
        window.draw(panelBody);
        
        // Rounded corners for panel
        float cornerRadius = 8;
        sf::CircleShape corners[4] = {
            sf::CircleShape(cornerRadius), // top-left
            sf::CircleShape(cornerRadius), // top-right
            sf::CircleShape(cornerRadius), // bottom-left
            sf::CircleShape(cornerRadius)  // bottom-right
        };
        
        corners[0].setPosition(position.x, position.y);
        corners[1].setPosition(position.x + size.x - cornerRadius * 2, position.y);
        corners[2].setPosition(position.x, position.y + size.y - cornerRadius * 2);
        corners[3].setPosition(position.x + size.x - cornerRadius * 2, position.y + size.y - cornerRadius * 2);
        
        for (auto& corner : corners) {
            corner.setFillColor(sf::Color(20, 20, 40, 200));
            window.draw(corner);
        }
        
        // Top and bottom strips to complete rounded look
        sf::RectangleShape topStrip(sf::Vector2f(size.x - cornerRadius * 2, cornerRadius));
        topStrip.setPosition(position.x + cornerRadius, position.y);
        topStrip.setFillColor(sf::Color(20, 20, 40, 200));
        window.draw(topStrip);
        
        sf::RectangleShape bottomStrip(sf::Vector2f(size.x - cornerRadius * 2, cornerRadius));
        bottomStrip.setPosition(position.x + cornerRadius, position.y + size.y - cornerRadius);
        bottomStrip.setFillColor(sf::Color(20, 20, 40, 200));
        window.draw(bottomStrip);
        
        // Sleek border
        sf::RectangleShape border(sf::Vector2f(size.x - 4, size.y - 4));
        border.setPosition(position.x + 2, position.y + 2);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1);
        border.setOutlineColor(sf::Color(100, 150, 255, 120));
        window.draw(border);
        
        // Title with floating effect
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(24);
        
        // Subtle floating animation for title
        float titleFloat = std::sin(animationTime * 2.0f) * 3.0f;
        
        // Content
        sf::Text content;
        content.setFont(font);
        content.setCharacterSize(16);
        
        // Different content for each tab
        std::string titleStr, contentStr;
        switch(activeTab) {
            case 0:
                titleStr = "✨ Cosmic Harmony";
                contentStr = "Experience the symphony of the universe.\n\n"
                           "• Stellar resonance patterns\n"
                           "• Quantum entanglement visualizer\n"
                           "• Nebula color palette generator\n"
                           "• Gravitational wave detector";
                break;
            case 1:
                titleStr = "🌟 Ethereal Dreams";
                contentStr = "Journey through dimensions unknown.\n\n"
                           "• Dream sequence analyzer\n"
                           "• Astral projection simulator\n"
                           "• Consciousness expansion tools\n"
                           "• Reality distortion field";
                break;
            case 2:
                titleStr = "🌌 Celestial Wonders";
                contentStr = "Discover the mysteries of space.\n\n"
                           "• Black hole observation deck\n"
                           "• Supernova birth tracker\n"
                           "• Cosmic ray analyzer\n"
                           "• Dark matter detector";
                break;
            case 3:
                titleStr = "💫 Quantum Magic";
                contentStr = "Bend reality with quantum mechanics.\n\n"
                           "• Probability wave manipulator\n"
                           "• Superposition state viewer\n"
                           "• Quantum teleportation pad\n"
                           "• Schrödinger's simulator";
                break;
            case 4:
                titleStr = "🔮 Mystic Portal";
                contentStr = "Open gateways to other realms.\n\n"
                           "• Interdimensional gateway\n"
                           "• Timeline browser\n"
                           "• Parallel universe viewer\n"
                           "• Temporal anomaly detector";
                break;
        }
        
        // Draw title shadow for depth
        sf::Text titleShadow;
        titleShadow.setFont(font);
        titleShadow.setString(titleStr);
        titleShadow.setCharacterSize(24);
        titleShadow.setFillColor(sf::Color(0, 0, 0, 100));
        titleShadow.setPosition(position.x + 22, position.y + 22 + titleFloat);
        window.draw(titleShadow);
        
        // Draw title
        title.setString(titleStr);
        title.setFillColor(sf::Color(220, 240, 255));
        title.setPosition(position.x + 20, position.y + 20 + titleFloat);
        window.draw(title);
        
        content.setString(contentStr);
        content.setFillColor(sf::Color(180, 200, 240));
        content.setPosition(position.x + 20, position.y + 60);
        
        // Fade in animation
        float alpha = std::min(1.0f, animationTime * 2.0f);
        sf::Color c = content.getFillColor();
        c.a = static_cast<sf::Uint8>(c.a * alpha);
        content.setFillColor(c);
        
        window.draw(content);
        
        // Animated decorative stars with floating effect
        for (int i = 0; i < 5; i++) {
            float angle = animationTime * 0.5f + i * 1.2566f; // 2*pi/5
            float radius = 30 + std::sin(animationTime * 2 + i) * 10;
            float floatY = std::sin(animationTime * 1.5f + i * 0.5f) * 5.0f;
            sf::CircleShape star(3);
            star.setPosition(
                position.x + size.x - 60 + std::cos(angle) * radius,
                position.y + 60 + std::sin(angle) * radius + floatY
            );
            star.setFillColor(sf::Color(255, 255, 200, static_cast<sf::Uint8>(150 + std::sin(animationTime * 3 + i) * 105)));
            window.draw(star);
        }
    }
};

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(1200, 800), "✨ Euphoric Spacy Magical Menu ✨",
                           sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    
    // Load font (using elegant/modern fonts)
    sf::Font font;
    // Try modern, elegant fonts first - Lato is a beautiful modern font
    if (!font.loadFromFile("/usr/share/fonts/truetype/lato/Lato-Light.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
                    if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
                        // Font loading failed
                        std::cerr << "Error: Could not load any font file. Please ensure Lato, Liberation, or DejaVu fonts are installed." << std::endl;
                        return 1;
                    }
                }
            }
        }
    }
    
    // Create particle system
    ParticleSystem particles;
    
    // Create menu tabs on the left side
    std::vector<MenuTab> tabs;
    float tabHeight = 80;
    float tabWidth = 200;
    float startY = 100;
    float spacing = 10;
    
    // Create 5 tabs with different colors (space theme)
    std::vector<sf::Color> tabColors = {
        sf::Color(50, 50, 100),   // Deep space blue
        sf::Color(70, 50, 90),    // Purple nebula
        sf::Color(40, 70, 90),    // Cosmic cyan
        sf::Color(80, 50, 80),    // Mystic purple
        sf::Color(50, 70, 100)    // Stellar blue
    };
    
    for (int i = 0; i < 5; i++) {
        tabs.emplace_back(
            sf::Vector2f(20, startY + i * (tabHeight + spacing)),
            sf::Vector2f(tabWidth, tabHeight),
            "Tab " + std::to_string(i + 1),
            tabColors[i]
        );
    }
    
    // Create feature display
    FeatureDisplay featureDisplay(
        sf::Vector2f(tabWidth + 60, 100),
        sf::Vector2f(900, 650)
    );
    
    sf::Clock clock;
    sf::Clock particleClock;
    float time = 0;
    
    // Background stars
    std::vector<sf::CircleShape> stars;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(0, 1200);
    std::uniform_real_distribution<float> yDist(0, 800);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.5f);
    std::uniform_int_distribution<int> alphaDist(100, 255);
    
    for (int i = 0; i < 200; i++) {
        sf::CircleShape star(sizeDist(rng));
        star.setPosition(xDist(rng), yDist(rng));
        star.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alphaDist(rng))));
        stars.push_back(star);
    }
    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        time += dt;
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    
                    // Check tab clicks
                    for (size_t i = 0; i < tabs.size(); i++) {
                        if (tabs[i].isClicked(mousePos)) {
                            // Deactivate all tabs
                            for (auto& tab : tabs) {
                                tab.setActive(false);
                            }
                            // Activate clicked tab
                            tabs[i].setActive(true);
                            featureDisplay.setActiveTab(i);
                            
                            // Emit particles
                            for (int j = 0; j < 20; j++) {
                                particles.emit(mousePos, sf::Color(150, 200, 255));
                            }
                        }
                    }
                }
            }
        }
        
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        
        // Update
        for (auto& tab : tabs) {
            tab.update(dt, mousePos, time);
        }
        featureDisplay.update(dt);
        particles.update(dt);
        
        // Emit random particles occasionally
        if (particleClock.getElapsedTime().asSeconds() > 0.1f) {
            for (size_t i = 0; i < tabs.size(); i++) {
                if (tabs[i].getActive()) {
                    sf::Vector2f tabPos(30 + tabWidth / 2, 100 + i * (tabHeight + spacing) + tabHeight / 2);
                    particles.emit(tabPos, sf::Color(200, 220, 255, 150));
                }
            }
            particleClock.restart();
        }
        
        // Draw
        window.clear(sf::Color(10, 10, 25)); // Deep space background
        
        // Draw animated stars
        for (size_t i = 0; i < stars.size(); i++) {
            float twinkle = std::sin(time * 2 + i * 0.1f) * 0.3f + 0.7f;
            sf::Color starColor = stars[i].getFillColor();
            starColor.a = static_cast<sf::Uint8>(starColor.a * twinkle);
            stars[i].setFillColor(starColor);
            window.draw(stars[i]);
        }
        
        // Draw particles
        particles.draw(window);
        
        // Draw feature display
        featureDisplay.draw(window, font);
        
        // Draw tabs (on top)
        for (auto& tab : tabs) {
            tab.draw(window, font, time);
        }
        
        // Draw title with floating animation and multiple effects
        float floatOffset = std::sin(time * 1.5f) * 15.0f; // Smooth up/down floating
        float floatOffsetX = std::cos(time * 0.8f) * 8.0f; // Subtle horizontal sway
        float scale = 1.0f + std::sin(time * 2.0f) * 0.05f; // Gentle pulsing
        
        // Draw multiple shadow layers for depth and glow
        for (int i = 4; i > 0; i--) {
            sf::Text titleShadow;
            titleShadow.setFont(font);
            titleShadow.setString("✨ EUPHORIC SPACY MAGICAL MENU ✨");
            titleShadow.setCharacterSize(32);
            sf::Color shadowColor(100, 150, 255, static_cast<sf::Uint8>(30 / i));
            titleShadow.setFillColor(shadowColor);
            titleShadow.setPosition(250 + floatOffsetX + i * 2, 20 + floatOffset + i * 2);
            titleShadow.setScale(scale, scale);
            window.draw(titleShadow);
        }
        
        // Main title with gradient-like effect using multiple text layers
        sf::Text title;
        title.setFont(font);
        title.setString("✨ EUPHORIC SPACY MAGICAL MENU ✨");
        title.setCharacterSize(32);
        
        // Animated color with smooth transitions
        float colorWave = std::sin(time * 2) * 0.5f + 0.5f;
        sf::Uint8 r = static_cast<sf::Uint8>(200 + colorWave * 55);
        sf::Uint8 g = static_cast<sf::Uint8>(220 + std::sin(time * 2.3f) * 35);
        sf::Uint8 b = 255;
        sf::Uint8 a = static_cast<sf::Uint8>(240 + std::sin(time * 3.0f) * 15);
        
        title.setFillColor(sf::Color(r, g, b, a));
        title.setPosition(250 + floatOffsetX, 20 + floatOffset);
        title.setScale(scale, scale);
        
        // Add subtle outline for crispness
        title.setOutlineThickness(1);
        title.setOutlineColor(sf::Color(150, 180, 255, static_cast<sf::Uint8>(a * 0.7f)));
        
        window.draw(title);
        
        window.display();
    }
    
    return 0;
}
