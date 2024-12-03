#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <cstdio>
#include <ctime>

void gameOverText(sf::Text &gameOver, sf::Font &font);
std::vector<sf::RectangleShape> initBoxes(std::vector<sf::RectangleShape> &boxes, int boxNum, sf::Vector2f boxSize, sf::FloatRect circleBounds);
void moveCircle(sf::CircleShape &circle, sf::Vector2u windowSize);
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

	// Vector of boxes
	std::vector<sf::RectangleShape> boxes;
	sf::Vector2f boxSize(100, 100);
	// Initialize all the boxes
	initBoxes(boxes, 10, boxSize, circleBounds);

	// Bar for the bottom of the screen
	sf::Vector2f barSize(200, 20);
	sf::RectangleShape bar(barSize);
	bar.setFillColor(sf::Color::White);
	bar.setPosition((window.getSize().x - barSize.x) / 2.0,
					(window.getSize().y - barSize.y) / 2.0);

	// Create game over text
	sf::Text gameOver;
	sf::Font font;
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
		}
		// Circle boundaries so we can check position relative to other objects
		sf::FloatRect circleBounds = circle.getGlobalBounds();

		// Keyboard input which moves the circle
		moveCircle(circle, window.getSize());

		window.clear();
		if (gameChecker) {
			window.draw(gameOver);
		} else {
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
	gameOverText.setString("GAME OVER");
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(sf::Color::Red);

	// Center the text
	sf::FloatRect textBounds = gameOverText.getLocalBounds();

	gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f,
						   textBounds.top + textBounds.height / 2.0f);
}

std::vector<sf::RectangleShape> initBoxes(std::vector<sf::RectangleShape> &boxes, int boxNum, sf::Vector2f boxSize, sf::FloatRect circleBounds) {
	// Random position of box
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	while (boxes.size() < boxNum) {
		sf::RectangleShape box(boxSize);
		box.setFillColor(sf::Color::Red);

		// Generate random position
		float x = std::rand() % (1100 - static_cast<int>(boxSize.x));
		float y = std::rand() % (400 - static_cast<int>(boxSize.y));
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
		}
	}
	return boxes;
}

void moveCircle(sf::CircleShape &circle, sf::Vector2u windowSize) {
	// Check for keyboard input and move the circle
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		if (circle.getPosition().x > 0) {
			circle.move(-0.1, 0);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		if (circle.getPosition().x + circle.getRadius() * 2 <
			windowSize.x) {
			circle.move(0.1, 0);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		if (circle.getPosition().y > 0) {
			circle.move(0, -0.1);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		if (circle.getPosition().y + circle.getRadius() * 2 <
			windowSize.y) {
			circle.move(0, 0.1);
		}
	}
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
	// return boxes;
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
