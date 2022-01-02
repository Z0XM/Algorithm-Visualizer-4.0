#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <string>

class VisVec {
public:
	VisVec();
	VisVec(const VisVec&) = default;
	VisVec(const std::vector<int>&);

	int operator[] (int index) const;

	std::vector<int>::iterator begin();
	std::vector<int>::iterator end();

	void clearEverything();
	void clearBuffer();
	void swapAndClearBuffer();

	void resize(int new_size);

	void clear();

	void push_back(int value);
	void push_front(int value);

	int pop_back();
	int pop_front();

	void insert(int index, int value);
	int erase(int index);

	unsigned int size();

	void fill(int size, int value);
	void randomize(int size);
	void randomize();

	void setPositionMapper(const std::function<sf::Vector2f(int, int)>& lambda_index_value);
	void setSizeMapper(const std::function<sf::Vector2f(int, int)>& lambda_index_value);
	void setColorMapper(const std::function<sf::Color(int, int)>& lambda_index_value);

	sf::Vector2f getSize(int index) const;
	sf::Vector2f getPosition(int index) const;

	void addIndexTracker(int* index, const sf::Color& fillColor);
	void setIndexColor(int index, const sf::Color& fillColor);
	void removeIndexTracker(int* index);

	void swap(int first, int second);
	void assign(int index, int value);

	void saveIndexTrackers();
	void turnOffIndexTracker(int* index);
	void turnOnIndexTracker(int* index);

	void draw(sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default);

	void startRecording();
	void stopRecording();
	void pauseRecording();
	void resumeRecording();
	void setPlaying(bool canPlay);
	bool getPlaying();
	void trigger();
	bool isAnimating();
	void play(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default);
	void playLoop(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default);

	void forceUpdateDrawBuffer();

private:

	void nextFrame();

private:
	std::vector<int> m_vector, m_playBuffer;

	enum class OperationState {
		 PAUSE, RESUME, INDEX, PUSHF, PUSHB, POPF, POPB, INSERT, ERASE, CLEAR, SWAP, ASSIGN
	};
	std::queue<OperationState> m_operationStates;
	std::queue<std::pair<int, int>> m_operationValues;
	std::queue<std::unordered_map<int, int>> m_indexTrackerValues;
	std::unordered_map<int, int> m_index_id_map;

	std::queue<OperationState> m_loopBuffer_operationStates;
	std::queue<std::pair<int, int>> m_loopBuffer_operationValues;
	std::queue<std::unordered_map<int, int>> m_loopBuffer_indexTrackerValues;

	std::vector<int> m_doesIndexHasColorMapped;

	struct {
		std::function<sf::Vector2f(int, int)> position;
		std::function<sf::Vector2f(int, int)> size;
		std::function<sf::Color(int, int)> color;
	} m_mapper;

	int id_count;
	std::unordered_map<int*, std::pair<int, bool>> m_indexTracker_id_map;
	std::unordered_map<int, sf::Color> m_id_color_map;

	sf::Clock m_clock;
	bool m_recording;
	bool m_can_play;
	bool m_play_trigger;
};
