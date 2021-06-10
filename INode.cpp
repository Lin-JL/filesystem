#include "INode.h"

INode::INode()
{
}

INode::INode(string type, const char* time_created, const int size)
{
	setType(type);
	setTimeCreated(time_created);
	setSize(size);
	for (int i = 0; i < NUM_INODE_DIRBLOCK; i++) {
		setDirect(i, -1);
	}
	for (int i = 0; i < NUM_INODE_INDBLICK; i++) {
		setIndirect(i, -1);
	}
}

void INode::setType(const string type)
{
	if (type == "FILE") {
		this->type = '0';
	} else if (type == "DIR") {
		this->type = '1';
	} else {
		cout << "Input type error" << endl;
	}
}

string INode::getType() const
{
	if (type == '0') {
		return "FILE";
	} else if (type == '1') {
		return "DIR";
	}
}

void INode::setTimeCreated(const char* time)
{
	memcpy(time_created, time, 20 * sizeof(char));
}

string INode::getTimeCreated() const
{
	return time_created;
}

void INode::setSize(const int size)
{
	this->size = size;
}

int INode::getSize() const
{
	return size;
}
void INode::setDirect(const int i, int block_id)
{
	addr[i] = block_id;
}

int INode::getDirect(const int i) const
{
	return addr[i];
}

void INode::setIndirect(const int i, int block_id)
{
	if (i >= NUM_INODE_INDBLICK || i < 0) {
		return;
	}
	addr[NUM_INODE_DIRBLOCK + i] = block_id;
}

void INode::setIndirect(int block_id)
{
	addr[NUM_INODE_DIRBLOCK] = block_id;
}

int INode::getIndirect(const int i) const
{
	if (i >= NUM_INODE_INDBLICK || i < 0) {
		return -1;
	}
	return addr[NUM_INODE_DIRBLOCK + i];
}

int INode::getIndirect()
{
	return addr[NUM_INODE_DIRBLOCK];
}
