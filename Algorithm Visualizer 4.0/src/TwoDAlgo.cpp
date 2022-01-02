#include "TwoDAlgo.hpp"

TwoDAlgo::TwoDAlgo()
{
}

void TwoDAlgo::load()
{
	loadParentElements();
	loadGUI();
	loadVisVec();

	onLoad();
}

void TwoDAlgo::unload()
{
	unloadParentElements();

	onUnload();
}

void TwoDAlgo::loadGUI()
{
	
}

void TwoDAlgo::loadVisVec()
{
	rows = 10;
	cols = 20;
	
	vec.setColorMapper([this](int index, int value) {return sf::Color::Red; });

	vec.setSizeMapper([this](int index, int value) {
		return sf::Vector2f(pg_visvec->getMaxSize().x / cols, pg_visvec->getMaxSize().y / rows);
	});

	vec.setPositionMapper([this](int index, int value) {
		int j = index % cols;
		int i = index / cols;
		sf::Vector2f size(vec.getSize(index));
		return 
			pg_visvec->getPosition() + 
			sf::Vector2f(
				(j - cols * 0.5f) * size.x + pg_visvec->getMaxSize().x * 0.5f + size.x * 0.5f,
				(i - rows * 0.5f) * size.y + pg_visvec->getMaxSize().y * 0.5f + size.y * 0.5f
			);
	});

	vec.fill(rows * cols, 0);

	vec_copy.resize(vec.size());
	std::copy(vec.begin(), vec.end(), vec_copy.begin());

	vec.forceUpdateDrawBuffer();
}

int TwoDAlgo::getVec2d(int row, int col)
{
	return vec[row * cols + col];
}

void TwoDAlgo::setVec2d(int row, int col, int value)
{
	vec.assign(row * cols + col, value);
}

void TwoDAlgo::onLoad()
{
}

void TwoDAlgo::onUnload()
{
}
