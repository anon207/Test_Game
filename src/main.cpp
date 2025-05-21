#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");

    sf::Vector2u windowSize = window.getSize();

    // Users rectangle
    sf::RectangleShape rectangle({});
    rectangle.setSize(sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.2f));
    rectangle.setFillColor(sf::Color(100, 250, 50));

    // Enemy rectangle
    sf::RectangleShape evilRectangle({});
    evilRectangle.setSize(sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.2f));
    evilRectangle.setFillColor(sf::Color(100, 250, 50));
    evilRectangle.setPosition({ windowSize.x - evilRectangle.getSize().x, 0 });



    // ball 
    sf::CircleShape ball(windowSize.x * 0.01f);
    ball.setOrigin({ ball.getRadius(), ball.getRadius() });
    ball.setPosition({ windowSize.x / 2.f, windowSize.y / 2.f });

    // ball trajectories
    sf::Vector2f upLeft = { -.03, -.03 };
    sf::Vector2f upRight = { .03, -.03 };
    sf::Vector2f downLeft = { -.03, .03 };
    sf::Vector2f downRight = { .03, .03 };

    sf::Vector2f current = downLeft;

    //sf::CircleShape testBall(windowSize.x * 0.01f);

    bool isMoving = false;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        float speed = .1f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            sf::Vector2f pos = rectangle.getPosition();
            if (pos.y > 0) {
                isMoving = true;
                rectangle.move({ 0, -speed });
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            float bottomLimit = window.getSize().y - rectangle.getSize().y;
            if (rectangle.getPosition().y < bottomLimit) {
                isMoving = true;
                rectangle.move({ 0, speed });
            }
        }
        
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

        // rectangle bounds
        float frontOfRectangle = rectangle.getSize().x;
        float backOfRectangle = rectangle.getPosition().x;
        float topOfRectangle = rectangle.getPosition().y;
        float bottomOfRectangle = rectangle.getPosition().y + rectangle.getSize().y;

        //testBall.setPosition({ frontOfRectangle, bottomOfBall });


        // collision cases
        if (bottomOfBall >= bottomOfScreen && current == downRight) {
            current = upRight;
        }

        if (bottomOfBall >= bottomOfScreen && current == downLeft) {
            current = upLeft;
        }

        if (rightSideOfBall >= rightSideOfScreen && current == upRight) {
            current = upLeft;
        }

        if (rightSideOfBall >= rightSideOfScreen && current == downRight) {
            current = downLeft;
        }

        if (topOfBall <= topOfScreen && current == upLeft) {
            current = downLeft;
        }
        if (topOfBall <= topOfScreen && current == upRight) {
            current = downRight;
        }

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
                upLeft.y -= .1f;
                downLeft.y += .1f;
            }

            current = upLeft;
        }

        // collision for bottom of users rectangle
        if (leftSideOfBall >= backOfRectangle &&
            rightSideOfBall <= frontOfRectangle &&
            topOfBall <= bottomOfRectangle && 
            topOfBall >= topOfRectangle) {

            if (isMoving) {
                downLeft.y += .1f;
                upLeft.y -= .1f;
            }

            current = downLeft;
        }


        isMoving = false;
        window.clear(sf::Color::Black);

        window.draw(ball);
        window.draw(rectangle);
        //window.draw(testBall);

        window.display();
    }
}
