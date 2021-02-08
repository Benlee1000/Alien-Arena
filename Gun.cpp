#include "Gun.h"
#include "Player.h"

int Gun::getBulletsSpare()
{
	return m_bulletsSpare;
}

int Gun::getBulletsInClip()
{
	return m_bulletsInClip;
}

int Gun::getClipSize()
{
	return m_clipSize;
}

Bullet* Gun::getBullets()
{
	return bullets;
}

void Gun::addBulletsSpare(int bulletsSpare)
{
	m_bulletsSpare += bulletsSpare;
}

void Gun::setWeapon(int bulletsSpare, int bulletsInClip, int clipSize, float fireRate, int bulletOffsetx, int bulletOffsety)
{
	m_bulletsSpare = bulletsSpare;
	m_bulletsInClip = bulletsInClip;
	m_clipSize = clipSize;
	m_fireRate = fireRate;

	m_bulletOffset.x = bulletOffsetx;
	m_bulletOffset.y = bulletOffsety;
}

bool Gun::reload()
{
	if (m_bulletsSpare >= m_clipSize)
	{
		// Plenty of bullets. Reload.
		// Possibly change this so the clip isn't "lost" and you can't reload when full
		m_bulletsInClip = m_clipSize;
		m_bulletsSpare -= m_clipSize;
		return true;
	}
	else if (m_bulletsSpare > 0)
	{
		// Only few bullets left
		m_bulletsInClip = m_bulletsSpare;
		m_bulletsSpare = 0;
		return true;
	}
	else
	{
		//Failed reload
		return false;
	}
}

bool Gun::fireBullet(Player player, Time gameTimeTotal, Time& lastPressed, Vector2f mouseWorldPosition)
{
	if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / m_fireRate && m_bulletsInClip > 0)
	{
		m_playerRotation = player.getRotation() * (3.141592 / 180);
		m_newBulletOffset.x = m_bulletOffset.x * cos(m_playerRotation) - m_bulletOffset.y * sin(m_playerRotation);
		m_newBulletOffset.y = m_bulletOffset.x * sin(m_playerRotation) + m_bulletOffset.y * cos(m_playerRotation);

		//Pass the centre of the player
		// and the centre of the cross-hair
		// to the shoot function
		bullets[m_currentBullet].shoot(player.getCenter().x + m_newBulletOffset.x, player.getCenter().y + m_newBulletOffset.y, mouseWorldPosition.x, mouseWorldPosition.y);
		m_currentBullet++;
		if (m_currentBullet > 99)
		{
			m_currentBullet = 0;
		}
		lastPressed = gameTimeTotal;
		m_bulletsInClip--;
		return true;
	}
	return false;
}
