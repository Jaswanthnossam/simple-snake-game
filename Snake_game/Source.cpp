#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

class StartPage {
public:
    StartPage(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    bool handleInput(sf::RenderWindow& window);

private:
    sf::Text titleText;
    sf::Text startText;
    sf::Font font;
    sf::Texture backgroundTexture;  // Added texture for the background
    sf::Sprite backgroundSprite;    // Added sprite for the background

    void loadFont();
    void loadBackground();
};

StartPage::StartPage(sf::RenderWindow& window) {
    loadFont();
    loadBackground();

    titleText.setFont(font);
    titleText.setString("Snake Game");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 100);

    startText.setFont(font);
    startText.setString("Start");
    startText.setCharacterSize(30);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(window.getSize().x / 2 - startText.getLocalBounds().width / 2, 300);
}

void StartPage::loadFont() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        // Handle font loading failure, exit or use default font
    }
}

void StartPage::loadBackground() {
    if (!backgroundTexture.loadFromFile("background_start.png")) {
        std::cerr << "Failed to load background image" << std::endl;
        // Handle background loading failure, exit or use default background
    }
    backgroundSprite.setTexture(backgroundTexture);
}

void StartPage::render(sf::RenderWindow& window) {
    window.clear();

    // Draw background
    window.draw(backgroundSprite);

    window.draw(titleText);
    window.draw(startText);
    window.display();
}

bool StartPage::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check for mouse click on the start button
                if (startText.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    return true;  // Start button clicked, transition to the game
                }
            }
        }
    }
    return false;  // No start button clicked yet
}



class Snake {
public:
    Snake();
    void update();
    void render(sf::RenderWindow& window);
    void handleInput();
    bool isGameOver();

private:
    sf::RectangleShape snakeHead;
    std::vector<sf::RectangleShape> snakeBody;
    sf::Vector2f direction;
    bool gameOver;

    sf::RectangleShape food;
    int score;

    sf::SoundBuffer eatBuffer;
    sf::Sound eatSound;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    void spawnFood();
};

Snake::Snake() {
    snakeHead.setSize(sf::Vector2f(20, 20));
    snakeHead.setFillColor(sf::Color::Green);
    snakeHead.setPosition(100, 100);

    direction = sf::Vector2f(1, 0);
    gameOver = false;

    food.setSize(sf::Vector2f(20, 20));
    food.setFillColor(sf::Color::Red);

    score = 0;
    spawnFood();

    // Load sound effect
    if (!eatBuffer.loadFromFile("eat_sound.wav")) {  // Provide the correct path to your sound file
        std::cerr << "Failed to load sound effect" << std::endl;
    }

    eatSound.setBuffer(eatBuffer);

    // Load background image
    if (!backgroundTexture.loadFromFile("background.png")) {  // Provide the correct path to your image file
        std::cerr << "Failed to load background image" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);
}

void Snake::update() {
    if (!gameOver) {
        sf::Vector2f prevPos = snakeHead.getPosition();

        // Move the head in the current direction
        snakeHead.move(direction * 20.0f);

        // Move the body segments
        for (size_t i = 0; i < snakeBody.size(); ++i) {
            sf::Vector2f temp = snakeBody[i].getPosition();
            snakeBody[i].setPosition(prevPos);
            prevPos = temp;
        }

        // Check for collision with body
        for (const auto& segment : snakeBody) {
            if (snakeHead.getPosition() == segment.getPosition()) {
                gameOver = true;
            }
        }

        // Check for collision with walls
        if (snakeHead.getPosition().x < 0 || snakeHead.getPosition().x >= 800 ||
            snakeHead.getPosition().y < 0 || snakeHead.getPosition().y >= 600) {
            gameOver = true;
        }

        // Check for collision with food
        if (snakeHead.getGlobalBounds().intersects(food.getGlobalBounds())) {
            score++;
            snakeBody.push_back(sf::RectangleShape(sf::Vector2f(20, 20)));
            spawnFood();
            eatSound.play();  // Play sound effect when eating food
        }
    }
}

void Snake::render(sf::RenderWindow& window) {
    // Draw background
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
        static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
    );
    window.draw(backgroundSprite);

    // Draw snake
    window.draw(snakeHead);
    for (const auto& segment : snakeBody) {
        window.draw(segment);
    }
    window.draw(food);

    // Display score
    sf::Font font;
    font.loadFromFile("arial.ttf");  // You may need to provide the correct path to a font file
    sf::Text scoreText("Score: " + std::to_string(score), font, 20);
    scoreText.setPosition(10, 10);
    window.draw(scoreText);
}

void Snake::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && direction.y == 0) {
        direction = sf::Vector2f(0, -1);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && direction.y == 0) {
        direction = sf::Vector2f(0, 1);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && direction.x == 0) {
        direction = sf::Vector2f(-1, 0);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && direction.x == 0) {
        direction = sf::Vector2f(1, 0);
    }
}

bool Snake::isGameOver() {
    return gameOver;
}

void Snake::spawnFood() {
    // Generate random position for the food
    int x = rand() % 39 + 1;  // Random x position between 1 and 39
    int y = rand() % 29 + 1;  // Random y position between 1 and 29

    food.setPosition(x * 20, y * 20);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Snake Game");

    StartPage startPage(window);
    Snake snake;

    sf::Clock clock;
    float deltaTime = 0.0f;
    bool gameStarted = false;

    while (window.isOpen()) {
        if (!gameStarted) {
            startPage.render(window);
            if (startPage.handleInput(window)) {
                gameStarted = true;
            }
        }
        else {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            deltaTime = clock.restart().asSeconds();

            snake.handleInput();
            snake.update();

            window.clear();
            snake.render(window);
            window.display();

            if (snake.isGameOver()) {
                std::cout << "Game Over! Score: " << snake.isGameOver() << std::endl;
                break;
            }

            sf::sleep(sf::milliseconds(100)); // Adjust the speed of the game
        }
    }

    return 0;
}
