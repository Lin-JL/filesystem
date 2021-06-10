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

constexpr int SYSTEM_SIZE = 16 * M;				// System总大小
constexpr int BLOCK_SIZE = 1 * K;				// Block空间大小
constexpr int NUM_BLOCK_INODE = 5;				// INode 的 block个数
constexpr int NUM_INODE_DIRBLOCK = 10;			// INode 中 direct的个数
constexpr int NUM_INODE_INDBLICK = 1;			// INode 中 indirect的个数
constexpr int DIR_ENTRY_LENGTH = 32;              // Dir Block中一个Entry的长度
constexpr int NAME_LENGTH = 28;                   // File/Dir名字长度

const std::string SYSTEM_FILE = "system.dat";   // 保存系统的文件