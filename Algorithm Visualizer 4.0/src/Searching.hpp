#pragma once
#include "OneDAlgo.hpp"

ONE_D_ALGORITHMS_FOR(Searching)
{
	SETUP_ALGO(2, "Linear", "Binary");

	CREATE_ALGO(0, {
			int i;
			vec.addIndexTracker(&i, sf::Color::White);

			vec.startRecording();

			for (i = 0; i < vec.size(); i++) {
				if (vec[i] == find_value)break;
				vec.saveIndexTrackers();
			}
			
			vec.removeIndexTracker(&i);
			vec.setIndexColor(i, sf::Color::Green);
			vec.saveIndexTrackers();

			vec.stopRecording();
		});

	CREATE_ALGO(1, {
			int i, j, smallest_i;
			for (i = 0; i < vec.size() - 1; i++) {
				smallest_i = i;
				for (j = i + 1; j < vec.size(); j++) {
					if (vec[j] < vec[smallest_i])
						smallest_i = j;
				}
				vec.swap(i, smallest_i);
			}

			int left = 0, right = vec.size() - 1, mid = (right - left)/ 2 + left;
	
			vec.addIndexTracker(&left, sf::Color::White);
			vec.addIndexTracker(&right, sf::Color::White);
			vec.addIndexTracker(&mid, sf::Color::Blue);
			
			vec.startRecording();

			while (left <= right) {
				mid = (right - left) / 2 + left;

				vec.saveIndexTrackers();

				if (find_value < vec[mid]) right = mid - 1;
				else if (find_value > vec[mid]) left = mid + 1;
				else break;
			}

			vec.removeIndexTracker(&left);
			vec.removeIndexTracker(&right);
			vec.removeIndexTracker(&mid);
			
			vec.setIndexColor(mid, sf::Color::Green);
			vec.saveIndexTrackers();

			vec.stopRecording();
		});

	void onLoad() override
	{
		find_value = vec.size() / 2;
		sld_find = zui::createFromStyleSheet<zui::Slider>("sld");
		sld_find->setVariable(find_value, 1, vec.size(), true);
		placeElement(sld_find.get(), pg_head.get(), 0, 3, "Find");

		tb_find = zui::createFromStyleSheet<zui::Textbox>("sld");
		placeElement(tb_find.get(), pg_head.get(), 1, 3, std::to_string((int)find_value));

		sld_find->attachAction(
			[this] {
				tb_find->setString(std::to_string((int)find_value));
			}
		);
	}

	void onUnload() override
	{
		zui::destroy(sld_find);
		zui::destroy(tb_find);
	}

	void onShuffle() override
	{
		find_value = vec.size() / 2;
		sld_find->setVariable(find_value, 1, vec.size(), true);
		tb_find->setString(std::to_string((int)find_value));
	}

	float find_value;

	zui::Slider_ptr sld_find;
	zui::Textbox_ptr tb_find;
};
