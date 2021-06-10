#include "DiskSystem.h"

DiskSystem::DiskSystem()
{

	/* ϵͳ����ռ� */
	system_start_addr = (char*)malloc(SYSTEM_SIZE * sizeof(char));

	/* ����super_block��ַ */
	super_block = (SuperBlock*)system_start_addr;
	super_block->setRestBlocks(NUM_DATA_BLOCKS);

	/* ����inode_bitmap��ַ */
	inode_bitmap = system_start_addr + OFFSET_INODE_BITMAP * BLOCK_SIZE;
	constexpr int inode_bitmap_size = (NUM_INODES - 1) / 8 + 1;
	char* tmp_inode_bitmap = new char[inode_bitmap_size];
	memcpy(inode_bitmap, tmp_inode_bitmap, inode_bitmap_size * sizeof(char));
	memset(inode_bitmap, 0, inode_bitmap_size * sizeof(char));
	delete[] tmp_inode_bitmap;

	/* ����data_bitmap��ַ */
	data_bitmap = system_start_addr + OFFSET_DATA_BITMAP * BLOCK_SIZE;
	constexpr int data_bitmap_size = (NUM_DATA_BLOCKS - 1) / 8 + 1;
	char* tmp_data_bitmap = new char[data_bitmap_size];
	memcpy(data_bitmap, tmp_data_bitmap, data_bitmap_size * sizeof(char));
	memset(data_bitmap, 0, data_bitmap_size * sizeof(char));
	delete[] tmp_data_bitmap;


	/* ��ʼ��i-node */
	inodes = (INode*)(system_start_addr + OFFSET_INODES * BLOCK_SIZE);
	INode* tmp_inodes = new INode[NUM_INODES];
	memcpy(inodes, tmp_inodes, NUM_INODES * sizeof(INode));
	delete[] tmp_inodes;

	/* ����data_block��ʼ��ַ */
	data_start_addr = system_start_addr + OFFSET_DATA * BLOCK_SIZE;

	/* �����ʼ�� */
	/* ��ʼ����Ŀ¼/root/ */
	int root_inode_id = getFreeINodeID();
	// cout << "root" << root_inode_id << endl;
	setINodeBitmap(root_inode_id, 1);
	//initINode(root_inode_id, "DIR", "root", getCurrTime().c_str(), -1, -1);
	INode& root = inodes[root_inode_id];
	root = INode("DIR", getCurrTime().c_str(), -1);

	/* ���Դ��ļ��лָ����� */
	systemReload();

	/* ��ӭ���� */
	welcomeMessage();
}

DiskSystem::~DiskSystem()
{
	// ����ϵͳ(All blocks)
	FILE* fp;
	errno_t err = fopen_s(&fp, SYSTEM_FILE.c_str(), "wb");
	fwrite(system_start_addr, sizeof(char), SYSTEM_SIZE, fp);
	fclose(fp);

	// �ͷſռ�
	free(system_start_addr);
}

void DiskSystem::systemReload()
{
	// �ӱ����ļ��ж�ȡ
	FILE* fp;
	errno_t err = fopen_s(&fp, SYSTEM_FILE.c_str(), "rb");
	if (err == 0) {
		fread(system_start_addr, sizeof(char), SYSTEM_SIZE, fp);
		fclose(fp);
	}
}

char* DiskSystem::getSystemStartAddr()
{
	return system_start_addr;
}

SuperBlock* DiskSystem::getSuperBlock()
{
	return super_block;
}

char* DiskSystem::getDataBlockAddrByID(int block_id)
{
	return data_start_addr + (block_id * BLOCK_SIZE);
}

INode* DiskSystem::getINodeAddrByID(int inode_id)
{
	return inodes + inode_id;
}

int DiskSystem::getDataBlockIDByAddr(char* block_addr)
{
	return (block_addr - data_start_addr) / BLOCK_SIZE;
}

void DiskSystem::setINodeBitmap(const int id, const int value)
{
	int num_char = id / 8;
	int offset = id % 8;
	char mask = 0x80 >> offset;
	if (value == 0) {
		inode_bitmap[num_char] &= ~mask;
	}
	else if (value == 1) {
		inode_bitmap[num_char] |= mask;
	}
}

int DiskSystem::getINodeBitmap(const int id)
{
	int num_char = id / 8;
	int offset = id % 8;
	char mask = 0x80 >> offset;
	return (inode_bitmap[num_char] & mask) == 0 ? 0 : 1;
}

void DiskSystem::setDataBlockBitmap(const int id, const int value)
{
	int num_char = id / 8;
	int offset = id % 8;
	char mask = 0x80 >> offset;
	if (value == 0) {
		data_bitmap[num_char] &= ~mask;
	}
	else if (value == 1) {
		data_bitmap[num_char] |= mask;
	}
}

void DiskSystem::setDataBlockBitmap(char* block, const int value)
{
	int id = getDataBlockIDByAddr(block);
	setDataBlockBitmap(id, value);
}

int DiskSystem::getDataBlockBitmap(const int id)
{
	int num_char = id / 8;
	int offset = id % 8;
	char mask = 0x80 >> offset;
	return (data_bitmap[num_char] & mask) == 0 ? 0 : 1;
}

int DiskSystem::getFreeINodeID()
{
	int free_node_id = -1;
	for (int i = 0; i < NUM_INODES; ++i) {
		if (getINodeBitmap(i) == 0) {
			free_node_id = i;
			break;
		}
	}
	return free_node_id;
}

int DiskSystem::getFreeDataBlockID()
{
	int free_data_block_id = -1;
	for (int i = 0; i < NUM_DATA_BLOCKS; ++i) {
		if (getDataBlockBitmap(i) == 0) {
			free_data_block_id = i;
			break;
		}
	}
	return free_data_block_id;
}

void DiskSystem::welcomeMessage()
{
	cout << "*****************************************************************************" << endl;
	cout << "*                                 File System                               *" << endl;
	cout << "*                    Developer: Jielong Line 201830640248                   *" << endl;
	cout << "*                    Developer:  Cehao Yang  xxxxxxxxxxxx                   *" << endl;
	cout << "*                    Developer:   Yixun Gu   xxxxxxxxxxxx                   *" << endl;
	cout << "*                    Copyright 2021, All Rights Reserved                    *" << endl;
	cout << "*****************************************************************************" << endl;
	cout << "*               1. Create a file: createFile <fileName> <fileSize>          *" << endl;
	cout << "*               2. Delete a file: deleteFile <fileName>                     *" << endl;
	cout << "*               3. Create a directory: createDir <dirName>                  *" << endl;
	cout << "*               4. Delete a directory: deleteDir <dirName>                  *" << endl;
	cout << "*               5. Change directory: changeDir <dirName>                    *" << endl;
	cout << "*               6. List all the files: dir                                  *" << endl;
	cout << "*               7. Copy a file: cp <exsitedFile> <newFile>                  *" << endl;
	cout << "*               8. Display usage: sum                                       *" << endl;
	cout << "*               9. Print file content: cat <filename>                       *" << endl;
	cout << "*               10. Show current absolute path: pwd                         *" << endl;
	cout << "*               11. Show this message: help                                 *" << endl;
	cout << "*               12. Shut down this system: exit                             *" << endl;
	cout << "*****************************************************************************" << endl;
}
