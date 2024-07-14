#pragma once
#include "Game.h"

class Render 
{
public:
  Render(Game& game);

  // Show menu
  inline void operator()() {run();};

private:
  void run();
  void PlayerESP();

private:
  Game& game_;
};



