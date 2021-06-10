#pragma once
#include<string>

#define K 1024
#define M K*K

/*
 * Block Structure of Filesystem: 
 *
 * Superblock | INode Bitmap | Data Bitmap | INode | Data Block
 *     1      |      1       |      16     |   5   |     .. 
 */

constexpr int SYSTEM_SIZE = 16 * M;				// System�ܴ�С
constexpr int BLOCK_SIZE = 1 * K;				// Block�ռ��С
constexpr int NUM_BLOCK_INODE = 5;				// INode �� block����
constexpr int NUM_INODE_DIRBLOCK = 10;			// INode �� direct�ĸ���
constexpr int NUM_INODE_INDBLICK = 1;			// INode �� indirect�ĸ���
constexpr int DIR_ENTRY_LENGTH = 32;              // Dir Block��һ��Entry�ĳ���
constexpr int NAME_LENGTH = 28;                   // File/Dir���ֳ���

const std::string SYSTEM_FILE = "system.dat";   // ����ϵͳ���ļ�