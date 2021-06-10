#pragma once

#include "util.h"
#include "INode.h"
#include "SuperBlock.h"

using namespace std;

/*
 * Block Structure of Filesystem:
 *
 * Superblock | INode Bitmap | Data Bitmap | INode | Data Block
 *     1      |      1       |      16     |   5   |    ..
 */

constexpr int NUM_INODES = NUM_BLOCK_INODE * BLOCK_SIZE / sizeof(INode);	// i-node总个数
constexpr int NUM_BLOCKS = SYSTEM_SIZE / BLOCK_SIZE;						// Block总个数
constexpr int NUM_BLOCK_INODE_BITMAP = (NUM_INODES - 1) / (8 * BLOCK_SIZE) + 1;	// INode Bitmap 的 block个数
constexpr int NUM_BLOCK_DATA_BITMAP = (NUM_BLOCKS - 1) / (8 * BLOCK_SIZE) + 1;	// Data Bitmap 的 block个数
constexpr int INODE_ID_LENGTH = DIR_ENTRY_LENGTH - NAME_LENGTH;				// INode ID 的长度
constexpr int NUM_ENTRY_PER_BLOCK = BLOCK_SIZE / DIR_ENTRY_LENGTH;			// Block中directory entry数量
constexpr int NUM_IND_TO_DIR = BLOCK_SIZE / INODE_ID_LENGTH;				// indirect到direct数量
constexpr int INODE_MAX_NUM_BLOCKS = NUM_INODE_DIRBLOCK
			+ NUM_INODE_INDBLICK * NUM_IND_TO_DIR + NUM_INODE_INDBLICK;		// INODE所有BLOCK数量

constexpr int OFFSET_INODE_BITMAP = 1;												// INode Bitmap Offset
constexpr int OFFSET_DATA_BITMAP = OFFSET_INODE_BITMAP + NUM_BLOCK_INODE_BITMAP;    // Data Bitmap Offset
constexpr int OFFSET_INODES = OFFSET_DATA_BITMAP + NUM_BLOCK_DATA_BITMAP;			// INode Offset
constexpr int OFFSET_DATA = OFFSET_INODES + NUM_BLOCK_INODE;						// Data Offset
constexpr int NUM_DATA_BLOCKS = NUM_BLOCKS - OFFSET_DATA;							// Data Block个数


class DiskSystem {
public:
	DiskSystem();		// 构造函数
	~DiskSystem();		// 析构函数
	void systemReload();	// 从磁盘中恢复数据
	char* getSystemStartAddr(); // 获取系统起始地址
	SuperBlock* getSuperBlock();
	char* getDataBlockAddrByID(int block_id);	// 根据ID获取Data Block地址
	int getDataBlockIDByAddr(char* block_addr);	// 根据地址获取Data BlockID
	INode* getINodeAddrByID(int inode_id);		// 根据ID获取I-Node地址
	void setINodeBitmap(const int id, const int value); // 对I-Node Bitmap赋值
	int getINodeBitmap(const int id);	// 获取I-Node Bitmap的值
	void setDataBlockBitmap(const int id, const int value); // 对Data Block Bitmap赋值
	void setDataBlockBitmap(char* block, const int value); 
	int getDataBlockBitmap(const int id);	// 获取Data Block Bitmap的值
	int getFreeINodeID();	// 获取空闲I-Node ID
	int getFreeDataBlockID(); // 获取空闲Data Block ID

	void welcomeMessage();

	INode* inodes;				// i-node地址
private:
	char* system_start_addr;	// 整个系统起始地址
	SuperBlock* super_block;	// super block地址
	char* inode_bitmap;			// i-node bitmap地址
	char* data_bitmap;			// data block bitmap地址
	char* data_start_addr;		// data block地址
};
