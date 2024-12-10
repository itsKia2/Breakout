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
	sf::Vector2f circleVelocity(0.1, -0.1);	 // Moves right and upward

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
				// window.close();
				// sf::RenderWindow window(sf::VideoMode(1100, 800), "Breakout");
			}
		}

		window.clear();
		if (gameChecker) {
			window.draw(gameOver);
			// printf("time to game restart\n");
		} else {
			// Circle boundaries so we can check position relative to other objects
			circleBounds = circle.getGlobalBounds();

			// Automated circle movement
			circle.move(circleVelocity);

			// Reverse direction if the ball hits the left or right wall
			if (circle.getPosition().x <= 0 || circle.getPosition().x + circle.getRadius() * 2 >= window.getSize().x) {
				circleVelocity.x = -circleVelocity.x;  // Reverse horizontal direction
			}

			// Calculate the bottom of the circle and the top of the bar
			circleBottom = circle.getPosition().y + circle.getRadius() * 2;	 // Bottom of the circle
			barTop = bar.getPosition().y;									 // Top of the bar
			circleLeft = circle.getPosition().x;							 // Left side of the circle
			circleRight = circle.getPosition().x + circle.getRadius() * 2;	 // Right side of the circle
			barLeft = bar.getPosition().x;									 // Left side of the bar
			barRight = bar.getPosition().x + bar.getSize().x;				 // Right side of the bar

			// Check if the circle hits the top of the bar AND is horizontally within the bar's bounds
			if (circleBottom >= barTop &&
				circleRight >= barLeft &&  // Circle overlaps bar horizontally
				circleLeft <= barRight) {
				circleVelocity.y = -circleVelocity.y;  // Reverse vertical direction
			}

			// Reverse direction if the ball hits the top wall
			if (circle.getPosition().y <= 0) {
				circleVelocity.y = -circleVelocity.y;  // Reverse vertical direction
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

// TODO return int ; do max retries counter and if it doesnt work in max retries then just run initBoxes() above again
int initBoxes(std::vector<sf::RectangleShape> &boxes, int boxNum, sf::Vector2f boxSize, sf::FloatRect circleBounds) {
	boxes.clear();

	int counter = 0;
	int finishedFlag = false;

	std::random_device rd;														   // Non-deterministic random device
	std::mt19937 gen(rd());														   // Seed the Mersenne Twister with the random device
	std::uniform_int_distribution<> xDist(0, 1100 - static_cast<int>(boxSize.x));  // Distribution for x position
	std::uniform_int_distribution<> yDist(0, 400 - static_cast<int>(boxSize.y));   // Distribution for y position

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
	float barX = mousePosition.x - bar.getSize().x / 2.0f;									   // Center the bar on the mouse's X position
	barX = std::max(0.f, std::min(barX, static_cast<float>(windowSize.x - bar.getSize().x)));  // Clamp within window width

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
