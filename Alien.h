#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Alien
{
private:
	// How fast is each Alien type?
	const float BLOATER_SPEED = 40;
	const float CHASER_SPEED = 80;
	const float CRAWLER_SPEED = 20;

	// How tough is each Alien type
	const float BLOATER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	// Make each Alien vary its speed slightly
	const int MAX_VARRIANCE = 30;
	const int OFFSET = 101 - MAX_VARRIANCE;

	// Where is this Alien?
	Vector2f m_Position;

	// A sprite for the Alien
	Sprite m_Sprite;

	// How fast can this one run/crawl?
	float m_Speed;

	// How much health has it got?
	float m_Health;

	// Is it still alive?
	bool m_Alive;

public:
	// Handle when a bullet hits a Alien
	bool hit();

	// Find out if the Alien is alive
	bool isAlive();

	// Spawn a new Alien
	void spawn(float startX, float startY, int type, int seed);

	// Return a rectangle that is the position in the world
	FloatRect getPosition();

	// Get a copy of the sprite to draw
	Sprite getSprite();

	// Update the Alien each frame
	void update(float elapsedTime, Vector2f playerLocation);

};


