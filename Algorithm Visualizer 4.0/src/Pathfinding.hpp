#pragma once
#include "TwoDAlgo.hpp"
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <iostream>

constexpr int WALL = 0;
constexpr int TARGET = 9;
constexpr int START = 2;
constexpr int PATH = 1;
constexpr int VISITED = 5;
constexpr int OPTION = 6;
constexpr int CORRECT_PATH = 3;
constexpr int WRONG_PATH = 4;


TWO_D_ALGORITHMS_FOR(Pathfinding) {

	bool isWall(int row, int col) {
		return getVec2d(row, col) == WALL;
	}

	bool isVisited(int row, int col)
	{
		return getVec2d(row, col) == VISITED;
	}

	bool isTarget(int row, int col){
		return end_pos.y == row && end_pos.x == col;
	}

	bool isStart(int row, int col) {
		return start_pos.y == row && start_pos.x == col;
	}

	bool isPath(int row, int col)
	{
		return getVec2d(row, col) == PATH;
	}

	void markAsVisited(int row, int col){
		setVec2d(row, col, VISITED);
	}

	void markAsOption(int row, int col) {
		setVec2d(row, col, OPTION);
	}
	
	void markPathFound(int row, int col, bool path_found_on_this_location){
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
		bruteForce(start_pos.y, start_pos.x);
		setVec2d(start_pos.y, start_pos.x, START);
		vec.stopRecording();
	});


	CREATE_ALGO(1, {
			loadBackupVector();

			vec.startRecording();

			struct Cell {
				sf::Vector2i pos;
				int dTravelled = 10000000;
				int dFromEnd = 0;
				int dTotal = 0;
				Cell* prev = nullptr;
			};

			std::vector<std::vector<Cell>> cells(rows, std::vector<Cell>(cols));
			
			auto getCell = [&cells](const sf::Vector2i& v) -> Cell* {
				return &cells[v.y][v.x];
			};

			getCell(start_pos)->pos = start_pos;
			getCell(start_pos)->dTravelled = 0;
			getCell(start_pos)->dFromEnd = abs(start_pos.x - end_pos.x) + abs(start_pos.y - end_pos.y);
			getCell(start_pos)->dTotal = getCell(start_pos)->dFromEnd;

			auto betterCell = [](const Cell* a, const Cell* b) {
				return (a->dTotal < b->dTotal || (a->dTotal == b->dTotal && (a->dFromEnd < b->dFromEnd || (a->dFromEnd == b->dFromEnd && a < b))));
			};

			std::set<Cell*, decltype(betterCell)> options(betterCell);

			options.insert(getCell(start_pos));

			while (!options.empty()) {
				Cell* closest(*(options.begin()));
				options.erase(options.begin());

				if (isTarget(closest->pos.y, closest->pos.x)) break;

				markAsVisited(closest->pos.y, closest->pos.x);

				sf::Vector2i dir[4] = { {0,-1}, {-1,0}, {1,0}, {0,1} };

				vec.pauseRecording();
				for (int i = 0; i < 4; i++) {
					sf::Vector2i newPos(closest->pos.x + dir[i].x, closest->pos.y + dir[i].y);
					if (isWall(newPos.y, newPos.x) || isVisited(newPos.y, newPos.x))continue;
					
					markAsOption(newPos.y, newPos.x);
					
					int newD = abs(newPos.x - closest->pos.x) + abs(newPos.y - closest->pos.y) + closest->dTravelled;
					if (options.find(getCell(newPos)) == options.end()) {
						getCell(newPos)->pos = newPos;
						getCell(newPos)->dTravelled = newD;
						getCell(newPos)->dFromEnd = abs(newPos.x - end_pos.x) + abs(newPos.y - end_pos.y);
						getCell(newPos)->dTotal = getCell(newPos)->dFromEnd + getCell(newPos)->dTravelled;
						getCell(newPos)->prev = closest;
						auto c = getCell(newPos);
						options.insert(c);
					}
					else if(newD < getCell(newPos)->dTravelled) {
						getCell(newPos)->dTravelled = newD;
						getCell(newPos)->dTotal = getCell(newPos)->dFromEnd + getCell(newPos)->dTravelled;
						getCell(newPos)->prev = closest;
					}
				}
				vec.resumeRecording();
			}

			sf::Vector2i p(end_pos);
			while (p != start_pos) {
				markPathFound(p.y, p.x, true);
				Cell* q = getCell(p)->prev;
				p = q->pos;
			}
			
			setVec2d(start_pos.y, start_pos.x, START);
			setVec2d(end_pos.y, end_pos.x, TARGET);

			vec.stopRecording();
		});


	std::vector<std::vector<int>> generateMaze(int rows, int cols)
	{
		/*std::vector<std::vector<int>> maze =
		{
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
			{0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 1, 1, 9, 0, 1, 1, 1, 0},
			{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
			{0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
			{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
			{0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		};
		start_pos = { 3, 9 };
		end_pos = { 5, 5 };*/

		std::vector<std::vector<int>> maze(rows, std::vector<int>(cols, WALL));

		for (int i = 1; i < rows - 1; i+=2) {
			for (int j = 1; j < cols - 1; j+=2) {
				maze[i][j] = -1;
			}
		}
		
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine gen(seed);
		
		std::uniform_int_distribution<> dist(0, (rows - 1) / 2 - 1);
		start_pos.x = dist(gen) * 2 + 1; start_pos.y = rows - 2;

		std::vector<sf::Vector2i> path_options;
		path_options.emplace_back(start_pos); 

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
			end_pos = cell;

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

		maze[start_pos.y][start_pos.x] = START;
		maze[end_pos.y][end_pos.x] = TARGET;

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
		sld_size->setVariable(maze_size, 3, 25, true);
		sld_size->attachAction([this]
			{
				tb_size->setString(std::to_string((int)maze_size * 2 + 1));
			}
		);

		tb_size = zui::createFromStyleSheet<zui::Textbox>("sld");
		placeElement(tb_size.get(), pg_head.get(), 0, 1, std::to_string((int)maze_size * 2 + 1));

		tb_info = zui::copy(tb_size);
		placeElement(tb_info.get(), pg_head.get(), 1, 3, "");

		pg_visvec->setMouseMoveAction([this]
			{
				sf::Vector2f mousePos = pg_visvec->getLocalMousePosition();
				int row = mousePos.y / vec.getSize(0).y;
				int col = mousePos.x / vec.getSize(0).x;
				if (row >= 0 && row < rows && col >= 0 && col < cols) {
					if (current_algo == 0) {
						if (isStart(row, col)) tb_info->setString("Start");
						else if (isTarget(row, col)) tb_info->setString("Target");

						else {
							int value = vec.getBufferValue(row * cols + col);

							std::string str;
							switch (value) {
							case WALL: str = "Wall"; break;
							case PATH: str = "Path"; break;
							case VISITED: str = "Visited"; break;
							case CORRECT_PATH: str = "Correct"; break;
							case WRONG_PATH: str = "Wrong"; break;
							case OPTION: str = "Option"; break;
							}

							tb_info->setString(str);
						}
					}
					else if (current_algo == 1) {

					}
				}
			}
		);
		

		btn_gen->callAction();

		vec.setColorMapper([this](int index, int value)
			{
				switch (value) {
				case START: return sf::Color::Cyan;
				case TARGET: return sf::Color::Magenta;
				case OPTION: return sf::Color::Yellow;
				case VISITED: return sf::Color::White;
				case WRONG_PATH: return sf::Color::Red;
				case CORRECT_PATH: return sf::Color::Green;
				case PATH: return sf::Color(126, 126, 126);
				case WALL: return sf::Color::Black;
				default: return sf::Color::White;
				}
			}
		);
	}

	void onUnload() override 
	{
		zui::destroy(btn_gen);
		zui::destroy(sld_size);
		zui::destroy(tb_size);
		zui::destroy(tb_info);
	}

	float maze_size;
	zui::TextButton_ptr btn_gen;
	zui::Slider_ptr sld_size;
	zui::Textbox_ptr tb_size, tb_info;
	
	sf::Vector2i start_pos, end_pos;
};