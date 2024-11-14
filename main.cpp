// Crée par Florentin (Kanoox)
// .©2024

#include <SFML/Graphics.hpp>
#include <iostream>

// Fonction pour afficher le menu de pause
void displayPauseMenu(sf::RenderWindow &window, sf::Font &font) {
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setString("Pause\nAppuyez sur Espace pour reprendre");
    pauseText.setCharacterSize(30);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(50, 200);

    window.clear(sf::Color::Black);
    window.draw(pauseText);
    window.display();
}

struct SnakeSegment {
    sf::RectangleShape shape;

    SnakeSegment(float x, float y, sf::Color color) {
        shape.setSize(sf::Vector2f(20, 20));
        shape.setFillColor(color);
        shape.setPosition(x, y);
    }
};

class Snake {
public:
    std::vector<SnakeSegment> segments;
    sf::Vector2f direction;
    int score;

    Snake() {
        segments.push_back(SnakeSegment(200, 200, sf::Color::Yellow));  // Tête du serpent
        segments.push_back(SnakeSegment(180, 200, sf::Color::Green));   // Premier segment
        segments.push_back(SnakeSegment(160, 200, sf::Color::Green));   // Deuxième segment
        direction = sf::Vector2f(20, 0);
        score = 0;
    }

    void move() {
        for (size_t i = segments.size() - 1; i > 0; --i) {
            segments[i].shape.setPosition(segments[i - 1].shape.getPosition());
        }
        segments[0].shape.move(direction);
    }

    void setDirection(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Up && direction.y == 0) {
            direction = sf::Vector2f(0, -20);
        } else if (key == sf::Keyboard::Down && direction.y == 0) {
            direction = sf::Vector2f(0, 20);
        } else if (key == sf::Keyboard::Left && direction.x == 0) {
            direction = sf::Vector2f(-20, 0);
        } else if (key == sf::Keyboard::Right && direction.x == 0) {
            direction = sf::Vector2f(20, 0);
        }
    }

    void draw(sf::RenderWindow &window) {
        segments[0].shape.setFillColor(sf::Color::Yellow);
        window.draw(segments[0].shape);

        for (size_t i = 1; i < segments.size(); ++i) {
            segments[i].shape.setFillColor(sf::Color::Green);
            window.draw(segments[i].shape);
        }
    }

    void grow() {
        sf::Vector2f lastSegmentPosition = segments.back().shape.getPosition();
        segments.push_back(SnakeSegment(lastSegmentPosition.x, lastSegmentPosition.y, sf::Color::Green));
        score++;
    }

    bool checkCollisionWithApple(const sf::CircleShape &apple) {
        sf::Vector2f headPosition = segments[0].shape.getPosition();
        sf::Vector2f applePosition = apple.getPosition();
        return (headPosition.x == applePosition.x && headPosition.y == applePosition.y);
    }

    bool checkCollisionWithWindow(const sf::RenderWindow &window) {
        sf::Vector2f headPosition = segments[0].shape.getPosition();
        float windowWidth = window.getSize().x - 80;
        float windowHeight = window.getSize().y - 80;
        //std::cout << headPosition.x << std::endl; // Test de print pour voir ou le serpent se situer en direct
        //std::cout << headPosition.y << std::endl; // Test de print pour voir ou le serpent se situer en direct
        //std::cout << windowHeight << std::endl;
        return (headPosition.x < 60 || headPosition.x >= windowWidth ||
        headPosition.y < 60 || headPosition.y >= windowHeight);
    }

    bool checkSelfCollision() {
        sf::Vector2f headPosition = segments[0].shape.getPosition();
        for (size_t i = 1; i < segments.size(); ++i) {
            if (segments[i].shape.getPosition() == headPosition) {
                return true;
            }
        }
        return false;
    }

    int getScore() {
        return score;
    }
};

int generateRandomPosition(int min, int max) {
    // Diviser les limites par 20 pour obtenir des valeurs multiples de 20
    int adjustedMin = min / 20;
    int adjustedMax = max / 20;
    
    // Générer une valeur aléatoire dans cette plage ajustée, puis la multiplier par 20
    return (adjustedMin + std::rand() % (adjustedMax - adjustedMin + 1)) * 20;
}

// Fonction pour générer une position aléatoire pour x et y dans la plage [0, 800] pour x et [0, 600] pour y
void regeneratePosition(int &x, int &y) {
    x = std::rand() % 20 * 20;
    y = std::rand() % 20 * 20;
}
int main() {
    const int windowWidth = 800;
    const int windowHeight = 800;
    const int tileSize = 20;
    const int border = 20 * 3;
    const int borderThickness = border;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SNAKE POUR LES NULS");
    Snake snake;
    window.setFramerateLimit(10);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(26);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 20);

    sf::CircleShape apple(10.f);
    apple.setFillColor(sf::Color::Red);
    apple.setPosition(std::rand() % 20 * 20, std::rand() % 20 * 20);

    bool isPaused = false;

    // Création des rectangles pour les bordures
    sf::RectangleShape topBorder(sf::Vector2f(windowWidth, borderThickness));
    topBorder.setFillColor(sf::Color::Black);
    topBorder.setPosition(0, 0);

    sf::RectangleShape bottomBorder(sf::Vector2f(windowWidth, borderThickness));
    bottomBorder.setFillColor(sf::Color::Black);
    bottomBorder.setPosition(0, windowHeight - borderThickness);

    sf::RectangleShape leftBorder(sf::Vector2f(borderThickness, windowHeight));
    leftBorder.setFillColor(sf::Color::Black);
    leftBorder.setPosition(0, 0);

    sf::RectangleShape rightBorder(sf::Vector2f(borderThickness, windowHeight));
    rightBorder.setFillColor(sf::Color::Black);
    rightBorder.setPosition(windowWidth - borderThickness, 0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                snake.setDirection(event.key.code);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isPaused = !isPaused;
            }

            if (!isPaused && event.type == sf::Event::KeyPressed) {
                snake.setDirection(event.key.code);
            }
        }

        if (isPaused) {
            displayPauseMenu(window, font);
            continue;
        }

        snake.move();

        if (snake.checkCollisionWithApple(apple)) {
            snake.grow();
            int x, y;
            std::cout << "x = " << x << " y = " << y << std::endl;

            // Générer des positions jusqu'à ce que la condition soit respectée
            do {
                regeneratePosition(x, y);
            } while (!(x > 60 && y < 720 && y >= 60 && x <= 720));

            apple.setPosition(x, y);
        }

        window.clear();

        // Dessiner le damier
        for (int i = 0; i < windowWidth / tileSize; i++) {
            for (int j = 0; j < windowHeight / tileSize; j++) {
                sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
                if ((i + j) % 2 == 0)
                    tile.setFillColor(sf::Color(150, 150, 150));
                else
                    tile.setFillColor(sf::Color(50, 50, 50));
                tile.setPosition(i * tileSize, j * tileSize);
                window.draw(tile);
            }
        }

        if (snake.checkCollisionWithWindow(window) || snake.checkSelfCollision()) {
            window.close();
        }

        // Dessiner les bordures autour du damier
        window.draw(topBorder);
        window.draw(bottomBorder);
        window.draw(leftBorder);
        window.draw(rightBorder);

        scoreText.setString("Score: " + std::to_string(snake.getScore()));
        window.draw(scoreText);
        snake.draw(window);
        window.draw(apple);
        window.display();
    }

    return 0;
}
