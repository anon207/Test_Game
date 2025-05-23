#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <optional>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>

// PRE:
// POST:
static void moveToTarget(
    float pos, 
    float targetPos, 
    sf::RenderWindow& window, 
    sf::RectangleShape& evilRectangle, 
    float speed
) {
    if (pos <= targetPos) {
        float bottomLimit = window.getSize().y - evilRectangle.getSize().y;
        if (evilRectangle.getPosition().y < bottomLimit) {
            evilRectangle.move({ 0, speed });
        }
    }
    else {
        float pos = evilRectangle.getPosition().y;
        if (pos > 0) {
            evilRectangle.move({ 0, -speed });
        }
    }
}

// PRE:
// POST:
static void moveToTargetRandom(
    float currentY,
    float targetY,
    sf::RenderWindow& window,
    sf::RectangleShape& evilRectangle,
    float speed,
    bool& isComplete
) {
    const float epsilon = 0.5f; // Acceptable margin of error
    float bottomLimit = window.getSize().y - evilRectangle.getSize().y;

    // Sanity check targetY
    if (std::isnan(targetY) || targetY < 0 || targetY > bottomLimit) {
        isComplete = true;
        return;
    }

    float newY = evilRectangle.getPosition().y;

    if (newY < targetY) {
        if (newY + speed <= bottomLimit) {
            evilRectangle.move({ 0, speed });
        }
    }
    else if (newY > targetY) {
        if (newY - speed >= 0.0f) {
            evilRectangle.move({ 0, -speed });
        }
    }

    newY = evilRectangle.getPosition().y;

    // Completion check with margin in case of error
    if (std::abs(newY - targetY) <= epsilon) {
        evilRectangle.setPosition({ evilRectangle.getPosition().x, targetY });
        isComplete = true;
    }
}

// PRE:
// POST:
static void sway(
    sf::RenderWindow& window, 
    sf::RectangleShape& evilRectangle, 
    float swayRange, 
    float originalY, 
    bool& swayUp, 
    float swaySpeed
) {
    float topLimit = 0.0f;
    float bottomLimit = window.getSize().y - evilRectangle.getSize().y;

    float maxUp = std::min(swayRange, originalY - topLimit);
    float maxDown = std::min(swayRange, bottomLimit - originalY);

    // Move the rectangle in the current sway direction
    if (swayUp) {
        evilRectangle.move({ 0, -swaySpeed });

        if (evilRectangle.getPosition().y <= originalY - maxUp) {
            // 50% chance to switch direction
            swayUp = (std::rand() % 2 == 0);
        }
    }
    else {
        evilRectangle.move({ 0, swaySpeed });

        if (evilRectangle.getPosition().y >= originalY + maxDown) {
            // 50% chance to switch direction
            swayUp = !(std::rand() % 2 == 0);
        }
    }
}

// PRE:
// POST:
static void executeRandomMovement(
    std::vector<float>& movementPattern,
    sf::RectangleShape& evilRectangle,
    bool& isComplete,
    float speed,
    sf::RenderWindow& window,
    bool& randomMovement
) {
    if (!movementPattern.empty()) {
        float currentY = evilRectangle.getPosition().y;
        float targetY = movementPattern.back();

        if (!isComplete) {
            speed = 0.08f;
            moveToTargetRandom(currentY, targetY, window, evilRectangle, speed, isComplete);
        }
        else {
            // ASSERT: random movement step finished
            movementPattern.pop_back();
            isComplete = false;
        }
    }
    else {
        // ASSERT: finished all movement steps in movement pattern
        randomMovement = false;
    }
}

// PRE:
// POST:
static void calculateTrajectory(
    sf::CircleShape& ball, 
    sf::Vector2f current, 
    sf::Vector2f upRight, 
    float rightSideOfScreen, 
    float topOfScreen, 
    sf::Vector2f downRight, 
    float bottomOfScreen, 
    sf::Vector2f& targetPosition
) {
    sf::Vector2f currentPosition = ball.getPosition();
    sf::Vector2f trajectory;

    if (current == upRight) {
        trajectory = upRight;
        while (currentPosition.x <= rightSideOfScreen) {
            currentPosition += trajectory;
            if (currentPosition.y <= topOfScreen) {
                trajectory = downRight;
            }
            if (currentPosition.y >= bottomOfScreen) {
                trajectory = upRight;
            }
        }
    }

    if (current == downRight) {
        trajectory = downRight;
        while (currentPosition.x <= rightSideOfScreen) {
            currentPosition += trajectory;
            if (currentPosition.y <= topOfScreen) {
                trajectory = downRight;
            }
            if (currentPosition.y >= bottomOfScreen) {
                trajectory = upRight;
            }
        }
    }
    targetPosition = currentPosition;
}

// PRE:
// POST:
static void getRandomMovements(
    sf::RectangleShape evilRectangle, 
    std::vector<float>& movementPattern, 
    bool& randomMovement
) {
    
    // Evil rectangle movements
    float halfUp = evilRectangle.getPosition().y - (evilRectangle.getSize().y / 2);
    float halfDown = evilRectangle.getPosition().y + (evilRectangle.getSize().y / 2);
    float quarterUp = evilRectangle.getPosition().y - (evilRectangle.getSize().y / 4);
    float quarterDown = evilRectangle.getPosition().y + (evilRectangle.getSize().y / 4);
    float sixthUp = evilRectangle.getPosition().y - (evilRectangle.getSize().y / 6);
    float sixthDown = evilRectangle.getPosition().y + (evilRectangle.getSize().y / 6);

    // Evil rectangle 'random' movement groups
    std::vector<float> Up = { halfUp };
    std::vector<float> Down = { halfDown };
    std::vector<float> upUp = { quarterUp, quarterUp };
    std::vector<float> downDown = { quarterDown, quarterDown };
    std::vector<float> upDown = { quarterDown, quarterUp };
    std::vector<float> downUp = { quarterUp, quarterDown };
    std::vector<float> upDownDown = { sixthDown, sixthDown, sixthUp };
    std::vector<float> downUpUp = { sixthUp, sixthUp, sixthDown };

    // random movement vector
    std::vector<std::vector<float>> randomMovements = {
        Up, Down, upUp, downDown, upDown, downUp, upDownDown, downUpUp
    };

    int randomNumber = std::rand() % 8;

    movementPattern = randomMovements[randomNumber];

    randomMovement = true;
}

// PRE:
// POST:
static void increaseBallSpeed(
    float& masterSpeed, 
    sf::Vector2f& current, 
    sf::Vector2f& upLeft, 
    sf::Vector2f& upRight,
    sf::Vector2f& downLeft, 
    sf::Vector2f& downRight
) {
    masterSpeed += float((1.0 / 900.0) * pow(masterSpeed, 3.0));
    
    if (masterSpeed > 200.0f) masterSpeed = 200.0f;

    if (current == upLeft) {
        upLeft = { -masterSpeed, -masterSpeed };
        upRight = { masterSpeed, -masterSpeed };
        downLeft = { -masterSpeed, masterSpeed };
        downRight = { masterSpeed, masterSpeed };
        current = upLeft;
    }
    else if (current == upRight) {
        upLeft = { -masterSpeed, -masterSpeed };
        upRight = { masterSpeed, -masterSpeed };
        downLeft = { -masterSpeed, masterSpeed };
        downRight = { masterSpeed, masterSpeed };
        current = upRight;
    }
    else if (current == downLeft) {
        upLeft = { -masterSpeed, -masterSpeed };
        upRight = { masterSpeed, -masterSpeed };
        downLeft = { -masterSpeed, masterSpeed };
        downRight = { masterSpeed, masterSpeed };
        current = downLeft;
    }
    else {
        upLeft = { -masterSpeed, -masterSpeed };
        upRight = { masterSpeed, -masterSpeed };
        downLeft = { -masterSpeed, masterSpeed };
        downRight = { masterSpeed, masterSpeed };
        current = downRight;
    }
}

// PRE:
// POST:
static void decreaseRandomnessFactor(
    int& randInt,
    sf::RectangleShape evilRectangle,
    std::vector<float>& movementPattern,
    bool& randomMovement
) {
    // Each frame decide if a random movement should be executed 1/5000 chance
    randInt += int((static_cast<double>(1) / 900) * pow(randInt, 3));
    int randNum = std::rand() % randInt;

    if (randNum == 0 && randomMovement == false) {
        getRandomMovements(evilRectangle, movementPattern, randomMovement);
    }
}

// PRE:
// POST:
static void detectUpwardMovement(
    sf::RectangleShape& rectangle, 
    bool& isMoving, 
    float speed
) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        sf::Vector2f pos = rectangle.getPosition();
        if (pos.y > 0) {
            isMoving = true;
            rectangle.move({ 0, -speed });
        }
    }
}

// PRE:
// POST:
static void detectDownwardMovement(
    sf::RenderWindow& window,
    sf::RectangleShape& rectangle,
    bool& isMoving,
    float speed
) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        float bottomLimit = window.getSize().y - rectangle.getSize().y;
        if (rectangle.getPosition().y < bottomLimit) {
            isMoving = true;
            rectangle.move({ 0, speed });
        }
    }
}

// PRE:
// POST:
static void checkForCloseEvent(
    sf::RenderWindow& window
) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

static void handleEvilRectangleMovement(
    sf::Vector2f current, 
    sf::Vector2f upRight, 
    sf::Vector2f downRight,
    sf::CircleShape& ball, 
    float rightSideOfScreen, 
    float topOfScreen, 
    float bottomOfScreen, 
    sf::Vector2f& targetPosition,
    bool& randomMovement,
    std::vector<float>& movementPattern,
    sf::RectangleShape& evilRectangle,
    bool& isComplete, 
    float speed, 
    sf::RenderWindow& window,
    sf::Vector2f upLeft,
    sf::Vector2f downLeft,
    float swayRange,
    float originalY,
    bool& swayUp,
    float swaySpeed
) {
    // Calculate trajectory for evil rectangle to go to
    if (current == upRight || current == downRight) {
        // ASSERT: either ball is going upRight or downRight

        calculateTrajectory(ball, current, upRight, rightSideOfScreen, topOfScreen, downRight, bottomOfScreen, targetPosition);
    }

    if (current == upRight || current == downRight) {
        if (randomMovement) {
            executeRandomMovement(movementPattern, evilRectangle, isComplete, speed, window, randomMovement);
        }
        else {
            float pos = evilRectangle.getPosition().y;
            float targetPos = targetPosition.y - (evilRectangle.getSize().y / 2);
            moveToTarget(pos, targetPos, window, evilRectangle, speed);
        }
    }

    if (current == upLeft || current == downLeft) {
        sway(window, evilRectangle, swayRange, originalY, swayUp, swaySpeed);
    }
}

static void checkCollisionCases(
    float bottomOfBall, 
    float bottomOfScreen,
    sf::Vector2f& current,
    sf::Vector2f downRight,
    sf::Vector2f upRight,
    sf::Vector2f downLeft,
    sf::Vector2f upLeft,
    float topOfBall,
    float topOfScreen,
    sf::Sound& boundaryHit
) {
    // collision cases
    if (bottomOfBall >= bottomOfScreen && current == downRight) {
        boundaryHit.play();
        current = upRight;
    }

    if (bottomOfBall >= bottomOfScreen && current == downLeft) {
        boundaryHit.play();
        current = upLeft;
    }

    if (topOfBall <= topOfScreen && current == upLeft) {
        boundaryHit.play();
        current = downLeft;
    }
    if (topOfBall <= topOfScreen && current == upRight) {
        boundaryHit.play();
        current = downRight;
    }
}

static void checkRectangleCollision(
    float leftSideOfBall, 
    float frontOfRectangle, 
    float bottomOfBall, 
    float bottomOfRectangle, 
    float topOfBall, 
    float topOfRectangle, 
    float leftSideOfScreen, 
    sf::Vector2f& current, 
    sf::Vector2f upLeft,
    sf::Vector2f upRight, 
    sf::Vector2f downLeft,
    sf::Vector2f downRight, 
    float backOfRectangle, 
    float rightSideOfBall, 
    bool isMoving, 
    float speed,
    sf::Sound& paddleHit
) {
    // Collision for front of users rectangle
    if (leftSideOfBall <= frontOfRectangle &&
        bottomOfBall <= bottomOfRectangle &&
        topOfBall >= topOfRectangle &&
        leftSideOfBall > leftSideOfScreen &&
        current == upLeft) {
        
        paddleHit.play();
        current = upRight;
    }

    if (leftSideOfBall <= frontOfRectangle &&
        bottomOfBall <= bottomOfRectangle &&
        topOfBall >= topOfRectangle &&
        leftSideOfBall > leftSideOfScreen &&
        current == downLeft) {
        
        paddleHit.play();
        current = downRight;
    }

    // collision for top of users rectangle
    if (leftSideOfBall >= backOfRectangle &&
        rightSideOfBall <= frontOfRectangle &&
        bottomOfBall >= topOfRectangle &&
        bottomOfBall <= bottomOfRectangle) {

        if (isMoving) {
            upLeft.y -= speed;
            downLeft.y += speed;
        }

        paddleHit.play();
        current = upLeft;
    }

    // collision for bottom of users rectangle
    if (leftSideOfBall >= backOfRectangle &&
        rightSideOfBall <= frontOfRectangle &&
        topOfBall <= bottomOfRectangle &&
        topOfBall >= topOfRectangle) {

        if (isMoving) {
            downLeft.y += speed;
            upLeft.y -= speed;
        }

        paddleHit.play();
        current = downLeft;
    }
}

static void checkEvilRectangleCollision(
    float rightSideOfBall, 
    float frontOfEvilRectangle, 
    float bottomOfBall, 
    float bottomOfEvilRectangle, 
    float topOfBall, 
    float topOfEvilRectangle, 
    float rightSideOfScreen, 
    sf::Vector2f& current, 
    sf::Vector2f upRight, 
    sf::Vector2f upLeft, 
    sf::Vector2f downRight, 
    sf::Vector2f downLeft, 
    float backOfEvilRectangle, 
    float leftSideOfBall, 
    float speed,
    sf::Sound& paddleHit
) {
    // Collision for front of evil rectangle
    if (rightSideOfBall >= frontOfEvilRectangle &&
        bottomOfBall <= bottomOfEvilRectangle &&
        topOfBall >= topOfEvilRectangle &&
        rightSideOfBall < rightSideOfScreen &&
        current == upRight) {

        paddleHit.play();
        current = upLeft;
    }

    if (rightSideOfBall >= frontOfEvilRectangle &&
        bottomOfBall <= bottomOfEvilRectangle &&
        topOfBall >= topOfEvilRectangle &&
        rightSideOfBall < rightSideOfScreen &&
        current == downRight) {

        paddleHit.play();
        current = downLeft;
    }

    // collision for top of evil rectangle
    if (rightSideOfBall <= backOfEvilRectangle &&
        leftSideOfBall >= frontOfEvilRectangle &&
        bottomOfBall >= topOfEvilRectangle &&
        bottomOfBall <= bottomOfEvilRectangle) {

        upRight.y -= speed;
        downRight.y += speed;

        paddleHit.play();
        current = upRight;
    }

    // collision for bottom of evil rectangle
    if (rightSideOfBall <= backOfEvilRectangle &&
        leftSideOfBall >= frontOfEvilRectangle &&
        topOfBall <= bottomOfEvilRectangle &&
        topOfBall >= topOfEvilRectangle) {

        downRight.y += speed;
        upRight.y -= speed;

        paddleHit.play();
        current = downRight;
    }
}

static void updateBallBounds(
    float& bottomOfBall,
    sf::CircleShape ball,
    float& rightSideOfBall,
    float& topOfBall,
    float& leftSideOfBall
) {
    // ball bounds
    bottomOfBall = ball.getPosition().y + ball.getRadius();
    rightSideOfBall = ball.getPosition().x + ball.getRadius();
    topOfBall = ball.getPosition().y - ball.getRadius();
    leftSideOfBall = ball.getPosition().x - ball.getRadius();
}

static void updateRectangleBounds(
    float& frontOfRectangle, 
    sf::RectangleShape rectangle, 
    float& backOfRectangle,
    float& topOfRectangle,
    float& bottomOfRectangle
) {
    // rectangle bounds
    frontOfRectangle = rectangle.getSize().x;
    backOfRectangle = rectangle.getPosition().x;
    topOfRectangle = rectangle.getPosition().y;
    bottomOfRectangle = rectangle.getPosition().y + rectangle.getSize().y;
}

static void updateEvilRectangleBounds(
    float& frontOfEvilRectangle, 
    sf::RectangleShape evilRectangle, 
    float& backOfEvilRectangle,
    float& topOfEvilRectangle,
    float& bottomOfEvilRectangle
) {
    frontOfEvilRectangle = evilRectangle.getPosition().x;
    backOfEvilRectangle = evilRectangle.getPosition().x + evilRectangle.getSize().x;
    topOfEvilRectangle = evilRectangle.getPosition().y;
    bottomOfEvilRectangle = evilRectangle.getPosition().y + evilRectangle.getSize().y;
}

static void resetBall(
    sf::CircleShape& ball, 
    sf::Vector2u windowSize, 
    float& masterSpeed
) {
    // reset ball
    ball.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f });

    // reset ball speed
    masterSpeed = .05f;
}

static void resetTrajectories(
    sf::Vector2f& upLeft, 
    float masterSpeed, 
    sf::Vector2f& upRight, 
    sf::Vector2f& downLeft, 
    sf::Vector2f& downRight, 
    sf::Vector2f& current
) {
    // reset trajectories
    upLeft = { -masterSpeed, -masterSpeed };
    upRight = { masterSpeed, -masterSpeed };
    downLeft = { -masterSpeed, masterSpeed };
    downRight = { masterSpeed, masterSpeed };

    // pick new random starting trajectory
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3);

    std::vector<sf::Vector2f> trajectories = {
        upLeft,
        upRight,
        downLeft,
        downRight
    };

    current = trajectories[dist(gen)];
}

static void handleScore(
    float rightSideOfBall, 
    float leftSideOfScreen, 
    float leftSideOfBall,
    float rightSideOfScreen,
    int& enemyScore, 
    int& userScore,
    sf::CircleShape& ball,
    sf::Vector2u windowSize, 
    float& masterSpeed, 
    sf::Vector2f& upLeft,
    sf::Vector2f& upRight,
    sf::Vector2f& downLeft,
    sf::Vector2f& downRight,
    sf::Vector2f& current,
    bool& goalScored
    ) {

    if (rightSideOfBall < leftSideOfScreen) {
        enemyScore += 1;
        goalScored = true;
    }

    if (leftSideOfBall > rightSideOfScreen) {
        userScore += 1;
        goalScored = true;
    }

    if (userScore < 5 && enemyScore < 5 && goalScored) {
        resetBall(ball, windowSize, masterSpeed);

        resetTrajectories(
            upLeft,
            masterSpeed,
            upRight,
            downLeft,
            downRight,
            current
        );
    }
    else {

        if (userScore == 5) {
            std::cout << "User wins" << std::endl;
        }

        if (enemyScore == 5) {
            std::cout << "Enemy wins" << std::endl;
        }
    }
}

static void startCountdown(
    bool& goalScored, 
    sf::Text CountdownText, 
    sf::RenderWindow& window, 
    sf::CircleShape& ball, 
    sf:: RectangleShape& rectangle, 
    sf::RectangleShape& evilRectangle, 
    sf::Text userScoreText,
    sf::Text enemyScoreText
) {
    if (goalScored) {

        for (int i = 3; i > 0; i--) {
            CountdownText.setString(std::to_string(i));

            window.clear(sf::Color::Black);

            window.draw(CountdownText);
            window.draw(ball);
            window.draw(rectangle);
            window.draw(evilRectangle);
            window.draw(userScoreText);
            window.draw(enemyScoreText);

            window.display();
            Sleep(500);
        }

        goalScored = false;
    }
}

//int wWinMain(
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR lpCmdLine,
//    _In_ int nShowCmd
//) {
int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Ultimate pong");

    //window.setFramerateLimit(2000);

    sf::Vector2u windowSize = window.getSize();

    std::srand(std::time(0));

    // Users rectangle
    sf::RectangleShape rectangle({});
    rectangle.setSize(sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.2f));
    rectangle.setFillColor(sf::Color(100, 250, 50));
    rectangle.setPosition({ 0, (windowSize.y / 2) - (rectangle.getSize().y / 2)});

    // Evil rectangle
    sf::RectangleShape evilRectangle({});
    evilRectangle.setSize(sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.2f));
    evilRectangle.setFillColor(sf::Color(100, 250, 50));
    evilRectangle.setPosition({ windowSize.x - evilRectangle.getSize().x, (windowSize.y / 2) - (evilRectangle.getSize().y / 2) });

    // ball
    sf::CircleShape ball(windowSize.x * 0.01f);
    ball.setOrigin({ ball.getRadius(), ball.getRadius() });
    ball.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f });

    // global ball speed
    float masterSpeed = .05f;

    // initilize random starting trajectory
    sf::Vector2f upLeft = { -masterSpeed, -masterSpeed };
    sf::Vector2f upRight = { masterSpeed, -masterSpeed };
    sf::Vector2f downLeft = { -masterSpeed, masterSpeed };
    sf::Vector2f downRight = { masterSpeed, masterSpeed };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3);

    std::vector<sf::Vector2f> trajectories = {
        upLeft,
        upRight,
        downLeft,
        downRight
    };

    sf::Vector2f current = trajectories[dist(gen)];

    // screen bounds
    float bottomOfScreen = window.getSize().y;
    float rightSideOfScreen = window.getSize().x;
    float topOfScreen = 0;
    float leftSideOfScreen = 0;

    // bools
    bool isMoving = false;
    bool randomMovement = false;
    bool isComplete = false;
    bool swayUp = false;
    bool goalScored = false;

    // floats
    float speed = .1f;
    float swaySpeed = .1f;
    float swayRange = 480.0f;
    float originalY = evilRectangle.getPosition().y;
    float bottomOfBall = 0;
    float rightSideOfBall = 0;
    float topOfBall = 0;
    float leftSideOfBall = 0;
    float frontOfRectangle = 0;
    float backOfRectangle = 0;
    float topOfRectangle = 0;
    float bottomOfRectangle = 0;
    float frontOfEvilRectangle = 0;
    float backOfEvilRectangle = 0;
    float topOfEvilRectangle = 0;
    float bottomOfEvilRectangle = 0;

    // ints
    int randInt = 5000;
    int userScore = 0;
    int enemyScore = 0;
    int gameState = 0; // 1 for player vs player, 2 for player vs bot, 3 for player vs player

    // movement pattern container
    std::vector<float> movementPattern;

    // Sound buffers
    sf::SoundBuffer boundaryHitBuffer;
    sf::SoundBuffer paddleHitBuffer;
    if (!boundaryHitBuffer.loadFromFile("sounds/boundary_Hit.wav") ||
        !paddleHitBuffer.loadFromFile("sounds/paddle_Hit.wav")) {
        std::cerr << "Failed to load sound\n";
    }

    // sounds
    sf::Sound boundaryHit(boundaryHitBuffer);
    sf::Sound paddleHit(paddleHitBuffer);

    // fonts
    sf::Font font("fonts/gameFont.ttf");

    // PVP BUTTON
    sf::RectangleShape pvpButton;
    pvpButton.setSize({ windowSize.x * 0.25f, windowSize.y * 0.15f });
    pvpButton.setFillColor(sf::Color(117, 153, 105));
    pvpButton.setPosition({ windowSize.x * 0.08f, windowSize.y * 0.7f });
    
    sf::Text pvpText(font, "Player vs Player", 24);
    sf::FloatRect pvpTextBounds = pvpText.getLocalBounds();
    pvpText.setOrigin(pvpTextBounds.position + pvpTextBounds.size / 2.f);
    sf::Vector2f pvpButtonCenter = pvpButton.getPosition() + pvpButton.getSize() / 2.f;
    pvpText.setPosition(pvpButtonCenter);

    // PVB BUTTON
    sf::RectangleShape pvbButton;
    pvbButton.setSize({ windowSize.x * 0.25f, windowSize.y * 0.15f });
    pvbButton.setFillColor(sf::Color(117, 153, 105));
    pvbButton.setPosition({ windowSize.x * 0.38f, windowSize.y * 0.7f });

    sf::Text pvbText(font, "Player vs Bot", 24);
    sf::FloatRect pvbTextBounds = pvbText.getLocalBounds();
    pvbText.setOrigin(pvbTextBounds.position + pvbTextBounds.size / 2.f);
    sf::Vector2f pvbButtonCenter = pvbButton.getPosition() + pvbButton.getSize() / 2.f;
    pvbText.setPosition(pvbButtonCenter);

    // BVB BUTTON
    sf::RectangleShape bvbButton;
    bvbButton.setSize({ windowSize.x * 0.25f, windowSize.y * 0.15f });
    bvbButton.setFillColor(sf::Color(117, 153, 105));
    bvbButton.setPosition({ windowSize.x * 0.68f, windowSize.y * 0.7f });

    sf::Text bvbText(font, "Bot vs Bot", 24);
    sf::FloatRect bvbTextBounds = bvbText.getLocalBounds();
    bvbText.setOrigin(bvbTextBounds.position + bvbTextBounds.size / 2.f);
    sf::Vector2f bvbButtonCenter = bvbButton.getPosition() + bvbButton.getSize() / 2.f;
    bvbText.setPosition(bvbButtonCenter);

    // User score text
    sf::Text userScoreText(font, "0", 50);
    sf::FloatRect userScoreTextBounds = userScoreText.getLocalBounds();
    userScoreText.setOrigin(userScoreTextBounds.position + userScoreTextBounds.size / 2.f);
    userScoreText.setPosition({windowSize.x * 0.4f, windowSize.y * 0.08f});

    // Enemy score text
    sf::Text enemyScoreText(font, "0", 50);
    sf::FloatRect enemyScoreTextBounds = enemyScoreText.getLocalBounds();
    enemyScoreText.setOrigin(enemyScoreTextBounds.position + enemyScoreTextBounds.size / 2.f);
    enemyScoreText.setPosition({ windowSize.x * 0.6f, windowSize.y * 0.08f });

    // Countdown text
    sf::Text CountdownText(font, "3", 50);
    sf::FloatRect CountdownTextBounds = CountdownText.getLocalBounds();
    CountdownText.setOrigin(CountdownTextBounds.position + CountdownTextBounds.size / 2.f);
    CountdownText.setPosition({ windowSize.x / 2.f, windowSize.y * 0.4f });


    // Clock
    sf::Clock clock;

    while (window.isOpen()) {

        
        

        if (gameState == 2) {
            
            checkForCloseEvent(window);

            startCountdown(
                goalScored,
                CountdownText,
                window,
                ball,
                rectangle,
                evilRectangle,
                userScoreText,
                enemyScoreText
            );

            handleScore(
                rightSideOfBall,
                leftSideOfScreen,
                leftSideOfBall,
                rightSideOfScreen,
                enemyScore,
                userScore,
                ball,
                windowSize,
                masterSpeed,
                upLeft,
                upRight,
                downLeft,
                downRight,
                current,
                goalScored
            );

            enemyScoreText.setString(std::to_string(enemyScore));
            userScoreText.setString(std::to_string(userScore));

            decreaseRandomnessFactor(randInt, evilRectangle, movementPattern, randomMovement);

            detectUpwardMovement(rectangle, isMoving, speed);

            detectDownwardMovement(window, rectangle, isMoving, speed);

            ball.move(current);

            updateBallBounds(
                bottomOfBall,
                ball,
                rightSideOfBall,
                topOfBall,
                leftSideOfBall
            );

            checkCollisionCases(
                bottomOfBall,
                bottomOfScreen,
                current,
                downRight,
                upRight,
                downLeft,
                upLeft,
                topOfBall,
                topOfScreen,
                boundaryHit
            );

            updateRectangleBounds(
                frontOfRectangle,
                rectangle,
                backOfRectangle,
                topOfRectangle,
                bottomOfRectangle
            );

            checkRectangleCollision(
                leftSideOfBall,
                frontOfRectangle,
                bottomOfBall,
                bottomOfRectangle,
                topOfBall,
                topOfRectangle,
                leftSideOfScreen,
                current,
                upLeft,
                upRight,
                downLeft,
                downRight,
                backOfRectangle,
                rightSideOfBall,
                isMoving,
                speed,
                paddleHit
            );

            isMoving = false;

            updateEvilRectangleBounds(
                frontOfEvilRectangle,
                evilRectangle,
                backOfEvilRectangle,
                topOfEvilRectangle,
                bottomOfEvilRectangle
            );

            checkEvilRectangleCollision(
                rightSideOfBall,
                frontOfEvilRectangle,
                bottomOfBall,
                bottomOfEvilRectangle,
                topOfBall,
                topOfEvilRectangle,
                rightSideOfScreen,
                current,
                upRight,
                upLeft,
                downRight,
                downLeft,
                backOfEvilRectangle,
                leftSideOfBall,
                speed,
                paddleHit
            );

            sf::Vector2f targetPosition = {};

            handleEvilRectangleMovement(
                current,
                upRight,
                downRight,
                ball,
                rightSideOfScreen,
                topOfScreen,
                bottomOfScreen,
                targetPosition,
                randomMovement,
                movementPattern,
                evilRectangle,
                isComplete,
                speed,
                window,
                upLeft,
                downLeft,
                swayRange,
                originalY,
                swayUp,
                swaySpeed
            );

            // ball speeds up (exponential function, slow at first fast later on)
            increaseBallSpeed(masterSpeed, current, upLeft, upRight, downLeft, downRight);


            window.clear(sf::Color::Black);

            window.draw(ball);
            window.draw(rectangle);
            window.draw(evilRectangle);
            window.draw(userScoreText);
            window.draw(enemyScoreText);

            window.display();
        }

        if (gameState == 0) {
            
            checkForCloseEvent(window);
            
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (pvpButton.getGlobalBounds().contains(mousePosF)) {
                    // Start Player vs Player game
                }
                else if (pvbButton.getGlobalBounds().contains(mousePosF)) {
                    // Start Player vs Bot game
                    gameState = 2;
                }
                else if (bvbButton.getGlobalBounds().contains(mousePosF)) {
                    // Start Bot vs Bot game
                }
            }


            window.clear(sf::Color::Black);

            window.draw(pvpButton);
            window.draw(pvpText);
            window.draw(pvbButton);
            window.draw(pvbText);
            window.draw(bvbButton);
            window.draw(bvbText);

            window.display();
        }
    }
}

