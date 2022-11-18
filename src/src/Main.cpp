// Term-Fall 2022

#include "GameWindow.hpp"

#include <iostream>
#include <vector>
using namespace std;

int main( int argc, char **argv ) 
{

	std::string luaPikachu = "Assets/pikachu.lua";
	std::string luaSnorlax = "Assets/snorlax.lua";
	std::string luaSene = "Assets/scene.lua";
	std::string title("F22 Pokemon Fight!");

	CS488Window::launch(argc, argv, new GameWindow({luaPikachu, luaSnorlax, luaSene}), 1024, 768, title);


	return 0;
}
