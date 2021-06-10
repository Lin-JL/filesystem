#pragma once

class SuperBlock {
private:
	int magic;
	int nblocks;
	int ninodeblocks;
	int ninodes;
	int restBlocks;

public:
	int getRestBlocks() const {
		return restBlocks;
	}

	void setRestBlocks(int num) {
		restBlocks = num;
	}

	void addRestBlocks(int num) {
		restBlocks += num;
	}

	void removeRestBlocks(int num) {
		restBlocks -= num;
	}
};