#pragma once

#include "testgl/constants.hpp"
#include "testgl/player.hpp"
#include "testgl/callbacks.hpp"
#include "testgl/chunk.hpp"
#include "testgl/world.hpp"

#include <iostream>

void tick_thread(World *world, Player *player, Window *window);
int main(int argc, char **argv);