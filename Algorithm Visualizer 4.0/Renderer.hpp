#pragma once
#include <SFML/Graphics.hpp>
#include <ZUI.hpp>
#include "src/Sorting.hpp"
#include "src/Searching.hpp"
#include "src/Pathfinding.hpp"
#include "src/Custom.hpp"


class Renderer {
public:
	Renderer();
	~Renderer();

	bool isRunning();

	void update();
	void pollEvents();
	void render();

	sf::Vector2f getMousePosition();
	sf::Vector2f getWinSize();

private:
	enum class AppMode {
		MAIN, SORT, SEARCH, PATHFIND, CUSTOM
	} curAppMode;

	void initWindow();
	void initGUI();

	void unloadAppMode();
	void loadAppMode(AppMode mode);

	sf::RenderWindow window;
	bool running;

	sf::Font font, creditFont;

	Sorting sorting;
	Searching searching;
	Pathfinding pathfinding;
	Custom custom;

	zui::Frame frm_main;
	zui::Page_ptr pg_main;
	std::vector<zui::TextButton_ptr> btn_vec;	
};
