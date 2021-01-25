#pragma once
#include "Alien.h"
using namespace sf;

int createBackground(VertexArray& rVA, IntRect arena);
Alien* createHorde(int numAliens, IntRect arena);

