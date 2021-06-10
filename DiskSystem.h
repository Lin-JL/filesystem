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

constexpr int NUM_INODES = NUM_BLOCK_INODE * BLOCK_SIZE / sizeof(INode);	// i-node�ܸ���
constexpr int NUM_BLOCKS = SYSTEM_SIZE / BLOCK_SIZE;						// Block�ܸ���
constexpr int NUM_BLOCK_INODE_BITMAP = (NUM_INODES - 1) / (8 * BLOCK_SIZE) + 1;	// INode Bitmap �� block����
constexpr int NUM_BLOCK_DATA_BITMAP = (NUM_BLOCKS - 1) / (8 * BLOCK_SIZE) + 1;	// Data Bitmap �� block����
constexpr int INODE_ID_LENGTH = DIR_ENTRY_LENGTH - NAME_LENGTH;				// INode ID �ĳ���
constexpr int NUM_ENTRY_PER_BLOCK = BLOCK_SIZE / DIR_ENTRY_LENGTH;			// Block��directory entry����
constexpr int NUM_IND_TO_DIR = BLOCK_SIZE / INODE_ID_LENGTH;				// indirect��direct����
constexpr int INODE_MAX_NUM_BLOCKS = NUM_INODE_DIRBLOCK
			+ NUM_INODE_INDBLICK * NUM_IND_TO_DIR + NUM_INODE_INDBLICK;		// INODE����BLOCK����

constexpr int OFFSET_INODE_BITMAP = 1;												// INode Bitmap Offset
constexpr int OFFSET_DATA_BITMAP = OFFSET_INODE_BITMAP + NUM_BLOCK_INODE_BITMAP;    // Data Bitmap Offset
constexpr int OFFSET_INODES = OFFSET_DATA_BITMAP + NUM_BLOCK_DATA_BITMAP;			// INode Offset
constexpr int OFFSET_DATA = OFFSET_INODES + NUM_BLOCK_INODE;						// Data Offset
constexpr int NUM_DATA_BLOCKS = NUM_BLOCKS - OFFSET_DATA;							// Data Block����


class DiskSystem {
public:
	DiskSystem();		// ���캯��
	~DiskSystem();		// ��������
	void systemReload();	// �Ӵ����лָ�����
	char* getSystemStartAddr(); // ��ȡϵͳ��ʼ��ַ
	SuperBlock* getSuperBlock();
	char* getDataBlockAddrByID(int block_id);	// ����ID��ȡData Block��ַ
	int getDataBlockIDByAddr(char* block_addr);	// ���ݵ�ַ��ȡData BlockID
	INode* getINodeAddrByID(int inode_id);		// ����ID��ȡI-Node��ַ
	void setINodeBitmap(const int id, const int value); // ��I-Node Bitmap��ֵ
	int getINodeBitmap(const int id);	// ��ȡI-Node Bitmap��ֵ
	void setDataBlockBitmap(const int id, const int value); // ��Data Block Bitmap��ֵ
	void setDataBlockBitmap(char* block, const int value); 
	int getDataBlockBitmap(const int id);	// ��ȡData Block Bitmap��ֵ
	int getFreeINodeID();	// ��ȡ����I-Node ID
	int getFreeDataBlockID(); // ��ȡ����Data Block ID

	void welcomeMessage();

	INode* inodes;				// i-node��ַ
private:
	char* system_start_addr;	// ����ϵͳ��ʼ��ַ
	SuperBlock* super_block;	// super block��ַ
	char* inode_bitmap;			// i-node bitmap��ַ
	char* data_bitmap;			// data block bitmap��ַ
	char* data_start_addr;		// data block��ַ
};
