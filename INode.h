#pragma once

#include<string.h>
#include "util.h"

using namespace std;

class INode {
public:
	INode();
	INode(string type, const char* time_created, const int size);
	void setType(const string type);
	string getType() const;
	void setTimeCreated(const char* time);
	string getTimeCreated() const;
	void setSize(const int size);
	int getSize() const;
	void setDirect(const int i, int block_id);
	int getDirect(const int i) const;
	void setIndirect(const int i, int block);
	void setIndirect(int block);
	int getIndirect(const int i) const;
	int getIndirect();

private:
	char type;
	char time_created[20];
	int size;
	int addr[NUM_INODE_DIRBLOCK + NUM_INODE_INDBLICK];
};