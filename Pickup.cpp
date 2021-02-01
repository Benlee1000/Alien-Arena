#include "Pickup.h"
#include "TextureHolder.h"

Pickup::Pickup(PickupType pickupType)
{
	// Store the type of this pickup
	m_Type = pickupType;

	// Associate the texture with the sprite
	if (m_Type == PickupType::HEALTH)
	{
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/health_pickup.png"));
		
		// How much is pickup worth
		m_Value = HEALTH_START_VALUE;

		// Set random value
		randValue = 1;
	}
	else if (m_Type == PickupType::AMMO)
	{
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/ammo_pickup.png"));
		
		// How much is pickup worth
		m_Value = AMMO_START_VALUE;

		// Set random value
		randValue = 2;
	}
	else 
	{
		if (m_Type == PickupType::AK47)
		{
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/AK47.png"));
		}
		else if (m_Type == PickupType::M16)
		{
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/M16.png"));
		}
		else if (m_Type == PickupType::SA80)
		{
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/SA80.png"));
		}
		else if (m_Type == PickupType::P90)
		{
			m_Sprite = Sprite(TextureHolder::GetTexture("graphics/P90.png"));
		}
		m_Value = 0;
		randValue = 3;
	}
	m_Sprite.setOrigin(25, 25);
	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;
}

void Pickup::setArena(IntRect arena)
{
	// Copy the details of the arena to the pickup's m_Arena
	m_Arena.left = arena.left + 50;
	m_Arena.width = arena.width - 100;
	m_Arena.top = arena.top + 50;
	m_Arena.height = arena.height - 100;
	spawn();
	
}

void Pickup::spawn()
{
	// Spawn at a random location, in the arena
	srand((int)time(0) / randValue);
	int x = (rand() % m_Arena.width) + 50;
	srand((int)time(0) * randValue);
	int y = (rand() % m_Arena.height) + 50;
	m_SecondsSinceSpawn = 0;
	m_Spawned = true;
	m_Sprite.setPosition(x, y);
}

FloatRect Pickup::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

Sprite Pickup::getSprite()
{
	return m_Sprite;
}

bool Pickup::isSpawned()
{
	return m_Spawned;
}

int Pickup::gotIt(Gun gun)
{
	m_Spawned = false;
	m_SecondsSinceDeSpawn = 0;

	if (m_Type == PickupType::AMMO)
	{
		return m_Value * gun.getClipSize();
	}
	return m_Value;
}

void Pickup::gotWeapon()
{
	m_Spawned = false;
	m_weapon_got = true;
	m_SecondsSinceDeSpawn = 0;

}

void Pickup::update(float elapsedTime)
{
	if (m_weapon_got)
	{
		// Exit update function if the weapon has been recieved
		return;
	}

	if (m_Spawned)
	{
		m_SecondsSinceSpawn += elapsedTime;
	}
	else
	{
		m_SecondsSinceDeSpawn += elapsedTime;
	}

	// Do we need to hide a pickup?
	if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned)
	{
		// Remove the pickup and put it somewhere else
		m_Spawned = false;
		m_SecondsSinceDeSpawn = 0;
	}
	
	// Do we need to spawn a pickup
	if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned)
	{
		// spawn the pickup and reset the timer
		spawn();
	}
}

void Pickup::upgrade()
{
	if (m_Type == PickupType::HEALTH)
	{
		m_Value += (HEALTH_START_VALUE * .5);
	}
	else
	{
		m_Value += (AMMO_START_VALUE * .5);
	}
	
	// Make them more frequent and last longer
	m_SecondsToWait -= (START_WAIT_TIME / 10);
	m_SecondsToLive += (START_SECONDS_TO_LIVE / 10);
}

void Pickup::reset()
{
	if (m_Type == PickupType::HEALTH)
	{
		m_Value = HEALTH_START_VALUE;
	}
	else
	{
		m_Value = AMMO_START_VALUE;
	}
	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;
}