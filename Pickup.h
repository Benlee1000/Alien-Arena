#pragma once
#include <SFML/Graphics.hpp>
#include "PickupTypes.h"
#include "Gun.h"
using namespace sf;

class Pickup
{
private:
	//Start value for health pickups
	const int HEALTH_START_VALUE = 50;
	const int AMMO_START_VALUE = 2;
	const int START_WAIT_TIME = 10;
	const int START_SECONDS_TO_LIVE = 6;

	// The sprite that represents this pickup
	Sprite m_Sprite;

	// The arena it exists in
	IntRect m_Arena;

	// How much is this pickup worth?
	int m_Value;

	// What type of pickup is this?
	PickupType m_Type;

	//A random value for pickup spawning
	int randValue;

	// Handle spawning and disappearing
	bool m_Spawned = false;
	bool m_weapon_got = false;
	float m_SecondsSinceSpawn;
	float m_SecondsSinceDeSpawn;
	float m_SecondsToLive;
	float m_SecondsToWait;

public:
	Pickup(PickupType pickupType);
	
	// Prepare a new pickup
	void setArena(IntRect arena);
	void spawn();
	
	// Check the position of a pickup
	FloatRect getPosition();
	
	// Get the sprite for drawing
	Sprite getSprite();
	
	// Let the pickup update itself each frame
	void update(float elapsedTime);
	
	// Is this pickup currently spawned?
	bool isSpawned();
	
	// Get the goodness from the pickup
	int gotIt(Gun gun);

	// Hide the weapon pickup once recieved
	void gotWeapon();
	
	// Upgrade the value of each pickup
	void upgrade();

	// Reset the values of each pickup
	void reset();
};



