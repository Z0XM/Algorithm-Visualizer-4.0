#include "OneDAlgo.hpp"
#include <iostream>

OneDAlgo::OneDAlgo()
{
}

void OneDAlgo::loadGUI()
{
	pg_visvec->setScroll(zui::Page::ScrollPlacement::BOTTOM, 25);

	btn_shuffle = zui::createFromStyleSheet<zui::TextButton>("red btn");
	placeElement(btn_shuffle.get(), pg_head.get(), 0, 0, "Shuffle");

	btn_reload = zui::copy(btn_shuffle);
	placeElement(btn_reload.get(), pg_head.get(), 1, 0, "Reload");

	sld_width = zui::copy(sld_speed);
	placeElement(sld_width.get(), pg_head.get(), 0, 1, "Width");

	tb_info = zui::createFromStyleSheet<zui::Textbox>("sld");
	placeElement(tb_info.get(), pg_head.get(), 1, 3, "");

	btn_shuffle->setAction([this]()
		{
			vec.clearBuffer();
			vec.randomize(1000 / (1 + barWidth));
			vec.forceUpdateDrawBuffer();

			createBackupVector();

			pg_visvec->setPosition(25, 25);
			pg_visvec->setMaxSize({ (barWidth + 1) * vec.size() , 450 });
			pg_visvec->setActiveRegion({ 0, pg_visvec->getActiveRegionPosition().y, 1000, 450 });
			pg_visvec->setScroll(zui::Page::ScrollPlacement::BOTTOM, 25);

			btn_pause->setString("Pause");
			vec.setPlaying(true);

			onShuffle();
		}
	);
	btn_reload->setAction([this]()
		{
			loadBackupVector();

			btn_pause->setString("Pause");
			vec.setPlaying(true);

			onReload();
		}
	);
	sld_width->setVariable(barWidth, 2.f, 50.f);


	sld_width->attachAction([this]
		{
			float p_x = pg_visvec->getActiveRegion().left, p_w_2 = pg_visvec->getActiveRegion().width * 0.5f;

			float i = (vec.size() * (p_x + p_w_2)) / pg_visvec->getMaxSize().x;

			pg_visvec->setMaxSize({ (barWidth + 1) * vec.size() , 450 });
			pg_visvec->setPosition(25 + std::max(0.f, 1000.f - pg_visvec->getMaxSize().x) * 0.5f, 25.f);

			pg_visvec->setActiveRegion(sf::FloatRect(pg_visvec->getActiveRegionPosition(), sf::Vector2f(1000, pg_visvec->getActiveRegion().height)));

			pg_visvec->moveActiveRegion(i * (barWidth + 1) - p_w_2 - p_x, 0.0f);

			pg_visvec->setScroll(zui::Page::ScrollPlacement::BOTTOM, 25);

			onWidthChange();
		}
	);

	pg_visvec->setMouseMoveAction([this]
		{
			sf::Vector2f mousePos = pg_visvec->getLocalMousePosition();
			int index = (mousePos.x) / (1 + barWidth);
			index = std::min(index, (int)vec.size() - 1);
			index = std::max(index, 0);
			tb_info->setString(std::to_string(vec.getBufferValue(index)));
		}
	);
}

void OneDAlgo::loadVisVec()
{
	vec.setColorMapper([this](int index, int value) {return sf::Color::Red; });

	vec.setSizeMapper([this](int index, int value) {
		return sf::Vector2f(barWidth, (pg_visvec->getMaxSize().y * value) / vec.size());
		});
	vec.setPositionMapper([this](int index, int value) {
		return pg_visvec->getPosition() +
			sf::Vector2f(
				(1 + barWidth) * index - pg_visvec->getActiveRegionPosition().x, 
				pg_visvec->getMaxSize().y - (pg_visvec->getMaxSize().y * value) / vec.size()
			);
		});

	vec.clearBuffer();
	vec.randomize(1000 / (1 + barWidth));
	vec.forceUpdateDrawBuffer();

	createBackupVector();
}

void OneDAlgo::load()
{
	loadParentElements();

	barWidth = 2.f;

	loadGUI();
	loadVisVec();

	onLoad();
}

void OneDAlgo::unload()
{
	unloadParentElements();

	zui::destroy(btn_shuffle);
	zui::destroy(btn_reload);
	zui::destroy(sld_width);
	zui::destroy(tb_info);

	onUnload();
}

void OneDAlgo::onLoad()
{

}

void OneDAlgo::onUnload()
{

}

void OneDAlgo::onShuffle()
{
}

void OneDAlgo::onReload()
{
}

void OneDAlgo::onWidthChange()
{
}