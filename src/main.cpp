#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <cstdio>
#include <random>

void gameOverText(sf::Text &gameOver, sf::Font &font);
int initBoxes(std::vector<sf::RectangleShape> &boxes, int boxNum, sf::Vector2f boxSize, sf::FloatRect circleBounds);
void moveBar(sf::RectangleShape &bar, sf::Vector2i mousePosition, sf::Vector2u windowSize);
void eliminateBoxes(std::vector<sf::RectangleShape> &boxes, sf::FloatRect circleBounds);
bool isOverlapping(const sf::FloatRect &rect1, const sf::FloatRect &rect2);

int main() {
	printf("Hello World\n");

	sf::RenderWindow window(sf::VideoMode(1100, 800), "Breakout");
	bool gameChecker = false;

	// Circle
	sf::CircleShape circle(50.f);
	circle.setFillColor(sf::Color::Green);
	circle.setPosition((window.getSize().x - circle.getRadius() * 2) / 2.0,
					   (window.getSize().y - circle.getRadius() * 2) / 2.0);
	sf::FloatRect circleBounds = circle.getGlobalBounds();
	sf::Vector2f circleVelocity(0.1f, -0.1f);

	// Vector of boxes
	std::vector<sf::RectangleShape> boxes;
	sf::Vector2f boxSize(100, 100);
	int initBoxFlag = false;
	// Initialize all the boxes
	while (!initBoxFlag) {
		initBoxFlag = initBoxes(boxes, 10, boxSize, circleBounds);
	}
	initBoxFlag = false;

	// Bar for the bottom of the screen
	sf::Vector2f barSize(250, 20);
	sf::RectangleShape bar(barSize);
	bar.setFillColor(sf::Color::White);
	bar.setPosition((window.getSize().x - barSize.x) / 2.0,
					(window.getSize().y * 1.85 - barSize.y) / 2.0);

	// Positions for circle and bar
	float circleBottom = circle.getPosition().y + circle.getRadius() * 2;  // Bottom of the circle
	float barTop = bar.getPosition().y;									   // Top of the bar
	float circleLeft = circle.getPosition().x;							   // Left side of the circle
	float circleRight = circle.getPosition().x + circle.getRadius() * 2;   // Right side of the circle
	float barLeft = bar.getPosition().x;								   // Left side of the bar
	float barRight = bar.getPosition().x + bar.getSize().x;				   // Right side of the bar

	// Create game over text
	sf::Text gameOver;
	sf::Font font;
	// TODO make this cross-platform later on
	if (!font.loadFromFile("/usr/share/fonts/noto/NotoSans-Bold.ttf")) {
		// Error handling if font doesn't load
		printf("error for font\n");
	}
	gameOver.setPosition(sf::Vector2f(
		window.getSize().x / 2.0f, window.getSize().y / 2.0f));
	gameOverText(gameOver, font);

	// This is the main loop where our program happens
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			// Check for restart input if the game is over
			if (gameChecker && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
				// this is the game over bool variable
				gameChecker = false;
				// reset circle, bar, and boxes positions
				circle.setPosition(window.getSize().x / 2.0f - circle.getRadius(), window.getSize().y / 2.0f);
				circleVelocity = sf::Vector2f(0.1, -0.1);
				bar.setPosition(window.getSize().x / 2.0f - bar.getSize().x / 2.0f, window.getSize().y - 50.f);
				while (!initBoxFlag) {
					initBoxFlag = initBoxes(boxes, 10, boxSize, circleBounds);
				}
				initBoxFlag = false;
			}
		}

		window.clear();
		if (gameChecker) {
			window.draw(gameOver);
		} else {
			// Circle boundaries so we can check position relative to other objects
			circleBounds = circle.getGlobalBounds();

			// Automated circle movement
			circle.move(circleVelocity);

			// Reverse direction if the ball hits the left or right wall
			if (circle.getPosition().x <= 0 || circle.getPosition().x + circle.getRadius() * 2 >= window.getSize().x) {
				circleVelocity.x = -circleVelocity.x;
			}
			// Reverse direction if the ball hits the top wall
			if (circle.getPosition().y <= 0) {
				circleVelocity.y = -circleVelocity.y;
			}

			// Calculate key components locations
			circleBottom = circle.getPosition().y + circle.getRadius() * 2;
			barTop = bar.getPosition().y;
			circleLeft = circle.getPosition().x;
			circleRight = circle.getPosition().x + circle.getRadius() * 2;
			barLeft = bar.getPosition().x;
			barRight = bar.getPosition().x + bar.getSize().x;

			// Check if the circle hits the top of the bar AND is horizontally within the bar's bounds
			if (circleBottom >= barTop &&
				circleRight >= barLeft &&
				circleLeft <= barRight) {
				// Calculate the collision point relative to the bar's width
				float collisionPoint = (circle.getPosition().x + circle.getRadius()) - barLeft;

				float relativePosition = (collisionPoint / bar.getSize().x) - 0.5f;	 // Range: -0.5 to 0.5
				circleVelocity.x = relativePosition * 0.15f;						 // Adjust this factor to limit horizontal speed

				// Clamp horizontal velocity to prevent extreme angles
				float maxHorizontalSpeed = 0.12f;  // Maximum allowed horizontal speed
				if (circleVelocity.x > maxHorizontalSpeed) {
					circleVelocity.x = maxHorizontalSpeed;
				} else if (circleVelocity.x < -maxHorizontalSpeed) {
					circleVelocity.x = -maxHorizontalSpeed;
				}

				// Reverse the vertical direction of the ball
				circleVelocity.y = -std::abs(circleVelocity.y);	 // Ensure it moves upward after hitting the bar

				// Normalize the velocity to maintain a constant speed
				float speed = std::sqrt(circleVelocity.x * circleVelocity.x + circleVelocity.y * circleVelocity.y);
				float desiredSpeed = 0.1f;	// Desired constant speed
				circleVelocity.x = (circleVelocity.x / speed) * desiredSpeed;
				circleVelocity.y = (circleVelocity.y / speed) * desiredSpeed;
			}

			if (circleBottom > window.getSize().y) {
				gameChecker = true;
			}

			// Mouse input which moves bar horizontally
			moveBar(bar, sf::Mouse::getPosition(window), window.getSize());
			window.draw(circle);
			window.draw(bar);
			for (const auto &box : boxes) {
				window.draw(box);  // Draw all boxes
			}
		}
		window.display();

		// Passes in boxes by reference
		eliminateBoxes(boxes, circleBounds);
		if (boxes.empty()) {
			gameChecker = true;
		}
	}

	return 0;
}

void gameOverText(sf::Text &gameOverText, sf::Font &font) {
	gameOverText.setFont(font);
	gameOverText.setString("GAME OVER - Press R to restart game");
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(sf::Color::Red);

	// Center the text
	sf::FloatRect textBounds = gameOverText.getLocalBounds();

	gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f,
						   textBounds.top + textBounds.height / 2.0f);
}

int initBoxes(std::vector<sf::RectangleShape> &boxes, int boxNum, sf::Vector2f boxSize, sf::FloatRect circleBounds) {
	boxes.clear();

	int counter = 0;
	int finishedFlag = false;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> xDist(0, 1100 - static_cast<int>(boxSize.x));
	std::uniform_int_distribution<> yDist(0, 400 - static_cast<int>(boxSize.y));

	// Random position of box
	while (boxes.size() < boxNum && counter < 100) {
		sf::RectangleShape box(boxSize);
		box.setFillColor(sf::Color::Red);

		// Generate random position
		float x = xDist(gen);
		float y = yDist(gen);
		box.setPosition(x, y);

		// Check for overlap with existing boxes
		bool overlaps = false;
		if (isOverlapping(box.getGlobalBounds(), circleBounds)) {
			overlaps = true;  // Box intersects with the circle
		}
		for (const auto &otherBox : boxes) {
			if (isOverlapping(box.getGlobalBounds(), otherBox.getGlobalBounds())) {
				overlaps = true;
				break;
			}
		}
		// If no overlap, add the box to the list
		if (!overlaps) {
			boxes.push_back(box);
		} else {
			counter = counter + 1;
		}
	}
	if (boxes.size() == boxNum) {
		finishedFlag = true;
	}
	return finishedFlag;
}

void moveBar(sf::RectangleShape &bar, sf::Vector2i mousePosition, sf::Vector2u windowSize) {
	float barX = mousePosition.x - bar.getSize().x / 2.0f;
	barX = std::max(0.f, std::min(barX, static_cast<float>(windowSize.x - bar.getSize().x)));

	bar.setPosition(barX, bar.getPosition().y);	 // Update the bar's position
}

void eliminateBoxes(std::vector<sf::RectangleShape> &boxes, sf::FloatRect circleBounds) {
	// Check each box with the circle to see if they intersect
	std::vector<sf::RectangleShape> retVal;
	for (const auto &box : boxes) {
		if (circleBounds.intersects(box.getGlobalBounds())) {
			printf("box has been hit!\n");
		} else {
			// This effectively removes the box that intersected with the circle
			retVal.push_back(box);
		}
	}
	boxes = retVal;
}

bool isOverlapping(const sf::FloatRect &rect1, const sf::FloatRect &rect2) {
	float rect1X = rect1.getSize().x + 50;
	float rect1Y = rect1.getSize().y + 50;
	float rect2Y = rect2.getSize().y + 50;
	float rect2X = rect2.getSize().x + 50;
	sf::FloatRect box1(rect1.getPosition().x, rect1.getPosition().y, rect1X,
					   rect1Y);
	sf::FloatRect box2(rect2.getPosition().x, rect2.getPosition().y, rect2X,
					   rect2Y);
	return box1.intersects(box2);
}
