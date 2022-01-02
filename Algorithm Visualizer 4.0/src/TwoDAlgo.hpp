#pragma once
#include "AlgoClass.hpp"

#define TWO_D_ALGORITHMS_FOR(type) class type : public TwoDAlgo

class TwoDAlgo : public AlgoClass {
public:
	TwoDAlgo();

	void load() override;
	void unload() override;

protected:
	void loadGUI() override;
	void loadVisVec() override;

	virtual void setupAlgoNames() override = 0;
	virtual void runSetActionLoop() override = 0;

	virtual void onLoad();
	virtual void onUnload();

	int getVec2d(int row, int col);
	void setVec2d(int row, int col, int value);

	int rows, cols;
};