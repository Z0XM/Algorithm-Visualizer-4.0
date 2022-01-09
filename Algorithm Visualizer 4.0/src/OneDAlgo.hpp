#pragma once
#include "AlgoClass.hpp"

#define ONE_D_ALGORITHMS_FOR(type) class type : public OneDAlgo

class OneDAlgo : public AlgoClass {
public:
	OneDAlgo();

	void load() override;
	void unload() override;

protected:
	void loadGUI() override;
	void loadVisVec() override;

	virtual void setupAlgoNames() override = 0;
	virtual void runSetActionLoop() override = 0;

	virtual void onLoad();
	virtual void onUnload();
	virtual void onShuffle();
	virtual void onReload();
	virtual void onWidthChange();

	float barWidth;	

	zui::TextButton_ptr btn_shuffle, btn_reload;
	zui::Slider_ptr sld_width;
	zui::Textbox_ptr tb_info;
};


