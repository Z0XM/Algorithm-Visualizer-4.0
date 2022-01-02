#pragma once
#include "TwoDAlgo.hpp"
#include <chrono>
#include <random>
#include <algorithm>
#include <set>


constexpr int WALL = 0;
constexpr int TARGET = 9;
constexpr int START = 2;
constexpr int PATH = 1;
constexpr int VISITED = 5;
constexpr int OPTION = 6;
constexpr int CORRECT_PATH = 3;
constexpr int WRONG_PATH = 4;


TWO_D_ALGORITHMS_FOR(Pathfinding) {

	bool isWall(int row, int col){
		return getVec2d(row, col) == WALL;
	}

	bool isVisited(int row, int col)
	{
		return getVec2d(row, col) == VISITED;
	}

	bool isTarget(int row, int col)
	{
		return getVec2d(row, col) == TARGET;
	}

	bool isPath(int row, int col)
	{
		return getVec2d(row, col) == PATH;
	}

	void markAsVisited(int row, int col)
	{
		setVec2d(row, col, VISITED);
	}

	void markAsOption(int row, int col) {
		setVec2d(row, col, OPTION);
	}
	
	void markPathFound(int row, int col, bool path_found_on_this_location)
	{
		if (path_found_on_this_location) setVec2d(row, col, CORRECT_PATH);
		else setVec2d(row, col, WRONG_PATH);
	}



	SETUP_ALGO(2, "Brute", "Dijkstra");

	int bruteForce(int y, int x)
	{
		if (isWall(y, x) || isVisited(y, x)) 
			return false;

		if (isTarget(y, x)) 
			return true;

		markAsVisited(y, x);

		int dir[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(dir, dir + 4, std::default_random_engine(seed));

		for (int i = 0; i < 4; i++){
			int newY = y + dir[i][1], newX = x + dir[i][0];
			vec.pauseRecording();
			if (isPath(newY, newX)) markAsOption(newY, newX);
			vec.resumeRecording();
		}

		bool path_found = false;
		for (int i = 0; i < 4 && !path_found; i++)
			path_found = bruteForce(y + dir[i][1], x + dir[i][0]);

		markPathFound(y, x, path_found);

		return path_found;
	}
	CREATE_ALGO(0, {
		loadBackupVector();

		vec.startRecording();
		bruteForce(start_cell.y, start_cell.x);
		vec.stopRecording();
	});



	CREATE_ALGO(1,
		{

			loadBackupVector();

			vec.startRecording();

			auto distFrom = [this](const sf::Vector2i& v, const sf::Vector2i& from) {
				return 10 * (int)std::sqrt((v.x - from.x) * (v.x - from.x) + (v.y - from.y) * (v.y - from.y));
			};

			std::set<sf::Vector2i, bool(*)(int, int)> options([](sf::Vector2i& a, sf::Vector2i& b) 
				{
					auto sa = distFrom(a, start_cell);
					auto sb = distFrom(b, start_cell);
					auto ea = distFrom(a, end_cell);
					auto eb = distFrom(b, end_cell);

					if (sa + ea < sb + eb) return true;
					if (sa + ea > sb + eb) return false;
					if (ea < eb) return true;
					return false;
				}
			);

			options.insert(start_cell);

			while (!options.empty()) {
				sf::Vector2i cell(*(options.begin()));
				options.erase(options.begin());

				if (isTarget(cell.y, cell.x))break;


			}

			
			vec.stopRecording();
		}
	);



	std::vector<std::vector<int>> generateMaze(int rows, int cols)
	{
		std::vector<std::vector<int>> maze(rows, std::vector<int>(cols, WALL));

		for (int i = 1; i < rows - 1; i+=2) {
			for (int j = 1; j < cols - 1; j+=2) {
				maze[i][j] = -1;
			}
		}
		
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine gen(seed);
		
		std::uniform_int_distribution<> dist(0, (rows - 1) / 2 - 1);
		start_cell.x = dist(gen) * 2 + 1; start_cell.y = rows - 2;

		std::vector<sf::Vector2i> path_options;
		path_options.emplace_back(start_cell); 

		auto ifCell = [rows, cols, &maze](const sf::Vector2i& cell, int value) {
			if (cell.y >= 0 && cell.y < rows && cell.x >= 0 && cell.x < cols && maze[cell.y][cell.x] == value)return true;
			return false;
		};

		int cells = (rows - 1) * (rows - 1) / 4;
		int p = 1;
			
		while (!path_options.empty()) {
			std::uniform_int_distribution<> dist(0, path_options.size() - 1);
			int index = dist(gen);
			sf::Vector2i cell(path_options[index]); path_options.erase(path_options.begin() + index);
			
			maze[cell.y][cell.x] = PATH;
			end_cell = cell;

			sf::Vector2i dir[4] = { {-2 , 0}, {2, 0}, {0, -2}, {0, 2} };
			
			shuffle(dir, dir + 4, gen);

			bool connected = false;
			for (int i = 0; i < 4; i++) {
				sf::Vector2i targetCell(cell.x + dir[i].x, cell.y + dir[i].y);
				if (ifCell(targetCell, -1) && std::find(path_options.begin(), path_options.end(), targetCell) == path_options.end()) {
					path_options.emplace_back(targetCell);
				}
				else if (!connected && ifCell(targetCell, PATH)) {
					maze[cell.y + dir[i].y / 2][cell.x + dir[i].x / 2] = PATH;
					connected = true;
				}
			}
		}

		maze[start_cell.y][start_cell.x] = START;
		maze[end_cell.y][end_cell.x] = TARGET;

		return maze;
	}

	void onLoad() override
	{
		maze_size = 5;

		pg_visvec->move((pg_visvec->getMaxSize().x - pg_visvec->getMaxSize().y) * 0.5f, 0);
		pg_visvec->setMaxSize(sf::Vector2f(pg_visvec->getMaxSize().y, pg_visvec->getMaxSize().y));

		btn_gen = zui::createFromStyleSheet<zui::TextButton>("red btn");
		placeElement(btn_gen.get(), pg_head.get(), 1, 0, "Generate");
		btn_gen->setAction([this]
			{
				rows = cols = maze_size * 2 + 1;

				vec.clear();
				vec.clearBuffer();
				vec.resize(rows * cols);

				auto maze = generateMaze(rows, cols);
				for (int i = 0; i < maze.size(); i++) {
					for (int j = 0; j < maze[i].size(); j++) {
						setVec2d(i, j, maze[i][j]);
					}
				}

				vec.forceUpdateDrawBuffer();

				createBackupVector();
			}
		);

		sld_size = zui::createFromStyleSheet<zui::Slider>("sld");
		placeElement(sld_size.get(), pg_head.get(), 0, 0, "Size");
		sld_size->setVariable(maze_size, 3, 50, true);
		sld_size->attachAction([this]
			{
				tb_size->setString(std::to_string((int)maze_size * 2 + 1));
			}
		);

		tb_size = zui::createFromStyleSheet<zui::Textbox>("sld");
		placeElement(tb_size.get(), pg_head.get(), 0, 1, std::to_string((int)maze_size * 2 + 1));


		btn_gen->callAction();

		vec.setColorMapper([this](int index, int value)
			{
				switch (value) {
				case 2: return sf::Color::Cyan;
				case 9: return sf::Color::Magenta; // Target
				case 6: return sf::Color::Yellow; // Target
				case 5: return sf::Color::White;  // Path
				case 4: return sf::Color::Red;     // Wrong Path
				case 3: return sf::Color::Green;   // Final Path / Trace
				case 1: return sf::Color(126, 126, 126);   // Path
				case 0: return sf::Color::Black;   // Wall
				}
			}
		);
	}

	void onUnload() override 
	{
		zui::destroy(btn_gen);
		zui::destroy(sld_size);
		zui::destroy(tb_size);
	}

	float maze_size;
	zui::TextButton_ptr btn_gen;
	zui::Slider_ptr sld_size;
	zui::Textbox_ptr tb_size;
	
	sf::Vector2i start_cell, end_cell;
};