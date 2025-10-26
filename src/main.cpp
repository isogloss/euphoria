#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <memory>

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
    
    void update(float dt, sf::Vector2f mousePos) {
        // Check hover
        bool wasHovered = isHovered;
        isHovered = mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
                    mousePos.y >= position.y && mousePos.y <= position.y + size.y;
        
        // Animate hover effect
        if (isHovered) {
            hoverAnimation = std::min(1.0f, hoverAnimation + dt * 3.0f);
            glowIntensity = std::sin(dt * 3.0f) * 0.3f + 0.7f;
        } else {
            hoverAnimation = std::max(0.0f, hoverAnimation - dt * 3.0f);
            glowIntensity = 0;
        }
    }
    
    void draw(sf::RenderWindow& window, sf::Font& font, float time) {
        // Draw glow effect when hovered
        if (hoverAnimation > 0) {
            sf::RectangleShape glow(size + sf::Vector2f(10, 10));
            glow.setPosition(position - sf::Vector2f(5, 5));
            sf::Color gc = glowColor;
            gc.a = static_cast<sf::Uint8>(gc.a * hoverAnimation * glowIntensity);
            glow.setFillColor(gc);
            window.draw(glow);
        }
        
        // Draw tab background with gradient effect
        sf::RectangleShape tab(size);
        tab.setPosition(position);
        
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
        tab.setOutlineThickness(2);
        tab.setOutlineColor(sf::Color(100, 150, 255, static_cast<sf::Uint8>(100 + hoverAnimation * 155)));
        window.draw(tab);
        
        // Draw text
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
        
        // Background panel
        sf::RectangleShape panel(size);
        panel.setPosition(position);
        panel.setFillColor(sf::Color(20, 20, 40, 200));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(sf::Color(100, 150, 255, 150));
        window.draw(panel);
        
        // Title
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(24);
        title.setFillColor(sf::Color(200, 220, 255));
        
        // Content
        sf::Text content;
        content.setFont(font);
        content.setCharacterSize(16);
        content.setFillColor(sf::Color(180, 200, 240));
        
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
        
        title.setString(titleStr);
        title.setPosition(position.x + 20, position.y + 20);
        window.draw(title);
        
        content.setString(contentStr);
        content.setPosition(position.x + 20, position.y + 60);
        
        // Fade in animation
        float alpha = std::min(1.0f, animationTime * 2.0f);
        sf::Color c = content.getFillColor();
        c.a = static_cast<sf::Uint8>(c.a * alpha);
        content.setFillColor(c);
        
        window.draw(content);
        
        // Animated decorative stars
        for (int i = 0; i < 5; i++) {
            float angle = animationTime * 0.5f + i * 1.2566f; // 2*pi/5
            float radius = 30 + std::sin(animationTime * 2 + i) * 10;
            sf::CircleShape star(3);
            star.setPosition(
                position.x + size.x - 60 + std::cos(angle) * radius,
                position.y + 60 + std::sin(angle) * radius
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
    
    // Load font (using default SFML font fallback)
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Try alternative paths
        if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
                // Use system default if available
                return 1;
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
    
    for (int i = 0; i < 200; i++) {
        sf::CircleShape star(sizeDist(rng));
        star.setPosition(xDist(rng), yDist(rng));
        star.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(100 + rand() % 156)));
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
            tab.update(dt, mousePos);
        }
        featureDisplay.update(dt);
        particles.update(dt);
        
        // Emit random particles occasionally
        if (particleClock.getElapsedTime().asSeconds() > 0.1f) {
            for (auto& tab : tabs) {
                if (tab.getActive()) {
                    sf::Vector2f tabPos(30 + tabWidth / 2, 100 + (&tab - &tabs[0]) * (tabHeight + spacing) + tabHeight / 2);
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
        
        // Draw title
        sf::Text title;
        title.setFont(font);
        title.setString("✨ EUPHORIC SPACY MAGICAL MENU ✨");
        title.setCharacterSize(32);
        title.setFillColor(sf::Color(200, 220, 255, static_cast<sf::Uint8>(200 + std::sin(time * 2) * 55)));
        title.setPosition(250, 20);
        window.draw(title);
        
        window.display();
    }
    
    return 0;
}
