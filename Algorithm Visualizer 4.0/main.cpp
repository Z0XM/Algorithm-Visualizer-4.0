#include "Renderer.hpp"

int main()
{
	Renderer sfml;
	while (sfml.isRunning())
	{
		sfml.update();
		sfml.render();
	}
}