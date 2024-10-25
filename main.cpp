#include <SFML/Graphics.hpp>


// Fonction pour afficher le menu de pause
void displayPauseMenu(sf::RenderWindow &window, sf::Font &font) {
    // Créer le texte pour le menu de pause
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setString("Pause\nAppuyez sur Espace pour reprendre");
    pauseText.setCharacterSize(30);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(50, 200);

    // Afficher le texte du menu de pause
    window.clear(sf::Color::Black);
    window.draw(pauseText);
    window.display();
}

struct SnakeSegment {
    sf::RectangleShape shape;

    SnakeSegment(float x, float y, sf::Color color) {
        shape.setSize(sf::Vector2f(20, 20));
        shape.setFillColor(color);  // Couleur spécifique pour le segment
        shape.setPosition(x, y);
    }
};

class Snake {
public:
    std::vector<SnakeSegment> segments;
    sf::Vector2f direction;
    int score;

    Snake() {
        // Ajouter 3 segments pour le serpent initial
        segments.push_back(SnakeSegment(200, 200, sf::Color::Red));  // Tête du serpent
        segments.push_back(SnakeSegment(180, 200, sf::Color::Green));  // Premier segment
        segments.push_back(SnakeSegment(160, 200, sf::Color::Green));  // Deuxième segment

        // Direction initiale du serpent (se déplace vers la droite)
        direction = sf::Vector2f(20, 0);
        score = 0; // Initialise le score à 0
    }

    // Fonction pour déplacer le serpent
    void move() {
        // Déplacer chaque segment vers la position du précédent (à partir de la queue)
        for (size_t i = segments.size() - 1; i > 0; --i) {
            segments[i].shape.setPosition(segments[i - 1].shape.getPosition());
        }

        // Déplacer la tête dans la direction actuelle
        segments[0].shape.move(direction);
    }

    // Fonction pour changer la direction du serpent
    void setDirection(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Up && direction.y == 0) {
            direction = sf::Vector2f(0, -20); // Monter
        }
        else if (key == sf::Keyboard::Down && direction.y == 0) {
            direction = sf::Vector2f(0, 20); // Descendre
        }
        else if (key == sf::Keyboard::Left && direction.x == 0) {
            direction = sf::Vector2f(-20, 0); // Aller à gauche
        }
        else if (key == sf::Keyboard::Right && direction.x == 0) {
            direction = sf::Vector2f(20, 0); // Aller à droite
        }
    }

    // Fonction pour dessiner le serpent sur la fenêtre
    void draw(sf::RenderWindow &window) {
        for (auto &segment : segments) {
            window.draw(segment.shape);
        }
    }

    // Ajouter un segment au serpent (quand il mange une pomme)
    void grow() {
        // Obtenir la position de la dernière partie du serpent
        sf::Vector2f lastSegmentPosition = segments.back().shape.getPosition();
        segments.push_back(SnakeSegment(lastSegmentPosition.x, lastSegmentPosition.y, sf::Color::Green));
        score++;
    }

    // Vérifier si la tête du serpent touche la pomme
    bool checkCollisionWithApple(const sf::CircleShape &apple) {
        sf::Vector2f headPosition = segments[0].shape.getPosition();
        sf::Vector2f applePosition = apple.getPosition();

        // Vérifier la collision avec la pomme (basée sur la distance)
        if (headPosition.x == applePosition.x && headPosition.y == applePosition.y) {
            return true;
        }
        return false;
    }

    // Fonction pour vérifier si le serpent a touché les bords de la fenêtre
    bool checkCollisionWithWindow(const sf::RenderWindow &window) {
        sf::Vector2f headPosition = segments[0].shape.getPosition();
        float windowWidth = window.getSize().x;
        float windowHeight = window.getSize().y;

        // Vérifier si la tête dépasse les limites de la fenêtre
        if (headPosition.x < 0 || headPosition.x + 20 > windowWidth ||
            headPosition.y < 0 || headPosition.y + 20 > windowHeight) {
            return true;
        }
        return false;
    }
    
        // Nouvelle fonction pour vérifier si le serpent se touche lui-même
    bool checkSelfCollision() {
        sf::Vector2f headPosition = segments[0].shape.getPosition();

        // Vérifie si la tête du serpent entre en collision avec l'un de ses segments
        for (size_t i = 1; i < segments.size(); ++i) {
            if (segments[i].shape.getPosition() == headPosition) {
                return true;
            }
        }
        return false;
    }

    // Fonction pour obtenir le score
    int getScore() {
        return score;
    }
};

int main() {
    // Taille de la fenêtre
    const int windowWidth = 600;
    const int windowHeight = 600;

    // Taille d'une case du damier
    const int tileSize = 20; // Par exemple, un carré de 75x75 pixels

    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SNAKE POUR LES NULS");

    Snake snake; // Déclaration du snake
    
    // Limiter le framerate pour que le jeu ne soit pas trop rapide
    window.setFramerateLimit(10);

    // Initialiser la police pour le score
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {  // Assurez-vous que le fichier de police est dans le même répertoire que votre exécutable
        return -1;  // Gérer l'erreur si la police ne peut pas être chargée
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24); // Taille du texte
    scoreText.setFillColor(sf::Color::White); // Couleur du texte
    scoreText.setPosition(10, 10); // Position du texte


    // Créer une pomme
    sf::CircleShape apple(10.f);  // Taille de la pomme (rayon 10 pixels)
    apple.setFillColor(sf::Color::Red);  // Couleur rouge pour la pomme
    apple.setPosition(std::rand() % 20 * 20, std::rand() % 20 * 20); // Position initiale aléatoire

    // Variable pour gérer la pause
    bool isPaused = false;

    // Boucle principale
    while (window.isOpen()) {
        // Gestion des événements (fermer la fenêtre)
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Détecter les touches de direction pour changer la direction du serpent
            if (event.type == sf::Event::KeyPressed) {
                snake.setDirection(event.key.code);
            }
        
            // Détecter la touche "Espace" pour basculer le mode pause
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isPaused = !isPaused;  // Inverser l'état de la pause
            }

            // Détecter les touches de direction seulement si le jeu n'est pas en pause
            if (!isPaused && event.type == sf::Event::KeyPressed) {
                snake.setDirection(event.key.code);
            }

        }

                // Si le jeu est en pause, afficher le menu de pause
        if (isPaused) {
            displayPauseMenu(window, font);
            continue;  // Passer à la prochaine itération sans exécuter le reste de la boucle
        }
        // Déplacer le serpent
        snake.move();

        // Vérifier si le serpent touche la pomme
        if (snake.checkCollisionWithApple(apple)) {
            // Ajouter un segment au serpent
            snake.grow();

            // Générer une nouvelle position aléatoire pour la pomme
            apple.setPosition(std::rand() % 20 * 20, std::rand() % 20 * 20);
        }

        // Effacer la fenêtre avant de dessiner
        window.clear();
        
        // Dessiner le damier
        for (int i = 0; i < windowWidth / tileSize; i++) {
            for (int j = 0; j < windowHeight / tileSize; j++) {
                sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));

                // Alterner les couleurs
                if ((i + j) % 2 == 0)
                    tile.setFillColor(sf::Color(150, 150, 150));  // Gris clair
                else
                    tile.setFillColor(sf::Color(50, 50, 50));    // Gris foncé

                // Placer le carré à la bonne position
                tile.setPosition(i * tileSize, j * tileSize);

                // Dessiner le carré dans la fenêtre
                window.draw(tile);
            }
        }

        // Vérifier si le serpent a touché un bord de la fenêtre
        if (snake.checkCollisionWithWindow(window) || snake.checkSelfCollision()) {
            
            // Fermer la fenêtre si le serpent touche les bords (tu peux gérer autrement)
            window.close();
        }


        // Mettre à jour et afficher le score
        scoreText.setString("Score: " + std::to_string(snake.getScore()));
        window.draw(scoreText);
        
        // Dessiner le serpent
        snake.draw(window);
        // Dessiner la pomme
        window.draw(apple);
        // Afficher le résultat
        window.display();
    }

    return 0;
}