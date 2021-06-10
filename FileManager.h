#pragma once

#include<vector>
#include<string>
#include<unordered_map>
#include"DiskSystem.h"

using namespace std;

class FileManager {
private:
	DiskSystem disk;
	int curr_dir_inode_id;
	vector<string> processPath(string path);											// Split path into vector
	bool getINodeMap(int dir_inode_id, unordered_map<string, int>& inode_map);			// Get inode map in directory
	void getINodeMapInBlock(int block_id, unordered_map<string, int>& inode_map);		// Get inode map in a dir block
	bool setINodeMap(int dir_inode_id, const unordered_map<string, int>& inode_map);	// Set inode map of directory
	void addEntry(int dir_inode_id, const char* name, int inode_id);					// Add directory entry
	void removeEntry(int dir_inode_id, const char* name);								// Remove directory entry
	void randFillBlock(int block_id, int size);											// Random fill a block
	string getINodeData(int inode_id);													// Get data of a block			
	void releaseBlocks(int inode_id);													// Release a inode ans its blocks
	bool setINodeBlocks(int inode_id, const vector<int>& blocks_id);					// Set all data blocks of a inode
	vector<int> getINodeBlocks(int inode_id);											// Get all data blocks of a inode

public:
	FileManager();
	~FileManager();
	int getNode(const vector<string>& path);
	void createFile(const char* file_name, const int file_size); // Create a file
	void deleteFile(const char* file_name); // Delete a file
	void createDirectory(const char* dir_name); // Create a directory
	void deleteDirectory(const char* dir_name); // Delete a directory
	void changeDirectory(const char* dir_name); // Change current directory
	void copyFile(const char* file_name_1, const char* file_name_2); // Copy a file
	stack<string> getCurrentPath(); // Get current working absolute path
	void listAll(); // List all the files and sub-directories under current working directory
	void printFileContents(const char* file_name); // Print out the file contents
	void displayUsage(); // Display the usage of storage space
	DiskSystem* getDisk();
};

