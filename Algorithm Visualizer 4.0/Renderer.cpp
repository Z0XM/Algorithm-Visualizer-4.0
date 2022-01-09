#include "Renderer.hpp"

#define CHOOSE_CLASS_FOR(what) \
	using enum AppMode; \
	switch (curAppMode) \
	{ \
	case MAIN: break; \
	case SORT: sorting.what; break; \
	case SEARCH: searching.what; break; \
	case PATHFIND: pathfinding.what; break; \
	}


void Renderer::initWindow()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	window.create(sf::VideoMode(1450, 500), "Algorithm Visualizer By Z0XM", sf::Style::Default, settings);
	window.setFramerateLimit(60);
}

void Renderer::initGUI()
{
	font.loadFromFile("../assets/consola.ttf");

	//ZUI STYLES
	{
		using enum zui::StyleSheet::StyleType;

		zui::StyleSheet::styleGroup["main btn"].set(FILL_COLOR, sf::Color::Transparent);
		zui::StyleSheet::styleGroup["main btn"].set(OUT_COLOR, sf::Color(160, 32, 240));
		zui::StyleSheet::styleGroup["main btn"].set(OUT_THICKNESS, 3);
		zui::StyleSheet::styleGroup["main btn"].set(S_FILL_COLOR, sf::Color(160, 32, 240, 126));
		zui::StyleSheet::styleGroup["main btn"].set(S_OUT_COLOR, sf::Color(124, 163, 251));
		zui::StyleSheet::styleGroup["main btn"].set(S_OUT_THICKNESS, 5);
		zui::StyleSheet::styleGroup["main btn"].set(FONT_SIZE, 28);
		zui::StyleSheet::styleGroup["main btn"].set(TEXT_FILL_COLOR, sf::Color::White);
		zui::StyleSheet::styleGroup["main btn"].set(SIZE, sf::Vector2f(250, 50));
		zui::StyleSheet::styleGroup["main btn"].set(FONT, font);


		zui::StyleSheet::styleGroup["red btn"].set(FILL_COLOR, sf::Color::Transparent);
		zui::StyleSheet::styleGroup["red btn"].set(OUT_COLOR, sf::Color::Red);
		zui::StyleSheet::styleGroup["red btn"].set(OUT_THICKNESS, 3);
		zui::StyleSheet::styleGroup["red btn"].set(S_FILL_COLOR, sf::Color(255, 0, 0, 126));
		zui::StyleSheet::styleGroup["red btn"].set(S_OUT_COLOR, sf::Color(255, 165, 0));
		zui::StyleSheet::styleGroup["red btn"].set(S_OUT_THICKNESS, 5);
		zui::StyleSheet::styleGroup["red btn"].set(FONT_SIZE, 28);
		zui::StyleSheet::styleGroup["red btn"].set(TEXT_FILL_COLOR, sf::Color::White);
		zui::StyleSheet::styleGroup["red btn"].set(SIZE, sf::Vector2f(150, 50));
		zui::StyleSheet::styleGroup["red btn"].set(FONT, font);
		
		zui::StyleSheet::styleGroup["green btn"].inheritFrom(zui::StyleSheet::styleGroup["red btn"]);
		zui::StyleSheet::styleGroup["green btn"].set(OUT_COLOR, sf::Color::Green);
		
		zui::StyleSheet::styleGroup["purple btn"].inheritFrom(zui::StyleSheet::styleGroup["red btn"]);
		zui::StyleSheet::styleGroup["purple btn"].set(OUT_COLOR, sf::Color(160, 32, 240));
		zui::StyleSheet::styleGroup["purple btn"].set(S_FILL_COLOR, sf::Color(160, 32, 240, 126));
		zui::StyleSheet::styleGroup["purple btn"].set(S_OUT_COLOR, sf::Color(124, 163, 251));


		zui::StyleSheet::styleGroup["sld"].inheritFrom(zui::StyleSheet::styleGroup["red btn"]);
		zui::StyleSheet::styleGroup["sld"].set(OUT_COLOR, sf::Color::Blue);
		zui::StyleSheet::styleGroup["sld"].set(BAR_FILL_COLOR, sf::Color(255, 0, 0, 150));
		zui::StyleSheet::styleGroup["sld"].set(S_BAR_FILL_COLOR, sf::Color(255, 0, 0, 255));
	}

	
	// MAIN FRAME
	{
		frm_main.setWindow(window);

		pg_main = zui::create<zui::Page>();
		pg_main->setFillColor(sf::Color(255, 255, 255, 86));
		pg_main->setMaxSize(sf::Vector2f(1450, 500));
		pg_main->setActiveRegion({ 0, 0, pg_main->getMaxSize().x, pg_main->getMaxSize().y / 2 });
		pg_main->setPosition(0, 0);
		pg_main->setScroll(zui::Page::ScrollPlacement::RIGHT);

		frm_main.addEntity(pg_main.get());

		sf::Vector2f gap(25, 50);
		int buttons_in_row = 5;

		zui::TextButton btn;
		zui::StyleSheet::styleGroup["main btn"].applyTo(btn);

		for (int i = 0; i < 10; i++) {
			btn_vec.push_back(zui::copy(btn));
			btn_vec[i]->setPosition(gap.x + (i % buttons_in_row) * (gap.x + btn.getSize().x), gap.y + (i / buttons_in_row) * (gap.y + btn.getSize().y));
			btn_vec[i]->setString("Button");
			pg_main->addEntity(btn_vec[i].get());
			frm_main.push_in_navigationOrder(*btn_vec[i].get());
		}

		btn_vec[0]->setString("Sorting");

		btn_vec[0]->setAction([this]
			{
				loadAppMode(AppMode::SORT);
			}
		);

		btn_vec[1]->setString("Searching");

		btn_vec[1]->setAction([this]
			{
				loadAppMode(AppMode::SEARCH);
			}
		);

		btn_vec[2]->setString("PathFinding");

		btn_vec[2]->setAction([this]
			{
				loadAppMode(AppMode::PATHFIND);
			}
		);
	}

	sorting.init(window);
	searching.init(window);
	pathfinding.init(window);
}

Renderer::Renderer()
{
	this->initWindow();
	this->initGUI();

	running = true;
	curAppMode = AppMode::MAIN;

	loadAppMode(AppMode::PATHFIND);
}

Renderer::~Renderer()
{

}

void Renderer::unloadAppMode()
{
	CHOOSE_CLASS_FOR(unload());
}

void Renderer::loadAppMode(AppMode mode)
{
	unloadAppMode();

	CHOOSE_CLASS_FOR(getFrame().resetEntityPointers());

	curAppMode = mode;

	CHOOSE_CLASS_FOR(load());
}

bool Renderer::isRunning()
{
	return this->running;
}

void Renderer::update()
{
	if (curAppMode == AppMode::MAIN)
		frm_main.update();
	else {
		CHOOSE_CLASS_FOR(getFrame().update());
	}
	
	this->pollEvents();
}

void Renderer::pollEvents()
{
	sf::Event event;
	sf::Vector2f mousePos = this->getMousePosition();
	while (this->window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			this->running = false;

		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
			if (curAppMode == AppMode::MAIN)this->running = false;
			else loadAppMode(AppMode::MAIN);
		}

		if (curAppMode == AppMode::MAIN) frm_main.pollEvents(event);
		else {
			CHOOSE_CLASS_FOR(getFrame().pollEvents(event));
		}
	}
}

void Renderer::render()
{
	this->window.clear();

	if (curAppMode == AppMode::MAIN) frm_main.draw();
	else {
		CHOOSE_CLASS_FOR(draw(window));
	}

	this->window.display();
}

sf::Vector2f Renderer::getMousePosition()
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
}

sf::Vector2f Renderer::getWinSize()
{
	return sf::Vector2f(this->window.getSize());
}