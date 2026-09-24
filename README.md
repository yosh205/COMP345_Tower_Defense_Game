# COMP345_Tower_Defense_Game
Assignment #1 for Concordia University course COMP 345

## Setup Instructions

This version of the program was created in Visual Studio Code with Ubuntu WSL to be able to compile C++.

Linux or WSL:
sudo apt install libsfml-dev
g++ -std=c++17 -Wall src/main.cpp src/Map/Map.cpp src/GUI/MapView.cpp -o towerdefense -lsfml-graphics -lsfml-window -lsfml-system
./towerdefense

## SFML Usage

SFML 2.6 (Simple and Fast Multimedia Library)
Used for the graphical user interface.

- **Made for 2D games.**
