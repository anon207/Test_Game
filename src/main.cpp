#include <SFML/Graphics.hpp>
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

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Ultimate pong");

    //window.setFramerateLimit(1000);

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

    // bools
    bool isMoving = false;
    bool randomMovement = false;
    bool isComplete = false;
    bool swayUp = false;

    // floats
    float speed = .1f;
    float swaySpeed = .1f;
    float swayRange = 480.0f;
    float originalY = evilRectangle.getPosition().y;

    // ints
    int randInt = 5000;

    // movement pattern container
    std::vector<float> movementPattern;

    while (window.isOpen()) {

        checkForCloseEvent(window);

        decreaseRandomnessFactor(randInt, evilRectangle, movementPattern, randomMovement);

        detectUpwardMovement(rectangle, isMoving, speed);

        detectDownwardMovement(window, rectangle, isMoving, speed);
        
        ball.move(current);

        // ball bounds
        float bottomOfBall = ball.getPosition().y + ball.getRadius();
        float rightSideOfBall = ball.getPosition().x + ball.getRadius();
        float topOfBall = ball.getPosition().y - ball.getRadius();
        float leftSideOfBall = ball.getPosition().x - ball.getRadius();

        // screen bounds
        float bottomOfScreen = window.getSize().y;
        float rightSideOfScreen = window.getSize().x;
        float topOfScreen = 0;
        float leftSideOfScreen = 0;

        // collision cases
        if (bottomOfBall >= bottomOfScreen && current == downRight) {
            current = upRight;
        }

        if (bottomOfBall >= bottomOfScreen && current == downLeft) {
            current = upLeft;
        }

        if (topOfBall <= topOfScreen && current == upLeft) {
            current = downLeft;
        }
        if (topOfBall <= topOfScreen && current == upRight) {
            current = downRight;
        }

        // rectangle bounds
        float frontOfRectangle = rectangle.getSize().x;
        float backOfRectangle = rectangle.getPosition().x;
        float topOfRectangle = rectangle.getPosition().y;
        float bottomOfRectangle = rectangle.getPosition().y + rectangle.getSize().y;

        // Collision for front of users rectangle
        if (leftSideOfBall <= frontOfRectangle && 
            bottomOfBall <= bottomOfRectangle && 
            topOfBall >= topOfRectangle && 
            leftSideOfBall > leftSideOfScreen &&
            current == upLeft) {

            current = upRight;
        }

        if (leftSideOfBall <= frontOfRectangle &&
            bottomOfBall <= bottomOfRectangle && 
            topOfBall >= topOfRectangle && 
            leftSideOfBall > leftSideOfScreen &&
            current == downLeft) {

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

            current = downLeft;
        }

        isMoving = false;

        // Evil rectangle bounds
        float frontOfEvilRectangle = evilRectangle.getPosition().x;
        float backOfEvilRectangle = evilRectangle.getPosition().x + evilRectangle.getSize().x;
        float topOfEvilRectangle = evilRectangle.getPosition().y;
        float bottomOfEvilRectangle = evilRectangle.getPosition().y + evilRectangle.getSize().y;

        // Collision for front of evil rectangle
        if (rightSideOfBall >= frontOfEvilRectangle &&
            bottomOfBall <= bottomOfEvilRectangle &&
            topOfBall >= topOfEvilRectangle &&
            rightSideOfBall < rightSideOfScreen &&
            current == upRight) {

            current = upLeft;
        }

        if (rightSideOfBall >= frontOfEvilRectangle &&
            bottomOfBall <= bottomOfEvilRectangle &&
            topOfBall >= topOfEvilRectangle &&
            rightSideOfBall < rightSideOfScreen &&
            current == downRight) {

            current = downLeft;
        }

        // collision for top of evil rectangle
        if (rightSideOfBall <= backOfEvilRectangle &&
            leftSideOfBall >= frontOfEvilRectangle &&
            bottomOfBall >= topOfEvilRectangle &&
            bottomOfBall <= bottomOfEvilRectangle) {

            upRight.y -= speed;
            downRight.y += speed;

            current = upRight;
        }

        // collision for bottom of evil rectangle
        if (rightSideOfBall <= backOfEvilRectangle &&
            leftSideOfBall >= frontOfEvilRectangle &&
            topOfBall <= bottomOfEvilRectangle &&
            topOfBall >= topOfEvilRectangle) {

            downRight.y += speed;
            upRight.y -= speed;

            current = downRight;
        }

        sf::Vector2f targetPosition = {};

        // Calculate trajectory for evil rectangle to go to
        if (current == upRight || current == downRight) {
            // ASSERT: either ball is going upRight or downRight

            calculateTrajectory(ball, current, upRight, rightSideOfScreen, topOfScreen, downRight, bottomOfScreen, targetPosition);
        }

        if (current == upRight || current == downRight) {
            if (randomMovement) {
                executeRandomMovement(movementPattern, evilRectangle, isComplete, speed, window, randomMovement);
            } else {
                float pos = evilRectangle.getPosition().y;
                float targetPos = targetPosition.y - (evilRectangle.getSize().y / 2);
                moveToTarget(pos, targetPos, window, evilRectangle, speed);
            }
        }

        if (current == upLeft || current == downLeft) {
            sway(window, evilRectangle, swayRange, originalY, swayUp, swaySpeed);
        }


        // ball speeds up (exponential function, slow at first fast later on)
        increaseBallSpeed(masterSpeed, current, upLeft, upRight, downLeft, downRight);

        window.clear(sf::Color::Black);

        window.draw(ball);
        window.draw(rectangle);
        window.draw(evilRectangle);

        window.display();
    }
}

