#include "VisVec.hpp"
#include <algorithm>
#include <chrono>
#include <random>

VisVec::VisVec()
	:id_count(0), m_recording(false), m_can_play(true)
{
}

VisVec::VisVec(const std::vector<int>& vector)
	:id_count(0), m_recording(false), m_can_play(true)
{
	m_vector.resize(vector.size());
	std::copy(vector.begin(), vector.end(), m_vector.begin());
}
int VisVec::operator[](int index) const
{
	return m_vector[index];
}

std::vector<int>::iterator VisVec::begin()
{
	return m_vector.begin();
}
std::vector<int>::iterator VisVec::end()
{
	return m_vector.end();
}

unsigned int VisVec::size()
{
	return m_vector.size();
}

void VisVec::resize(int new_size)
{
	m_vector.resize(new_size);
	m_playBuffer.resize(new_size);
}

void VisVec::clearEverything()
{
	m_vector.clear();
	m_playBuffer.clear();

	while (!m_operationStates.empty())m_operationStates.pop();
	while (!m_indexTrackerValues.empty())m_indexTrackerValues.pop();
	while (!m_operationValues.empty())m_operationValues.pop();

	while (!m_loopBuffer_operationStates.empty())m_loopBuffer_operationStates.pop();
	while (!m_loopBuffer_indexTrackerValues.empty())m_loopBuffer_indexTrackerValues.pop();
	while (!m_loopBuffer_operationValues.empty())m_loopBuffer_operationValues.pop();

	m_doesIndexHasColorMapped.clear();
	m_id_color_map.clear();
	m_indexTracker_id_map.clear();

	id_count = 0; 
	m_recording = false; 
	m_can_play = true;
}

void VisVec::clearBuffer()
{
	m_playBuffer.clear();

	while (!m_operationStates.empty())m_operationStates.pop();
	while (!m_indexTrackerValues.empty())m_indexTrackerValues.pop();
	while (!m_operationValues.empty())m_operationValues.pop();

	while (!m_loopBuffer_operationStates.empty())m_loopBuffer_operationStates.pop();
	while (!m_loopBuffer_indexTrackerValues.empty())m_loopBuffer_indexTrackerValues.pop();
	while (!m_loopBuffer_operationValues.empty())m_loopBuffer_operationValues.pop();

	m_doesIndexHasColorMapped.clear();
	m_id_color_map.clear();
	m_indexTracker_id_map.clear();
	id_count = 0;
}

void VisVec::swapAndClearBuffer()
{
	std::swap(m_vector, m_playBuffer);
	clearBuffer();
}

void VisVec::clear()
{
	m_vector.clear();
	if (m_recording) {
		m_operationStates.push(OperationState::CLEAR);
	}
}

void VisVec::push_back(int value)
{
	m_vector.push_back(value);
	if (m_recording) {
		m_operationStates.push(OperationState::PUSHB);
		m_operationValues.push(std::make_pair(-1, value));
	}
}
void VisVec::push_front(int value)
{
	m_vector.insert(m_vector.begin(), value);
	if (m_recording) {
		m_operationStates.push(OperationState::PUSHF);
		m_operationValues.push(std::make_pair(-1, value));
	}
}

int VisVec::pop_back()
{
	int t = m_vector.back();

	m_vector.pop_back();
	if (m_recording) {
		m_operationStates.push(OperationState::POPB);
	}

	return t;
}

int VisVec::pop_front()
{
	int t = m_vector[0];
	
	m_vector.erase(m_vector.begin());
	if (m_recording) {
		m_operationStates.push(OperationState::POPF);
	}

	return t;
}

void VisVec::insert(int index, int value)
{
	m_vector.insert(m_vector.begin() + index, value);
	if (m_recording) {
		m_operationStates.push(OperationState::INSERT);
		m_operationValues.push(std::make_pair(index, value));
	}
}

int VisVec::erase(int index)
{
	int t = m_vector[index];

	m_vector.erase(m_vector.begin() + index);
	if (m_recording) {
		m_operationStates.push(OperationState::ERASE);
		m_operationValues.push(std::make_pair(index, -1));
	}

	return t;
}

void VisVec::setPositionMapper(const std::function<sf::Vector2f(int, int)>& lambda)
{
	m_mapper.position = lambda;
}

void VisVec::setSizeMapper(const std::function<sf::Vector2f(int, int)>& lambda)
{
	m_mapper.size = lambda;
}

void VisVec::setColorMapper(const std::function<sf::Color(int, int)>& lambda)
{
	m_mapper.color = lambda;
}

sf::Vector2f VisVec::getSize(int index) const
{
	return m_mapper.size(index, m_vector[index]);
}

sf::Vector2f VisVec::getPosition(int index) const
{
	return m_mapper.position(index, m_vector[index]);
}

void VisVec::addIndexTracker(int* index, const sf::Color& fillColor)
{
	int id;
	bool already_exists = false;
	for (auto it = m_id_color_map.begin(); it != m_id_color_map.end(); it++) {
		if (it->second == fillColor) {
			id = it->first;
			already_exists = true;
			break;
		}
	}
	if (!already_exists) {
		id = id_count++;
		m_id_color_map[id] = fillColor;
	}

	m_indexTracker_id_map[index] = std::make_pair(id, true);
	
}

void VisVec::setIndexColor(int index, const sf::Color& fillColor)
{
	int id;
	bool already_exists = false;
	for (auto it = m_id_color_map.begin(); it != m_id_color_map.end(); it++) {
		if (it->second == fillColor) {
			id = it->first;
			already_exists = true;
			break;
		}
	}
	if (!already_exists) {
		id = id_count++;
		m_id_color_map[id] = fillColor;
	}

	if (index >= 0 && index < m_vector.size())
		m_index_id_map[index] = id;
}

void VisVec::removeIndexTracker(int* index)
{
	//int id = m_indexTracker_id_map[index];
	//m_id_color_map.erase(id);
	m_indexTracker_id_map.erase(index);
}

void VisVec::turnOffIndexTracker(int* index)
{
	if (m_recording) {
		m_indexTracker_id_map.at(index).second = false;
		pauseRecording();
		saveIndexTrackers();
		resumeRecording();
	}
}

void VisVec::turnOnIndexTracker(int* index)
{
	if (m_recording) {
		m_indexTracker_id_map.at(index).second = true;
		pauseRecording();
		saveIndexTrackers();
		resumeRecording();
	}
}

void VisVec::swap(int first, int second)
{
	std::swap(m_vector[first], m_vector[second]);
	if (m_recording) {
		m_operationStates.push(OperationState::SWAP);
		m_operationValues.push(std::make_pair(first, second));
	}
}

void VisVec::assign(int index, int value)
{
	m_vector[index] = value;
	if(m_recording){
		m_operationStates.push(OperationState::ASSIGN);
		m_operationValues.push(std::make_pair(index, value));
	}
}

void VisVec::fill(int size, int value)
{
	m_vector.resize(size);
	for (int i = 0; i < size; i++)
		m_vector[i] = value;
}

void VisVec::randomize(int size)
{
	m_vector.clear();
	for (int i = 0; i < size; i++)
		m_vector.push_back(i + 1);

	std::shuffle(m_vector.begin(), m_vector.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

	if (m_recording) {
		m_operationStates.push(OperationState::CLEAR);
		for (int i = 0; i < m_vector.size(); i++) {
			m_operationStates.push(OperationState::PUSHB);
			m_operationValues.push(std::make_pair(-1, m_vector[i]));
		}
	}
}

void VisVec::randomize()
{
	std::shuffle(m_vector.begin(), m_vector.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

	if (m_recording) {
		m_operationStates.push(OperationState::CLEAR);
		for (int i = 0; i < m_vector.size(); i++) {
			m_operationStates.push(OperationState::PUSHB);
			m_operationValues.push(std::make_pair(- 1, m_vector[i]));
		}
	}
}

void VisVec::saveIndexTrackers()
{
	if (m_recording) {
		m_indexTrackerValues.push(std::unordered_map<int, int>());
		for (auto it = m_indexTracker_id_map.begin(); it != m_indexTracker_id_map.end(); it++) {
			if (it->second.second && *it->first >= 0 && *it->first < m_vector.size())
				m_indexTrackerValues.back()[*it->first] = it->second.first;
		}

		for (auto it = m_index_id_map.begin(); it != m_index_id_map.end(); it++) {
			m_indexTrackerValues.back()[it->first] = it->second;
		}

		m_index_id_map.clear();

		m_operationStates.push(OperationState::INDEX);
	}
}

void VisVec::draw(sf::RenderWindow& window, sf::RenderStates states)
{
	sf::RectangleShape rect;
	for (int i = 0; i < m_playBuffer.size(); i++) {
		
		rect.setFillColor(m_mapper.color(i, m_playBuffer[i]));
		rect.setSize(m_mapper.size(i, m_playBuffer[i]));
		rect.setPosition(m_mapper.position(i, m_playBuffer[i]));

		rect.setOrigin(rect.getSize() * 0.5f);

		if (i < m_doesIndexHasColorMapped.size() && m_doesIndexHasColorMapped[i] != -1) {
			rect.setFillColor(m_id_color_map[m_doesIndexHasColorMapped[i]]);
		}
		window.draw(rect);
	}
}

void VisVec::startRecording()
{
	m_recording = true;
	forceUpdateDrawBuffer();

	while (!m_operationStates.empty())m_operationStates.pop();
	while (!m_indexTrackerValues.empty())m_indexTrackerValues.pop();
	while (!m_operationValues.empty())m_operationValues.pop();
}
void VisVec::stopRecording()
{
	m_recording = false;

	m_loopBuffer_operationStates = m_operationStates;
	m_loopBuffer_operationValues = m_operationValues;
	m_loopBuffer_indexTrackerValues = m_indexTrackerValues;
}
void VisVec::pauseRecording()
{
	if (m_recording) {
		if (m_operationStates.back() != OperationState::PAUSE)
			m_operationStates.push(OperationState::PAUSE);
	}
}
void VisVec::resumeRecording()
{
	if (m_recording) {
		if (m_operationStates.back() != OperationState::RESUME)
			m_operationStates.push(OperationState::RESUME);
	}
}

void VisVec::nextFrame()
{
	using enum OperationState;

	OperationState drawState = m_operationStates.front();
	m_operationStates.pop();

	std::pair<int, int> pair;

	switch (drawState) {
	case PUSHF: case PUSHB: case INSERT: case ERASE: case SWAP: case ASSIGN:
		pair = m_operationValues.front();
		m_operationValues.pop();
	}

	switch (drawState)
	{
	case PUSHF:
		m_playBuffer.insert(m_playBuffer.begin(), pair.second);
		break;
	case PUSHB:
		m_playBuffer.push_back(pair.second);
		break;
	case POPF:
		m_playBuffer.erase(m_playBuffer.begin());
		break;
	case POPB:
		m_playBuffer.pop_back();
		break;
	case INSERT:
		m_playBuffer.insert(m_playBuffer.begin() + pair.first, pair.second);
		break;
	case ERASE:
		m_playBuffer.erase(m_playBuffer.begin() + pair.first);
		break;
	case CLEAR:
		m_playBuffer.clear();
		break;
	case SWAP:
		std::swap(m_playBuffer[pair.first], m_playBuffer[pair.second]);
		break;
	case ASSIGN:
		m_playBuffer[pair.first] = pair.second;
		break;
	case INDEX:
		m_doesIndexHasColorMapped.clear();
		m_doesIndexHasColorMapped.resize(m_playBuffer.size(), -1);
		if (!m_indexTrackerValues.empty()) {
			for (auto it = m_indexTrackerValues.front().begin(); it != m_indexTrackerValues.front().end(); it++) {
				m_doesIndexHasColorMapped[it->first] = it->second;
			}
			m_indexTrackerValues.pop();
		}

		break;
	}
}

void VisVec::setPlaying(bool canPlay)
{
	m_can_play = canPlay;
}
bool VisVec::getPlaying()
{
	return m_can_play;
}
void VisVec::trigger()
{
	m_play_trigger = true;
}
bool VisVec::isAnimating()
{
	return m_operationStates.empty();
}
void VisVec::play(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states)
{
	if (!m_operationStates.empty()) {
		if (m_play_trigger || m_can_play && m_clock.getElapsedTime() >= duration) {
			using enum OperationState;

			while (!m_operationStates.empty() && m_operationStates.front() == PAUSE) {
				m_operationStates.pop();

				while (m_operationStates.front() != RESUME)
					nextFrame();

				m_operationStates.pop();
			}
			while (!m_operationStates.empty() && m_operationStates.front() == RESUME)
				m_operationStates.pop();

			if (!m_operationStates.empty())
				nextFrame();

			m_play_trigger = false;
			m_clock.restart();
		}
	}

	draw(window, states);
}

void VisVec::playLoop(sf::Time duration, sf::RenderWindow& window, sf::RenderStates states)
{
	if (m_operationStates.empty() && !m_loopBuffer_operationStates.empty()) {
		m_operationStates = m_loopBuffer_operationStates;
		m_operationValues = m_loopBuffer_operationValues;
		m_indexTrackerValues = m_loopBuffer_indexTrackerValues;
	}
	play(duration, window, states);
}

void VisVec::forceUpdateDrawBuffer()
{
	m_playBuffer.resize(m_vector.size());
	std::copy(m_vector.begin(), m_vector.end(), m_playBuffer.begin());
}
