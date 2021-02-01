#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "TextureHolder.h"
#include <SFML/Audio.hpp>
using namespace sf;
using namespace std;

class Player;
class Gun
{
private:
	// Variables to hold gun values
	int m_bulletsSpare;
	int m_bulletsInClip;
	int m_clipSize;
	float m_fireRate;
	int m_currentBullet = 0;

	//Array of bullets
	Bullet* bullets = new Bullet[100];

	// Where to spawn the bullet (Depends on the weapon and rotation)
	Vector2f m_bulletOffset;
	Vector2f m_newBulletOffset;
	float m_playerRotation;

public:
	//Getters for dispalying gun information
	int getBulletsSpare();
	int getBulletsInClip();
	int getClipSize();
	Sprite getSprite();
	Bullet* getBullets();
	void addBulletsSpare(int bulletsSpare);

	void setWeapon(int bulletsSpare, int bulletsInClip, int clipSize, float fireRate, int bulletOffsetx, int bulletOffsety);
	bool reload();
	bool fireBullet(Player player, Time gameTimeTotal, Time& lastPressed, Vector2f mouseWorldPosition);

};

