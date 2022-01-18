#include "AlgoClass.hpp"

AlgoClass::AlgoClass()
{
}

void AlgoClass::init(sf::RenderWindow& window)
{
	frm_main.setWindow(window);

	setupAlgoNames();

	current_algo = -1;
}

void AlgoClass::placeElement(zui::Textbox* tb, zui::Page* pg, float x, float y, const std::string& str)
{
	int gapX = 25, gapY = 10;
	tb->setPosition(gapX + x * (gapX + tb->getSize().x), gapY + y * (gapY + tb->getSize().y));
	tb->setString(str);

	pg->addEntity(tb);
}

void AlgoClass::loadParentElements()
{
	speed = 0.f;

	pg_head = zui::create<zui::Page>();
	pg_head->setFillColor(sf::Color(255, 255, 255, 78));
	pg_head->setMaxSize({ 400, 500 });
	pg_head->setPosition(1050, 0);
	pg_head->setActiveRegion({ 0,0, 400, 250 });
	pg_head->setScroll(zui::Page::ScrollPlacement::RIGHT);

	sld_speed = zui::createFromStyleSheet<zui::Slider>("sld");
	placeElement(sld_speed.get(), pg_head.get(), 1, 1, "Speed");

	btn_pause = zui::createFromStyleSheet<zui::TextButton>("green btn");
	placeElement(btn_pause.get(), pg_head.get(), 0, 2, "Pause");

	btn_step = zui::createFromStyleSheet<zui::TextButton>("green btn");
	placeElement(btn_step.get(), pg_head.get(), 1, 2, "Step");


	pg_algo = zui::create<zui::Page>();
	pg_algo->setFillColor(sf::Color::Black);
	pg_algo->setMaxSize({ 400, 500 });
	pg_algo->setPosition(1050, 250);
	pg_algo->setActiveRegion({ 0,0, 400, 250 });
	pg_algo->setScroll(zui::Page::ScrollPlacement::RIGHT);

	zui::TextButton sort;
	zui::StyleSheet::styleGroup["purple btn"].applyTo(sort);

	for (int i = 0; i < algoNames.size(); i++) {
		btn_algo.push_back(zui::copy(sort));
		placeElement(btn_algo[i].get(), pg_algo.get(), i % 2, i / 2, algoNames[i]);
	}


	pg_visvec = zui::create<zui::Page>();
	pg_visvec->setFillColor(sf::Color::Transparent);
	pg_visvec->setMaxSize({ 1000, 450 });
	pg_visvec->setPosition(25, 25);
	pg_visvec->setActiveRegion({ 0,0, 1000, 450 });

	frm_main.addEntity(pg_head.get());
	frm_main.addEntity(pg_algo.get());
	frm_main.addEntity(pg_visvec.get());
	for (int i = 0; i < btn_algo.size(); i++)
		frm_main.push_in_navigationOrder(*btn_algo[i].get());

	sld_speed->setVariable(speed, 3.f, 0.f);

	btn_pause->setAction([this]()
		{
			if (vec.getPlaying()) {
				btn_pause->setString("Play");
				vec.setPlaying(false);
			}
			else {
				btn_pause->setString("Pause");
				vec.setPlaying(true);
			}
		}
	);

	btn_step->setAction([this]()
		{
			vec.trigger();
		}
	);

	runSetActionLoop();


	bound.setSize(sf::Vector2f(1010, 460));
	bound.setFillColor(sf::Color::Transparent);
	bound.setOutlineColor(sf::Color::White);
	bound.setOutlineThickness(3);
	bound.setPosition(20, 20);
}

void AlgoClass::unloadParentElements()
{
	frm_main.removeEntity(pg_head.get());
	frm_main.removeEntity(pg_algo.get());
	frm_main.removeEntity(pg_visvec.get());
	frm_main.clear_navigationOrder();

	zui::destroy(pg_head);
	zui::destroy(pg_algo);
	zui::destroy(pg_visvec);
	zui::destroy(btn_pause);
	zui::destroy(btn_step);
	zui::destroy(sld_speed);
	for (int i = 0; i < algoNames.size(); i++)
		zui::destroy(btn_algo[i]);
	btn_algo.clear();
}

void AlgoClass::createBackupVector()
{
	vec_copy.resize(vec.size());
	std::copy(vec.begin(), vec.end(), vec_copy.begin());
}
void AlgoClass::loadBackupVector()
{
	vec.clearBuffer();
	std::copy(vec_copy.begin(), vec_copy.end(), vec.begin());
	vec.forceUpdateDrawBuffer();
}


zui::Frame& AlgoClass::getFrame()
{
	return frm_main;
}

void AlgoClass::draw(sf::RenderWindow& window, sf::RenderStates states)
{
	vec.play(sf::seconds(speed), window);
	if (!vec.isAnimating())
		current_algo = -1;

	window.draw(bound);

	frm_main.draw();
}

void AlgoClass::addAlgoName(const std::string& name)
{
	algoNames.push_back(name);
}