#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>

const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Segment {
    int x, y;
    Segment(int x, int y) : x(x), y(y) {}
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<Segment> snake;
    Direction dir;
    sf::RectangleShape food;
    sf::SoundBuffer eatBuffer;
    sf::Sound eatSound;
    int score;
    float speed;
    sf::Font font;
    sf::Text scoreText;

public:
    SnakeGame() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game"), dir(RIGHT), score(0), speed(0.1f) {
        window.setFramerateLimit(60);
        snake.push_back(Segment(WIDTH / 2, HEIGHT / 2));
        spawnFood();

        eatBuffer.loadFromFile("eat.wav");
        eatSound.setBuffer(eatBuffer);

        font.loadFromFile("arial.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
    }

    void spawnFood() {
        int fx = (rand() % (WIDTH / SIZE)) * SIZE;
        int fy = (rand() % (HEIGHT / SIZE)) * SIZE;
        food.setSize(sf::Vector2f(SIZE, SIZE));
        food.setFillColor(sf::Color::Red);
        food.setPosition(fx, fy);
    }

    void run() {
        sf::Clock clock;
        float timer = 0;

        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;

            handleInput();

            if (timer > speed) {
                update();
                timer = 0;
            }

            render();
        }
    }

    void handleInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir != DOWN) dir = UP;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir != UP) dir = DOWN;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir != RIGHT) dir = LEFT;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir != LEFT) dir = RIGHT;
    }

    void update() {
        Segment head = snake.front();
        switch (dir) {
            case UP: head.y -= SIZE; break;
            case DOWN: head.y += SIZE; break;
            case LEFT: head.x -= SIZE; break;
            case RIGHT: head.x += SIZE; break;
        }

        // Wall collision
        if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
            window.close();
            return;
        }

        // Self collision
        for (auto& s : snake) {
            if (s.x == head.x && s.y == head.y) {
                window.close();
                return;
            }
        }

        snake.insert(snake.begin(), head);

        // Food collision
        if (head.x == food.getPosition().x && head.y == food.getPosition().y) {
            eatSound.play();
            score += 10;
            speed *= 0.95f; // Increase difficulty
            spawnFood();
        } else {
            snake.pop_back();
        }
    }

    void render() {
        window.clear();

        for (auto& s : snake) {
            sf::RectangleShape rect(sf::Vector2f(SIZE - 2, SIZE - 2));
            rect.setFillColor(sf::Color::Green);
            rect.setPosition(s.x, s.y);
            window.draw(rect);
        }

        window.draw(food);

        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    SnakeGame game;
    game.run();
    return 0;
}
