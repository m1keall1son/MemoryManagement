#pragma once

class Test {

public:

	Test() = default;
	Test(int val) { mVal = val; }
	void setVal(int val) { mVal = val; }
	int getVal() const { return mVal; }

private:

	int mVal;

};

unsigned int randInt(unsigned int max) {
	double mix = (double)rand() / (double)RAND_MAX;
	return (unsigned int)floor((mix * (double)max));
}