#include "FileManager.h"

vector<string> FileManager::processPath(string path)
{
	vector<string> splited_path;
	if (path.size() == 0) {
		return splited_path;
	}
	if (path[0] == '/') {
		splited_path.push_back("/");
	}
	char* path_c = new char[path.size() + 1];
	strcpy_s(path_c, path.size() + 1 , path.c_str());
	char* buf;
	char* p = strtok_s(path_c, "/", &buf);
	while (p) {
		if (strcmp(p, ".")) {
			splited_path.push_back(p);
		}
		p = strtok_s(NULL, "/", &buf);
	}
	delete[] path_c;
	return splited_path;
}

bool FileManager::getINodeMap(int dir_inode_id, unordered_map<string, int>& inode_map)
{
	INode node = disk.inodes[dir_inode_id];
	if (node.getType() != "DIR") {
		return false;
	}
	inode_map.clear();
	vector<int> blocks_id = getINodeBlocks(dir_inode_id);
	for (int block_id : blocks_id) {
		getINodeMapInBlock(block_id, inode_map);
	}
	return true;
}

void FileManager::getINodeMapInBlock(int block_id, unordered_map<string, int>& inode_map)
{
	if (block_id == -1) {
		return;
	}
	int i = 0;
	char* p = disk.getDataBlockAddrByID(block_id);
	while (*p != EOF && i < NUM_ENTRY_PER_BLOCK) {
		char name[NAME_LENGTH + 1];
		memcpy(name, p, NAME_LENGTH);
		name[NAME_LENGTH] = '\0';
		char id_c[INODE_ID_LENGTH];
		memcpy(id_c, p + NAME_LENGTH, INODE_ID_LENGTH);
		int id = bits2Int(id_c);
		inode_map[name] = id;
		i++;
		p += DIR_ENTRY_LENGTH;
	}
}

bool FileManager::setINodeMap(int dir_inode_id, const unordered_map<string, int>& inode_map)
{
	INode& dir_inode = disk.inodes[dir_inode_id];
	if (dir_inode.getType() != "DIR") {
		return false;
	}
	releaseBlocks(dir_inode_id);
	vector<int> blocks_id;
	auto iter = inode_map.begin();
	while (iter != inode_map.end()) {
		int block_id = disk.getFreeDataBlockID();
		disk.setDataBlockBitmap(block_id, 1);
		blocks_id.push_back(block_id);
		char* p = disk.getDataBlockAddrByID(block_id);
		int i;
		for (i = 0; i < NUM_ENTRY_PER_BLOCK && iter != inode_map.end(); i++) {
			memcpy(p, (iter->first).c_str(), NAME_LENGTH);
			char id_c[INODE_ID_LENGTH];
			int2Bits(id_c, iter->second);
			memcpy(p + NAME_LENGTH, id_c, INODE_ID_LENGTH);
			++iter;
			p += DIR_ENTRY_LENGTH;
		}
		if (i < NUM_ENTRY_PER_BLOCK) {
			*p = EOF;
			break;
		}
	}
	setINodeBlocks(dir_inode_id, blocks_id);
	return true;
}

void FileManager::addEntry(int dir_inode_id, const char* name, int inode_id)
{
	unordered_map<string, int> inode_map;
	getINodeMap(dir_inode_id, inode_map);
	inode_map[name] = inode_id;
	setINodeMap(dir_inode_id, inode_map);
}

void FileManager::removeEntry(int dir_inode_id, const char* name)
{
	unordered_map<string, int> inode_map;
	getINodeMap(dir_inode_id, inode_map);
	inode_map.erase(name);
	setINodeMap(dir_inode_id, inode_map);
}

void FileManager::randFillBlock(int block_id, int size)
{
	char* block = disk.getDataBlockAddrByID(block_id);
	size = min(size, BLOCK_SIZE);
	char* rand_str = new char[size];
	for (int i = 0; i < size; i++) {
		rand_str[i] = 32 + (rand() % (127 - 32));
	}
	memcpy(block, rand_str, size);
}

string FileManager::getINodeData(int inode_id)
{
	string str;
	const INode& node = disk.inodes[inode_id];
	vector<int> blocks_id = getINodeBlocks(inode_id);
	for (int i = 0; i < blocks_id.size(); i++) {
		int block_id = blocks_id[i];
		char* p = disk.getDataBlockAddrByID(block_id);
		char buf[BLOCK_SIZE + 1];
		memcpy(buf, p, BLOCK_SIZE);
		buf[BLOCK_SIZE] = '\0';
		str += buf;
	}
	return str;
}

void FileManager::releaseBlocks(int inode_id)
{
	INode& inode = disk.inodes[inode_id];
	SuperBlock* superblock = disk.getSuperBlock();
	for (int i = 0; i < NUM_INODE_DIRBLOCK; i++) {
		int block_id = inode.getDirect(i);
		if (block_id == -1) {
			continue;
		}
		disk.setDataBlockBitmap(block_id, 0);
		inode.setDirect(i, -1);
		superblock->addRestBlocks(1);
	}
	for (int i = 0; i < NUM_INODE_INDBLICK; i++) {
		int ind_block_id = inode.getIndirect(i);
		if (ind_block_id == -1) {
			continue;
		}
		char* p = disk.getDataBlockAddrByID(ind_block_id);
		for (int i = 0; i < NUM_IND_TO_DIR && *p != EOF; i++) {
			char id_c[INODE_ID_LENGTH];
			memcpy(id_c, p, INODE_ID_LENGTH);
			int block_id = bits2Int(id_c);
			disk.setDataBlockBitmap(block_id, 0);
			superblock->addRestBlocks(1);
			p += INODE_ID_LENGTH;
		}
		disk.setDataBlockBitmap(ind_block_id, 0);
		inode.setIndirect(i, -1);
		superblock->addRestBlocks(1);
	}
}

bool FileManager::setINodeBlocks(int inode_id, const vector<int>& blocks_id)
{
	int n = blocks_id.size();
	if (n > INODE_MAX_NUM_BLOCKS - NUM_INODE_INDBLICK) {
		return false;
	}
	releaseBlocks(inode_id);
	INode& inode = disk.inodes[inode_id];
	SuperBlock* superblock = disk.getSuperBlock();
	int idx = 0;
	for (int i = 0; i < min(n, NUM_INODE_DIRBLOCK); i++) {
		inode.setDirect(i, blocks_id[idx]);
		disk.setDataBlockBitmap(blocks_id[idx], 1);
		superblock->removeRestBlocks(1);
		++idx;
	}
	for (int i = 0; i < NUM_INODE_INDBLICK; i++) {
		if (idx == n) {
			return true;
		}
		int ind_block_id = disk.getFreeDataBlockID();
		disk.setDataBlockBitmap(ind_block_id, 1);
		inode.setIndirect(i, ind_block_id);
		superblock->removeRestBlocks(1);
		char* p = disk.getDataBlockAddrByID(ind_block_id);
		for (int j = 0; j < NUM_IND_TO_DIR; j++) {
			if (idx == n) {
				*p = EOF;
				return true;
			}
			disk.setDataBlockBitmap(blocks_id[idx], 1);
			superblock->removeRestBlocks(1);
			char id_c[INODE_ID_LENGTH];
			int2Bits(id_c, blocks_id[idx]);
			memcpy(p, id_c, INODE_ID_LENGTH);
			++idx;
			p += INODE_ID_LENGTH;
		}
	}
	return true;
}

vector<int> FileManager::getINodeBlocks(int inode_id)
{
	vector<int> blocks_id;
	const INode& inode = disk.inodes[inode_id];
	for (int i = 0; i < NUM_INODE_DIRBLOCK; i++) {
		int block_id = inode.getDirect(i);
		if (block_id == -1) {
			continue;
		}
		blocks_id.push_back(block_id);
	}
	for (int i = 0; i < NUM_INODE_INDBLICK; i++) {
		int ind_block_id = inode.getIndirect(i);
		if (ind_block_id == -1) {
			continue;
		}
		char* p = disk.getDataBlockAddrByID(ind_block_id);
		for (int i = 0; i < NUM_IND_TO_DIR && *p != EOF; i++) {
			char id_c[INODE_ID_LENGTH];
			memcpy(id_c, p, INODE_ID_LENGTH);
			int block_id = bits2Int(id_c);
			blocks_id.push_back(block_id);
			p += INODE_ID_LENGTH;
		}
	}
	return blocks_id;
}

FileManager::FileManager()
{
	curr_dir_inode_id = 0;
}

FileManager::~FileManager()
{
}

int FileManager::getNode(const vector<string>& path)
{
	int n = path.size();
	if (n == 0) {
		return curr_dir_inode_id;
	}
	int i = 0;
	int node_id = curr_dir_inode_id;
	if (path[0] == "/") {
		node_id = 0;
		i++;
	}
	while (i < n) {
		unordered_map<string, int> inode_map;
		if (!getINodeMap(node_id, inode_map) || !inode_map.count(path[i])) {
			return -1;
		}
		node_id = inode_map[path[i]];
		i++;
	}
	return node_id;
}

void FileManager::createFile(const char* file_name, const int file_size)
{
	vector<string> path = processPath(file_name);
	if (getNode(path) != -1) {
		cout << "cannot createFile '" << file_name << "': File exists";
		return;
	}
	if (path.size() == 0) {
		return;
	}
	string name = path.back();
	path.pop_back();
	int dir_inode_id = getNode(path);
	if (dir_inode_id == -1) {
		cout << "cannot createFile '" << file_name << "': No such file or directory";
		return;
	}
	constexpr int max_size = INODE_MAX_NUM_BLOCKS - NUM_INODE_INDBLICK;
	if (file_size > max_size) {
		cout << "cannot createFile '" << file_name << "': The file is too large: at most " << max_size << "KB";
		return;
	}
	if (file_size > disk.getSuperBlock()->getRestBlocks()) {
		cout << "cannot createFile '" << file_name << "': Insufficient space";
		return;
	}
	int inode_id = disk.getFreeINodeID();
	if (inode_id == -1) {
		cout << "cannot createFile '" << file_name << "': The number of file/directory reaches the limit";
		return;
	}
	INode& inode = disk.inodes[inode_id];
	inode = INode("FILE", getCurrTime().c_str(), file_size);
	addEntry(dir_inode_id, name.c_str(), inode_id);
	disk.setINodeBitmap(inode_id, 1);
	vector<int> blocks_id(file_size);
	srand((unsigned)time(NULL));
	for (int i = 0; i < file_size; i++) {
		int block_id = disk.getFreeDataBlockID();
		disk.setDataBlockBitmap(block_id, 1);
		blocks_id[i] = block_id;
		randFillBlock(block_id, BLOCK_SIZE);
	}
	setINodeBlocks(inode_id, blocks_id);
}

void FileManager::deleteFile(const char* file_name)
{
	vector<string> path = processPath(file_name);
	int file_inode_id = getNode(path);
	if (file_inode_id == -1) {
		cout << "cannot deleteFile '" << file_name << "': No such file or directory";
		return;
	}
	if (disk.inodes[file_inode_id].getType() == "DIR") {
		cout << "cannot deleteFile '" << file_name << "': Is a directory";
		return;
	}
	releaseBlocks(file_inode_id);
	disk.setINodeBitmap(file_inode_id, 0);
	string name = path.back();
	path.pop_back();
	int dir_inode_id = getNode(path);
	removeEntry(dir_inode_id, name.c_str());
}

void FileManager::createDirectory(const char* dir_name)
{
	vector<string> path = processPath(dir_name);
	if (getNode(path) != -1) {
		cout << "cannot createDir '" << dir_name << "': File exists";
		return;
	}
	if (path.size() == 0) {
		return;
	}
	string name = path.back();
	path.pop_back();
	int dir_prt_node_id = getNode(path);
	if (dir_prt_node_id == -1) {
		cout << "cannot createDir '" << dir_name << "': No such file or directory";
		return;
	}
	int inode_id = disk.getFreeINodeID();
	int block_id = disk.getFreeDataBlockID();
	if (inode_id == -1 || block_id == -1) {
		cout << "cannot createDir '" << dir_name << "': The number of file/directory reaches the limit";
		return;
	}
	INode& inode = disk.inodes[inode_id];
	inode = INode("DIR", getCurrTime().c_str(), -1);
	addEntry(dir_prt_node_id, name.c_str(), inode_id);
	disk.setINodeBitmap(inode_id, 1);
	addEntry(inode_id, "..", dir_prt_node_id);
	disk.setDataBlockBitmap(block_id, 1);
}

void FileManager::deleteDirectory(const char* dir_name)
{
	vector<string> path = processPath(dir_name);
	if (path.size() == 0 || (path.size() == 1 && path[0] == "..")) {
		cout << "refusing to remove ¡®.¡¯ or ¡®..¡¯";
		return;
	}
	int dir_inode_id = getNode(path);
	if (dir_inode_id == 0) {
		cout << "cannot delete /";
		return;
	}
	if (dir_inode_id == -1) {
		cout << "cannot deleteDir '" << dir_name << "': No such file or directory";
		return;
	}
	INode& dir_inode = disk.inodes[dir_inode_id];
	if (dir_inode.getType() != "DIR") {
		cout << "cannot deleteDir '" << dir_name << "': Is not a directory";
		return;
	}
	unordered_map<string, int> inode_map;
	getINodeMap(dir_inode_id, inode_map);
	int prt_inode_id = inode_map[".."];
	inode_map.erase("..");
	for (auto iter = inode_map.begin(); iter != inode_map.end(); iter++) {
		INode& inode = disk.inodes[iter->second];
		string name = dir_name;
		name += "/" + iter->first;
		if (inode.getType() == "FILE") {
			deleteFile(name.c_str());
		}
		else {
			deleteDirectory(name.c_str());
		}
	}
	releaseBlocks(dir_inode_id);
	disk.setINodeBitmap(dir_inode_id, 0);
	removeEntry(prt_inode_id, path.back().c_str());
}

void FileManager::changeDirectory(const char* dir_name)
{
	vector<string> path = processPath(dir_name);
	int inode_id = getNode(path);
	if (inode_id == -1) {
		cout << "cannot changeDir '" << dir_name << "': No such file or directory";
		return;
	}
	if (disk.inodes[inode_id].getType() != "DIR") {
		cout << "cannot changeDir '" << dir_name << "': Not a directory";
	}
	curr_dir_inode_id = inode_id;
}

void FileManager::copyFile(const char* file_name1, const char* file_name2)
{
	vector<string> path1 = processPath(file_name1);
	int file_inode_id1 = getNode(path1);
	if (file_inode_id1 == -1) {
		cout << "cannot cp '" << file_name1 << "': No such file or directory";
		return;
	}
	const INode& inode1 = disk.inodes[file_inode_id1];
	int file_size = inode1.getSize();
	if (inode1.getType() == "DIR") {
		cout << "cannot cp '" << file_name1 << "': Is a directory";
		return;
	}
	vector<string> path2 = processPath(file_name2);
	if (getNode(path2) != -1) {
		cout << "cannot cp '" << file_name1 << "' to '" << file_name2 << "': File exists";
		return;
	}
	if (path2.size() == 0) {
		return;
	}
	string name2 = path2.back();
	path2.pop_back();
	int dir_node_id = getNode(path2);
	if (dir_node_id == -1) {
		cout << "cannot cp '" << file_name1 << "' to '" << file_name2 << "': No such file or directory";
		return;
	}
	if (file_size > disk.getSuperBlock()->getRestBlocks()) {
		cout << "cannot cp '" << file_name1 << "': Insufficient space";
		return;
	}

	int inode_id2 = disk.getFreeINodeID();
	if (inode_id2 == -1) {
		cout << "cannot cp '" << file_name1 << "': The number of file/directory reaches the limit";
		return;
	}
	INode& inode2 = disk.inodes[inode_id2];
	inode2 = INode("FILE", getCurrTime().c_str(), file_size);
	addEntry(dir_node_id, name2.c_str(), inode_id2);
	disk.setINodeBitmap(inode_id2, 1);
	vector<int> blocks1_id = getINodeBlocks(file_inode_id1);
	vector<int> blocks2_id(file_size);
	for (int i = 0; i < file_size; i++) {
		int block1_id = blocks1_id[i];
		int block2_id = disk.getFreeDataBlockID();
		disk.setDataBlockBitmap(block2_id, 1);
		blocks2_id[i] = block2_id;
		char* p1 = disk.getDataBlockAddrByID(block1_id);
		char* p2 = disk.getDataBlockAddrByID(block2_id);
		memcpy(p2, p1, BLOCK_SIZE);
	}
	setINodeBlocks(inode_id2, blocks2_id);
	return;
}

stack<string> FileManager::getCurrentPath()
{
	stack<string> path_stack;
	int inode_id = curr_dir_inode_id;
	int prt_inode_id;
	while (inode_id != 0) {
		const INode& inode = disk.inodes[curr_dir_inode_id];
		unordered_map<string, int> inode_map;
		getINodeMap(inode_id, inode_map);
		prt_inode_id = inode_map[".."];
		getINodeMap(prt_inode_id, inode_map);
		for (auto iter = inode_map.begin(); iter != inode_map.end(); ++iter) {
			if (iter->second == inode_id) {
				path_stack.push(iter->first);
				break;
			}
		}
		inode_id = prt_inode_id;
	}
	return path_stack;
}

void FileManager::listAll()
{
	vector<string> name_list;
	vector<string> type_list;
	vector<string> size_list;
	vector<string> time_created_list;
	const int dir_inode_id = curr_dir_inode_id;
	unordered_map<string, int> inode_map;
	getINodeMap(dir_inode_id, inode_map);
	for (auto iter = inode_map.begin(); iter != inode_map.end(); ++iter) {
		name_list.push_back(iter->first);
		int inode_id = iter->second;
		const INode& inode = disk.inodes[inode_id];
		type_list.push_back(inode.getType());
		time_created_list.push_back(inode.getTimeCreated());
		int size = inode.getSize();
		if (size == -1) {
			size_list.push_back("-");
		}
		else {
			size_list.push_back(to_string(inode.getSize()) + "KB");
		}
	}
	cout << left
		<< setw(20) << "Name"
		<< setw(10) << "Type"
		<< setw(10) << "Size"
		<< "Time Created"
		<< endl;
	for (int i = 0; i < 59; ++i) {
		cout << "-";
	}
	cout << endl;
	for (int i = 0; i < name_list.size(); ++i) {
		cout << left
			<< setw(20) << name_list[i]
			<< setw(10) << type_list[i]
			<< setw(10) << size_list[i]
			<< time_created_list[i]
			<< endl;
	}
}

void FileManager::printFileContents(const char* file_name)
{
	vector<string> path = processPath(file_name);
	int inode_id = getNode(path);
	if (inode_id == -1) {
		cout << "cannot cat '" << file_name << "': No such file or directory";
		return;
	}
	if (disk.inodes[inode_id].getType() == "DIR") {
		cout << "cannot cat '" << file_name << "': Is a directory";
		return;
	}
	cout << getINodeData(inode_id);
}

void FileManager::displayUsage()
{
	int used_count = 0;
	int unused_count = 0;
	int bar_cnt = 0;
	unused_count = disk.getSuperBlock()->getRestBlocks();
	used_count = NUM_DATA_BLOCKS - unused_count;
	double usage = (double)(used_count + OFFSET_DATA) / (double)NUM_BLOCKS;
	bar_cnt = 25 * usage;
	cout << endl;
	cout << "Space usage: ";
	cout << "[";
	for (int i = 0; i < 25; ++i)
	{
		if (i < bar_cnt)
		{
			cout << "=";
		}
		else
		{
			cout << " ";
		}

	}
	cout << setprecision(3);
	cout << "] " << usage * 100 << " %  " << usage * 16 << " MB / " << "16 MB" << endl;
	cout << left << "Used blocks: " << setw(6) << used_count
		<< "Unused blocks: " << setw(6) << unused_count << endl;
}

DiskSystem* FileManager::getDisk()
{
	return &disk;
}
