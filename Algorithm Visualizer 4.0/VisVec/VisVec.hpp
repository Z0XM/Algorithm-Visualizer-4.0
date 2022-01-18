#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <functional>
#include <unordered_map>
#include <string>

#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>

#include <iostream>

class VisVec {
public:
	VisVec()
		:m_recording(false), m_can_play(true)
	{
	}

	VisVec(const std::vector<int>& vector)
		: m_recording(false), m_can_play(true)
	{
		m_vector.resize(vector.size());
		std::copy(vector.begin(), vector.end(), m_vector.begin());
	}

	VisVec(const VisVec&) = default;

	int operator[](int index) const
	{
		return m_vector[index];
	}

	std::vector<int>::iterator begin()
	{
		return m_vector.begin();
	}
	std::vector<int>::iterator end()
	{
		return m_vector.end();
	}

	void clearEverything()
	{
		m_vector.clear();
		m_playBuffer.clear();

		m_operationStates.clear();
		m_indexTrackerValues.clear();
		m_operationValues.clear();

		m_loopBuffer_operationStates.clear();
		m_loopBuffer_indexTrackerValues.clear();
		m_loopBuffer_operationValues.clear();

		m_indexTracker_color_map.clear();

		m_recording = false;
		m_can_play = true;
	}

	void clearBuffer()
	{
		m_playBuffer.clear();

		m_operationStates.clear();
		m_indexTrackerValues.clear();
		m_operationValues.clear();

		m_loopBuffer_operationStates.clear();
		m_loopBuffer_indexTrackerValues.clear();
		m_loopBuffer_operationValues.clear();

		m_indexTracker_color_map.clear();
	}

	void swapAndClearBuffer()
	{
		std::swap(m_vector, m_playBuffer);
		clearBuffer();
	}

	int getBufferValue(int index)
	{
		return m_playBuffer[index];
	}

	void resize(int new_size)
	{
		m_vector.resize(new_size);
		m_playBuffer.resize(new_size);
	}

	void clear()
	{
		m_vector.clear();
		if (m_recording) {
			m_operationStates.push_back(OperationState::CLEAR);
		}
	}

	void push_back(int value)
	{
		m_vector.push_back(value);
		if (m_recording) {
			m_operationStates.push_back(OperationState::PUSHB);
			m_operationValues.push_back(std::make_pair(-1, value));
		}
	}
	void push_front(int value)
	{
		m_vector.insert(m_vector.begin(), value);
		if (m_recording) {
			m_operationStates.push_back(OperationState::PUSHF);
			m_operationValues.push_back(std::make_pair(-1, value));
		}
	}

	int pop_back()
	{
		int t = m_vector.back();

		m_vector.pop_back();
		if (m_recording) {
			m_operationStates.push_back(OperationState::POPB);
		}

		return t;
	}
	int pop_front()
	{
		int t = m_vector[0];

		m_vector.erase(m_vector.begin());
		if (m_recording) {
			m_operationStates.push_back(OperationState::POPF);
		}

		return t;
	}

	void insert(int index, int value)
	{
		m_vector.insert(m_vector.begin() + index, value);
		if (m_recording) {
			m_operationStates.push_back(OperationState::INSERT);
			m_operationValues.push_back(std::make_pair(index, value));
		}
	}
	int erase(int index)
	{
		int t = m_vector[index];

		m_vector.erase(m_vector.begin() + index);
		if (m_recording) {
			m_operationStates.push_back(OperationState::ERASE);
			m_operationValues.push_back(std::make_pair(index, -1));
		}

		return t;
	}

	unsigned int size()
	{
		return m_vector.size();
	}

	int max() {
		return *max_element(m_vector.begin(), m_vector.end());
	}
	int min() {
		return *min_element(m_vector.begin(), m_vector.end());
	}

	void fill(int size, int value)
	{
		m_vector.resize(size);
		for (int i = 0; i < size; i++)
			m_vector[i] = value;
	}
	void randomize(int size)
	{
		m_vector.clear();
		for (int i = 0; i < size; i++)
			m_vector.push_back(i + 1);

		std::shuffle(m_vector.begin(), m_vector.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

		if (m_recording) {
			m_operationStates.push_back(OperationState::CLEAR);
			for (int i = 0; i < m_vector.size(); i++) {
				m_operationStates.push_back(OperationState::PUSHB);
				m_operationValues.push_back(std::make_pair(-1, m_vector[i]));
			}
		}
	}
	void randomize()
	{
		std::shuffle(m_vector.begin(), m_vector.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

		if (m_recording) {
			m_operationStates.push_back(OperationState::CLEAR);
			for (int i = 0; i < m_vector.size(); i++) {
				m_operationStates.push_back(OperationState::PUSHB);
				m_operationValues.push_back(std::make_pair(-1, m_vector[i]));
			}
		}
	}

	void setPositionMapper(const std::function<sf::Vector2f(int, int)>& lambda)
	{
		m_mapper.position = lambda;
	}
	void setSizeMapper(const std::function<sf::Vector2f(int, int)>& lambda)
	{
		m_mapper.size = lambda;
	}
	void setColorMapper(const std::function<sf::Color(int, int)>& lambda)
	{
		m_mapper.color = lambda;
	}

	sf::Vector2f getSize(int index) const
	{
		return m_mapper.size(index, m_vector[index]);
	}
	sf::Vector2f getPosition(int index) const
	{
		return m_mapper.position(index, m_vector[index]);
	}

	void addIndexTracker(int* index, const sf::Color& fillColor)
	{
		m_indexTracker_color_map[index] = std::make_pair(fillColor, true);
	}
	void setIndexColor(int index, const sf::Color& fillColor)
	{
		if (index >= 0 && index < m_vector.size())
			m_index_color_map[index] = fillColor;
	}
	void removeIndexTracker(int* index)
	{
		//int id = m_indexTracker_color_map[index];
		//m_id_color_map.erase(id);
		m_indexTracker_color_map.erase(index);
	}

	void swap(int first, int second)
	{
		std::swap(m_vector[first], m_vector[second]);
		if (m_recording) {
			m_operationStates.push_back(OperationState::SWAP);
			m_operationValues.push_back(std::make_pair(first, second));
		}
	}
	void assign(int index, int value)
	{
		m_vector[index] = value;
		if (m_recording) {
			m_operationStates.push_back(OperationState::ASSIGN);
			m_operationValues.push_back(std::make_pair(index, value));
		}
	}
	void saveFrame()
	{
		if (m_recording) {
			m_indexTrackerValues.push_back(std::unordered_map<int, sf::Color>());
			for (auto it = m_indexTracker_color_map.begin(); it != m_indexTracker_color_map.end(); it++) {
				if (it->second.second && *it->first >= 0 && *it->first < m_vector.size())
					m_indexTrackerValues.back()[*it->first] = it->second.first;
			}

			for (auto it = m_index_color_map.begin(); it != m_index_color_map.end(); it++) {
				m_indexTrackerValues.back()[it->first] = it->second;
			}

			m_index_color_map.clear();

			m_operationStates.push_back(OperationState::INDEX);
		}
	}

	void turnOffIndexTracker(int* index)
	{
		if (m_recording) {
			m_indexTracker_color_map.at(index).second = false;
			pauseRecording();
			saveFrame();
			resumeRecording();
		}
	}
	void turnOnIndexTracker(int* index)
	{
		if (m_recording) {
			m_indexTracker_color_map.at(index).second = true;
			pauseRecording();
			saveFrame();
			resumeRecording();
		}
	}


	void startRecording()
	{
		m_recording = true;
		forceUpdateDrawBuffer();

		m_operationStates.clear();
		m_indexTrackerValues.clear();
		m_operationValues.clear();
	}
	void stopRecording()
	{
		m_recording = false;

		m_loopBuffer_operationStates = m_operationStates;
		m_loopBuffer_operationValues = m_operationValues;
		m_loopBuffer_indexTrackerValues = m_indexTrackerValues;
	}
	void pauseRecording()
	{
		if (m_recording) {
			if (!m_operationStates.empty() && m_operationStates.back() != OperationState::PAUSE)
				m_operationStates.push_back(OperationState::PAUSE);
		}
	}
	void resumeRecording()
	{
		if (m_recording) {
			if (!m_operationStates.empty() && m_operationStates.back() != OperationState::RESUME)
				m_operationStates.push_back(OperationState::RESUME);
		}
	}

	void setPlaying(bool canPlay)
	{
		m_can_play = canPlay;
	}
	bool getPlaying()
	{
		return m_can_play;
	}
	void trigger()
	{
		m_play_trigger = true;
	}
	bool isAnimating()
	{
		return !m_operationStates.empty();
	}

	void draw(sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default)
	{
		sf::RectangleShape rect;
		for (int i = 0; i < m_playBuffer.size(); i++) {
			rect.setFillColor(m_mapper.color(i, m_playBuffer[i]));
			rect.setSize(m_mapper.size(i, m_playBuffer[i]));
			rect.setPosition(m_mapper.position(i, m_playBuffer[i]));

			if (!m_indexTrackerValues.empty() && m_indexTrackerValues.front().find(i) != m_indexTrackerValues.front().end())
				rect.setFillColor(m_indexTrackerValues.front().at(i));

			window.draw(rect);
		}
	}


	void play(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default)
	{
		if (!m_operationStates.empty()) {
			if (m_play_trigger || m_can_play && m_clock.getElapsedTime() >= duration) {

				while (!m_operationStates.empty() && m_operationStates.front() == OperationState::PAUSE) {
					m_operationStates.pop_front();

					while (m_operationStates.front() != OperationState::RESUME)
						nextFrame();

					m_operationStates.pop_front();
				}
				while (!m_operationStates.empty() && m_operationStates.front() == OperationState::RESUME)
					m_operationStates.pop_front();

				if (!m_operationStates.empty())
					nextFrame();

				m_play_trigger = false;
				m_clock.restart();
			}
		}

		draw(window, states);
	}
	void playLoop(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states = sf::RenderStates::Default)
	{
		if (m_operationStates.empty() && !m_loopBuffer_operationStates.empty()) {
			m_operationStates = m_loopBuffer_operationStates;
			m_operationValues = m_loopBuffer_operationValues;
			m_indexTrackerValues = m_loopBuffer_indexTrackerValues;
		}
		play(duration, window, states);
	}

	void forceUpdateDrawBuffer()
	{
		m_playBuffer.resize(m_vector.size());
		std::copy(m_vector.begin(), m_vector.end(), m_playBuffer.begin());
	}

	bool writeAnimationDataToFile(const std::string& filepath)
	{
		// playBuffer
		// opState
		// opValues
		// indexTrackerValues

		std::ofstream file(filepath, std::ofstream::trunc);

		if (!file.is_open()) return false;

		file << m_playBuffer.size() << ' ';
		for (int i = 0; i < m_playBuffer.size(); i++) {
			file << m_playBuffer[i] << ' ';
		}
		file << '\n';

		file << m_operationStates.size() << ' ';
		for (int i = 0; i < m_operationStates.size(); i++) {
			file << (int)m_operationStates[i] << ' ';
		}
		file << '\n';

		file << m_operationValues.size() << ' ';
		for (int i = 0; i < m_operationValues.size(); i++) {
			file << (int)m_operationValues[i].first << ' ' << (int)m_operationValues[i].second << ' ';
		}
		file << '\n';

		file << m_indexTrackerValues.size() << '\n';
		for (int i = 0; i < m_indexTrackerValues.size(); i++) {
			file << m_indexTrackerValues[i].size() << ' ';
			for (auto j = m_indexTrackerValues[i].begin(); j != m_indexTrackerValues[i].end(); j++) {
				file << j->first << ' ' << (int)j->second.r << ' ' << (int)j->second.g << ' ' << (int)j->second.b << ' ' << (int)j->second.a << ' ';
			}
			file << '\n';
		}

		file.close();

		return true;
	}
	bool loadAnimationDataFromFile(const std::string& filepath)
	{
		// playBuffer
		// opState
		// opValues
		// indexTrackerValues

		std::ifstream file(filepath);

		if (!file.is_open()) return false;

		int size, item;
		file >> size;

		if (size != m_vector.size()) return false;

		clearBuffer();

		for (int i = 0; i < size; i++) {
			file >> item;
			m_playBuffer.push_back(item);
		}

		file >> size;
		for (int i = 0; i < size; i++) {
			file >> item;
			m_operationStates.push_back((OperationState)item);
		}

		file >> size;
		for (int i = 0; i < size; i++) {
			int item2;
			file >> item >> item2;
			m_operationValues.push_back(std::make_pair(item, item2));
		}

		file >> size;
		for (int i = 0; i < size; i++) {
			m_indexTrackerValues.push_back(std::unordered_map<int, sf::Color>());
			
			int size2, r, g, b, a;
			file >> size2;
			for (int j = 0; j < size2; j++) {
				file >> item >> r >> g >> b >> a;

				m_indexTrackerValues.back()[item] = sf::Color(r, g, b, a);
			}

		}

		file.close();

		return true;
	}

private:

	void nextFrame()
	{
		OperationState drawState = m_operationStates.front();
		m_operationStates.pop_front();

		std::pair<int, int> pair;

		switch (drawState) {
		case OperationState::PUSHF: case OperationState::PUSHB: case OperationState::INSERT: case OperationState::ERASE: case OperationState::SWAP: case OperationState::ASSIGN:
			pair = m_operationValues.front();
			m_operationValues.pop_front();
		}

		switch (drawState)
		{
		case OperationState::PUSHF:
			m_playBuffer.insert(m_playBuffer.begin(), pair.second);
			break;
		case OperationState::PUSHB:
			m_playBuffer.push_back(pair.second);
			break;
		case OperationState::POPF:
			m_playBuffer.erase(m_playBuffer.begin());
			break;
		case OperationState::POPB:
			m_playBuffer.pop_back();
			break;
		case OperationState::INSERT:
			m_playBuffer.insert(m_playBuffer.begin() + pair.first, pair.second);
			break;
		case OperationState::ERASE:
			m_playBuffer.erase(m_playBuffer.begin() + pair.first);
			break;
		case OperationState::CLEAR:
			m_playBuffer.clear();
			break;
		case OperationState::SWAP:
			std::swap(m_playBuffer[pair.first], m_playBuffer[pair.second]);
			break;
		case OperationState::ASSIGN:
			m_playBuffer[pair.first] = pair.second;
			break;
		case OperationState::INDEX:
			if (!m_indexTrackerValues.empty()) {
				//for (auto it = m_indexTrackerValues.front().begin(); it != m_indexTrackerValues.front().end(); it++) {
				//	//m_rects[it->first].setFillColor(it->second);
				//}
				m_indexTrackerValues.pop_front();
			}

			break;
		}
	}

private:
	std::vector<int> m_vector, m_playBuffer;

	enum class OperationState {
		PAUSE, RESUME, INDEX, PUSHF, PUSHB, POPF, POPB, INSERT, ERASE, CLEAR, SWAP, ASSIGN
	};
	std::deque<OperationState> m_operationStates;
	std::deque<std::pair<int, int>> m_operationValues;
	std::deque<std::unordered_map<int, sf::Color>> m_indexTrackerValues;
	std::unordered_map<int, sf::Color> m_index_color_map;

	std::deque<OperationState> m_loopBuffer_operationStates;
	std::deque<std::pair<int, int>> m_loopBuffer_operationValues;
	std::deque<std::unordered_map<int, sf::Color>> m_loopBuffer_indexTrackerValues;

	struct {
		std::function<sf::Vector2f(int, int)> position;
		std::function<sf::Vector2f(int, int)> size;
		std::function<sf::Color(int, int)> color;
	} m_mapper;

	std::unordered_map<int*, std::pair<sf::Color, bool>> m_indexTracker_color_map;
	
	sf::Clock m_clock;
	bool m_recording;
	bool m_can_play;
	bool m_play_trigger;
};











