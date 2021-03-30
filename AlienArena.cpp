// Disclaimer: The inspiration for this game was based off a project in "Beginning C++ Programming" by John Norton

#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Player.h"
#include "AlienArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include "math.h"
#include "Gun.h"
#include "PickupTypes.h"
using namespace sf;


int main()
{
	/*
	**********
	Setup Game
	**********
	*/

	// Here is the instance of TextureHolder
	TextureHolder holder;

	// The game will always be in one of four states
	enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };

	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and
	// create an SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;
	RenderWindow window(VideoMode(resolution.x, resolution.y), "Alien Arena", Style::Fullscreen);

	// Create a an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Here is our clock for timing everything
	Clock clock;

	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in
	// relation to world coordinates
	Vector2f mouseWorldPosition;

	// Where is the mouse in
	// relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class and a gun type variable
	Player player;
	string gunType;

	//Create an instance of the gun, set values for starter weapon
	Gun gun;
	gun.setWeapon(24, 8, 8, 1, 33, 10);

	//Get a pointer to the array of bullets
	Bullet* bullets = nullptr;
	bullets = gun.getBullets();

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;

	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	// Prepare for a horde of Aliens
	int numAliens;
	int numAliensAlive;
	Alien* aliens = nullptr;

	// When was the fire button last pressed?
	Time lastPressed;

	// Hide the mouse pointer and replace it with crosshair
	window.setMouseCursorVisible(false);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// Create a couple of pickups
	Pickup healthPickup(PickupType::HEALTH);
	Pickup ammoPickup(PickupType::AMMO);
	Pickup ak47Pickup(PickupType::AK47);
	Pickup m16Pickup(PickupType::M16);
	Pickup sa80Pickup(PickupType::SA80);
	Pickup p90Pickup(PickupType::P90);

	// Handle weapon pickup despawning 
	bool ak47Spawned = true;
	bool m16Spawned = true;
	bool sa80Spawned = true;
	bool p90Spawned = true;

	// About the game
	int score = 0;
	int hiScore = 0;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.jpg");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(145, 988);

	// Load the font
	Font font;
	font.loadFromFile("fonts/earthorbiter.ttf");

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// LEVELING up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream << "1- Increased max health" << "\n2- Increased run speed" << "\n3- More and better health pickups" << "\n4- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(200, 980);

	// Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);

	// Load the high score from a text file
	std::ifstream inputFile("gamedata/scores.txt");
	if (inputFile.is_open())
	{
		// >> Reads the data
		inputFile >> hiScore;
		inputFile.close();
	}

	// Hi Score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());

	// Aliens remaining
	Text aliensRemainingText;
	aliensRemainingText.setFont(font);
	aliensRemainingText.setCharacterSize(55);
	aliensRemainingText.setFillColor(Color::White);
	aliensRemainingText.setPosition(1500, 980);
	aliensRemainingText.setString("Aliens: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;

	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = 10;

	// Prepare the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	// Prepare the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	Sound splat;
	splat.setBuffer(splatBuffer);

	// Prepare the default shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	// Prepare the AK47 shoot sound
	SoundBuffer shootAK47Buffer;
	shootAK47Buffer.loadFromFile("sound/AK47_shoot.wav");
	Sound shootAK47;
	shootAK47.setBuffer(shootAK47Buffer);

	// Prepare the M16 shoot sound
	SoundBuffer shootM16Buffer;
	shootM16Buffer.loadFromFile("sound/M16_shoot.wav");
	Sound shootM16;
	shootM16.setBuffer(shootM16Buffer);

	// Prepare the SA80 shoot sound
	SoundBuffer shootSA80Buffer;
	shootSA80Buffer.loadFromFile("sound/SA80_shoot.wav");
	Sound shootSA80;
	shootSA80.setBuffer(shootSA80Buffer);

	// Prepare the P90 shoot sound
	SoundBuffer shootP90Buffer;
	shootP90Buffer.loadFromFile("sound/P90_shoot.wav");
	Sound shootP90;
	shootP90.setBuffer(shootP90Buffer);

	// Prepare the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	// Prepare the reload failed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	// Prepare the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	// Prepare the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

	// The main game loop
	while (window.isOpen())
	{
		/*
		************
		Handle input
		************
		*/

		// Handle events by polling
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				// Pause a game while playing
				if (event.key.code == Keyboard::Return && state == State::PLAYING)
				{
					state = State::PAUSED;
				}

				// Restart while paused
				else if (event.key.code == Keyboard::Return && state == State::PAUSED)
				{
					state = State::PLAYING;

					// Reset the clock so there isn't a frame jump
					clock.restart();
				}

				// Start a new game while in GAME_OVER state
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;
					wave = 0;
					score = 0;

					//Prepare the starter weapon for the next game
					player.setGun("StarterWeapon");
					gun.setWeapon(24, 8, 8, 1, 33, 10);

					// Reset the player's stats
					player.resetPlayerStats();

					// Reset the pickup upgrades
					healthPickup.reset();
					ammoPickup.reset();

					// Reset gun pickups
					ak47Spawned = true;
					m16Spawned = true;
					sa80Spawned = true;
					p90Spawned = true;
				}
				if (state == State::PLAYING)
				{
					// Reloading
					if (event.key.code == Keyboard::R)
					{
						if (gun.reload())
						{
							reload.play();
						}
						else
						{
							reloadFailed.play();
						}
					}

				}
			}
		} // End event polling

		// Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Handle WASD and shooting while playing
		if (state == State::PLAYING)
		{
			// Handle the pressing and releasing of the WASD keys
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}
			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}
			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}
			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			// Fire a bullet
			if (Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gun.fireBullet(player, gameTimeTotal, lastPressed, mouseWorldPosition))
				{
					gunType = player.getGun();
					if (gunType == "StarterWeapon")
					{
						shoot.play();
					}
					else if (gunType == "AK47")
					{
						shootAK47.play();
					}
					else if (gunType == "M16")
					{
						shootM16.play();
					}
					else if (gunType == "SA80")
					{
						shootSA80.play();
					}
					else if (gunType == "P90")
					{
						shootP90.play();
					}
					
				}
			} // End fire a bullet
		} // End WASD and shooting while playing

		// Handle the LEVELING up state
		if (state == State::LEVELING_UP)
		{
			// Handle the player LEVELING up
			if (event.key.code == Keyboard::Num1)
			{
				// Increase health
				player.upgradeHealth();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num2)
			{
				// Increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num3)
			{
				// Upgrade health pickup
				healthPickup.upgrade();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num4)
			{
				// Upgrade ammo pickup
				ammoPickup.upgrade();
				state = State::PLAYING;
			}
			if (state == State::PLAYING)
			{
				// Increase the wave number
				wave++;

				// Prepare the level
				arena.width = 50 * wave + 800; // 250 * wave + 500
				arena.height = 50 * wave + 800;
				arena.left = 0;
				arena.top = 0;

				// Pass the vertex array by reference
				// to the createBackground function
				int tileSize = createBackground(background, arena);

				// Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// Configure the pick-ups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);
				if ((wave == 3 || wave == 4) && ak47Spawned)
				{
					ak47Pickup.setArena(arena);
				}
				else if ((wave == 5 || wave == 6) && m16Spawned)
				{
					m16Pickup.setArena(arena);
				}
				else if ((wave == 7 || wave == 8) && sa80Spawned)
				{
					sa80Pickup.setArena(arena);
				}
				else if ((wave == 9 || wave == 10) && p90Spawned)
				{
					p90Pickup.setArena(arena);
				}
		
				// Create a horde of Aliens, default 4 * wave
				numAliens = 4 * wave;

				// Delete the previously allocated memory (if it exists)
				delete[] aliens;
				aliens = createHorde(numAliens, arena);
				numAliensAlive = numAliens;

				// Play the powerup sound
				powerup.play();

				// Reset the clock so there isn't a frame jump
				clock.restart();
			}
		}// End LEVELING up

		/*
		****************
		UPDATE THE FRAME
		****************
		*/

		if (state == State::PLAYING)
		{
			// Update the delta time
			Time dt = clock.restart();

			// Update the total game time
			gameTimeTotal += dt;

			// Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			// Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// Make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// Make the view centre around the player
			mainView.setCenter(player.getCenter());

			// Loop through each Alien and update them
			for (int i = 0; i < numAliens; i++)
			{
				if (aliens[i].isAlive())
				{
					aliens[i].update(dt.asSeconds(), playerPosition);
				}
			}

			// Update any bullets that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			// Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			if (wave >= 3 && wave <= 4)
			{
				ak47Pickup.update(dtAsSeconds);
			}
			else if (wave >= 5 && wave <= 6)
			{
				if (ak47Spawned)
				{
					// Despawn weapon previous weapon pickups
					ak47Pickup.gotWeapon();
					ak47Spawned = false;
				}
				m16Pickup.update(dtAsSeconds);
			}
			else if (wave >= 7 && wave <= 8)
			{
				if (m16Spawned)
				{
					// Despawn weapon previous weapon pickups
					m16Pickup.gotWeapon();
					m16Spawned = false;
				}
				sa80Pickup.update(dtAsSeconds);
			}
			else if (wave >= 9 && wave <= 10)
			{
				if (sa80Spawned)
				{
					// Despawn weapon previous weapon pickups
					sa80Pickup.gotWeapon();
					sa80Spawned = false;
				}
				p90Pickup.update(dtAsSeconds);
			}

			// Collision detection
			// Have any Aliens been shot?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numAliens; j++)
				{
					if (bullets[i].isInFlight() && aliens[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(aliens[j].getPosition()))
						{
							// Stop the bullet
							bullets[i].stop();
							// Register the hit and see if it was a kill
							if (aliens[j].hit())
							{
								// Not just a hit but a kill too
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}
								numAliensAlive--;

								// When all the Aliens are dead (again)
								if (numAliensAlive == 0)
								{
									state = State::LEVELING_UP;
								}

								// Make a splat sound
								splat.play();
							}
						}
					}
				}
			} // End Alien being shot

			// Have any aliens touched the player
			for (int i = 0; i < numAliens; i++)
			{
				if (player.getPosition().intersects(aliens[i].getPosition()) && aliens[i].isAlive())
				{
					if (player.hit(gameTimeTotal))
					{
						hit.play();
					}
					if (player.getHealth() <= 0)
					{
						state = State::GAME_OVER;
						std::ofstream outputFile("gamedata/scores.txt");

						// << writes the data
						outputFile << hiScore;
						outputFile.close();
					}
				}
			}

			// Has the player touched health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt(gun));

				// Play a sound
				pickup.play();
			}

			// Has the player touched ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				gun.addBulletsSpare(ammoPickup.gotIt(gun));

				// Play a sound
				reload.play();
			}

			// Has the player touched AK47 pickup
			if (player.getPosition().intersects(ak47Pickup.getPosition()) && ak47Pickup.isSpawned())
			{
				ak47Pickup.gotWeapon();
				player.setGun("AK47");
				// Set gun stats: Spare Ammo, Clip Ammo, Clip Size, Fire Rate, x Offset, y Offset
				gun.setWeapon(36, 12, 12, 2, 36, 10);
				ak47Spawned = false;
				// Play a sound
				pickup.play();
			}

			// Has the player touched M16 pickup
			if (player.getPosition().intersects(m16Pickup.getPosition()) && m16Pickup.isSpawned())
			{
				m16Pickup.gotWeapon();
				player.setGun("M16");
				gun.setWeapon(48, 16, 16, 3, 42, 10);
				m16Spawned = false;
				// Play a sound
				pickup.play();
			}

			// Has the player touched SA80 pickup
			if (player.getPosition().intersects(sa80Pickup.getPosition()) && sa80Pickup.isSpawned())
			{
				sa80Pickup.gotWeapon();
				player.setGun("SA80");
				gun.setWeapon(60, 20, 20, 4, 42, 10);
				sa80Spawned = false;
				// Play a sound
				pickup.play();
			}

			// Has the player touched P90 pickup
			if (player.getPosition().intersects(p90Pickup.getPosition()) && p90Pickup.isSpawned())
			{
				p90Pickup.gotWeapon();
				player.setGun("P90");
				gun.setWeapon(100, 50, 50, 10, 34, 10);
				p90Spawned = false;
				// Play a sound
				pickup.play();
			}

			// size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 50));

			// Increment the number of frames since the previous update
			framesSinceLastHUDUpdate++;

			// re-calculate every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{
				// Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssAliensAlive;

				// Update the ammo text
				ssAmmo << gun.getBulletsInClip() << "/" << gun.getBulletsSpare();
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssAliensAlive << "Aliens:" << numAliensAlive;
				aliensRemainingText.setString(ssAliensAlive.str());
				framesSinceLastHUDUpdate = 0;
			} // End HUD update

		} // End updating the scene

		/*
		**************
		Draw the scene
		**************
		*/

		if (state == State::PLAYING)
		{
			window.clear();

			// set the mainView to be displayed in the window
			// And draw everything related to it
			window.setView(mainView);

			// Draw the background
			window.draw(background, &textureBackground);

			// Draw the Aliens
			for (int i = 0; i < numAliens; i++)
			{
				window.draw(aliens[i].getSprite());
			}

			// Draw the bullets
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			// Draw the player
			window.draw(player.getSprite());

			// Draw the pick-ups, if currently spawned
			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}
			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}
			if (ak47Pickup.isSpawned())
			{
				window.draw(ak47Pickup.getSprite());
			}
			if (m16Pickup.isSpawned())
			{
				window.draw(m16Pickup.getSprite());
			}
			if (sa80Pickup.isSpawned())
			{
				window.draw(sa80Pickup.getSprite());
			}
			if (p90Pickup.isSpawned())
			{
				window.draw(p90Pickup.getSprite());
			}

			//Draw the crosshair
			window.draw(spriteCrosshair);

			// Switch to the HUD view
			window.setView(hudView);

			// Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(aliensRemainingText);
		}
		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}
		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}
		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}
		window.display();
	} // End of main game loop

	// Delete the previously allocated memory (if it exists)
	delete[] aliens;
	delete[] bullets;

	return 0;
}
