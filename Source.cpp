#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include <queue>
#include <bitset>
#include <thread>
#include <unordered_map>
#define CHAR_POSITIVE_INDEX(i) i + CHAR_MIN * -1
#define CHAR_NEGATIVE_INDEX(i) i * -1 
#define CHAR_INDEX_IN_RANGE(i) (i <= 0) ? CHAR_NEGATIVE_INDEX(i) :  CHAR_POSITIVE_INDEX(i)
using namespace std;

typedef  std::pair<char, uint64_t> _byteFreq;
bool Compare_ByteFreq(_byteFreq first, _byteFreq second)
{
	return first.second < second.second;
}
vector<_byteFreq>* _z_byte_counting(ifstream& descriptor, uint64_t& sizeofBuff)
{
	vector<_byteFreq>* result;
	char temp;
	result = new vector<_byteFreq>;
	sizeofBuff = 0;
	result->resize(CHAR_MAX + CHAR_MIN * -1 + 1);
	for (int16_t i = CHAR_MIN; i <= CHAR_MAX; i++)
	{
		result->at(CHAR_INDEX_IN_RANGE(i)) = std::make_pair(i,0);
	}
	while (descriptor.get(temp))
	{
		result->at(CHAR_INDEX_IN_RANGE(temp)).second++;
		sizeofBuff++;
	}
	sort(result->begin(), result->end(),Compare_ByteFreq);
	for (auto it = result->begin();it != result->end() ; it++)
	{
		if (result->begin()->second == 0 && it->second != 0)
		{
			result->erase(result->begin(),it);
			break;
		}
	}
	return result;
}
/// <summary>
/// next step - huffman tree func
/// </summary>
///
///hashtable
//typedef pair<char, string> byte_path;
//template<>
//class hash<byte_path>
//{
//	size_t operator()(const byte_path& obj) const
//	{
//		return std::hash<char>()(obj.first);
//	}
//};
typedef unordered_map<char, string> bytePathTable;

/// 
/// 
typedef struct HuffmanNode {
	HuffmanNode	*left,
				*right;
	uint64_t	 freq;
	char		 item;
}Hnode_t;
struct Compare {
	bool operator()(const Hnode_t* left, const Hnode_t* right)
	{
		return left->freq > right->freq;
	}
};
//auto Compare = [](Hnode_t* left, Hnode_t* right) { return left->freq < right->freq; };
bool Cmp(pair<char, string> left, pair<char, string> right) {return left.first > right.first; };
class Htree
{
private:
	uint16_t size;
	Hnode_t* top;
	vector<_byteFreq>* freqTable;
	vector<pair<char,string>> pathTable;
	bytePathTable pathTab;
	priority_queue<Hnode_t* ,std::vector<Hnode_t*>, Compare> build;
public:
	Htree()
	{
		freqTable = new vector<_byteFreq>;
	}
	bool Create(vector<_byteFreq>* freqTable)
	{
		Hnode_t	*tmp;
		*this->freqTable = *freqTable;
		while (!freqTable->empty())
		{
			tmp = newNode(freqTable->at(0), NULL, NULL);
			freqTable->erase(freqTable->begin());
			build.push(tmp);
		}
	
		return true;
	}
	Hnode_t* newNode(_byteFreq data, Hnode_t* left, Hnode_t* right)
	{
		Hnode_t* node = new Hnode_t;
		node->item = data.first;
		node->freq = data.second;
		node->left = left;
		node->right = right;
		return node;
	}
	void Build()
	{
		Hnode_t * top,
				* left,
				* right;
		_byteFreq temp;
		while (build.size() > 1)
		{
			left = build.top();
		    build.pop();
			right = build.top();
			build.pop();
			temp.first = '*';
			temp.second = left->freq + right->freq;
			top = newNode(temp, left, right);
			build.push(top);
		}
		this->top = build.top();
		build.pop();
	}
	bool Insert(_byteFreq data)
	{
		Hnode_t* node,
			   * temp;
		_byteFreq tmp;
		node = newNode(data, NULL, NULL);
		temp = this->top;
		tmp.first = NULL;
		tmp.second = temp->freq + data.second;
		this->top = newNode(tmp, NULL, NULL);
		if (temp->freq < data.second)
		{
			this->top->left = temp;
			this->top->right = node;
		}
		else
		{
			this->top->left = node;
			this->top->right = temp;
		}
		//delete node;
		return true;
	}
	void prePrint()
	{

		char* path = (char*)calloc(256, sizeof(char));


		tree_print(this->top, 0, path, 0);
	}

	void tree_print(Hnode_t* tree, int depth, char* path, int direction)
	{

		if (tree == NULL)
			return;


		depth++;


		tree_print(tree->right, depth, path, 1);


		path[depth - 2] = 0;

		if (direction)
			path[depth - 2] = 1;

		if (tree->left)
			path[depth - 1] = 1;

		printf("\n");

		for (int i = 0; i < depth - 1; i++)
		{
			if (i == depth - 2)
				printf("+");
			else if (path[i])
				printf("|");
			else
				printf(" ");

			for (int j = 1; j < 5; j++)
				if (i < depth - 2)
					printf(" ");
				else
					printf("-");
		}

		printf("(%c :%d)\n", tree->item, tree->freq);

		for (int i = 0; i < depth; i++)
		{
			if (path[i])
				printf("|");
			else
				printf(" ");

			for (int j = 1; j < 5; j++)
				printf(" ");
		}


		tree_print(tree->left, depth, path, 0);
	}
	string DecodeCh(char ch)
	{

		
		if (!this->pathTab.size())
		{
			string* path = new string;
			PathSearch(this->top, *path);
			sort(pathTable.begin(), pathTable.end(), Cmp);
			delete path;
		}
		string val  = pathTab.at(ch);
		/*auto iter = this->pathTable.begin();
		while (iter != pathTable.end() - 1 && iter->first != ch)
		{
			iter++;
		}*/
	
		return val;
	}
	void PathSearch(Hnode_t* root,string path)
	{
		if (root == NULL)
		{
			return;
		}
		if (root->left == NULL && root->right == NULL)
		{
			/*if (root->item == -128)
			{
				cout << "huy";
			}
			bool check = true;
			bytePathTable::iterator it = pathTab.find(root->item);*/
			/*if (it != pathTab.end())
			{
				check = (it->first != root->item);
			}
			if (check)
			/{/*
			/*	if (*/this->pathTab.insert(bytePathTable::value_type(root->item, path));/*.second == false)*/
			/*	{
					cout << "fall"<<endl;
				}
			}
			else
			{
				cout << "fall" << endl;
			}*/
		}
		if (root->left)
		{
			path += '0';
			PathSearch(root->left, path);
			path.pop_back();
		}
		if (root->right)
		{
			path += '1';
			PathSearch(root->right, path);
			path.pop_back();
		}
	}

	string* EncodePath()
	{

	}

	~Htree()
	{
		delete freqTable;
	}
};
struct unwritedBits
{
    char bits;
	short int position;
};
unwritedBits* writeBits(string* path, ofstream& file, unwritedBits* uB )
{
	char bits;
	uint8_t position, range;
	unwritedBits* _uB = uB;
	if (uB != NULL) 
	{
		for (; uB->position < 8; uB->position++)
		{
			if (path->empty()) 
			{
				return uB;
			}
			if (path->at(0) - '0')
			{
				uB->bits |= 1 << (7 - uB->position);
			}
			path->erase(path->begin());
		}
		file.write(&uB->bits, 1);
		//cout << bitset<8>(uB->bits) << "uB"<<endl;
	}
	position = bits = 0;
	for (uint8_t i = 0, attempts = path->size() / 8 ; i < attempts; i++)
	{
		position = 0;
		while (position < 8)
		{
			if (path->at(position) - '0')
			{
				bits |= 1 << (7 - position);
			}
			position++;
		}
	//	cout << bitset<8>(bits) << "_uB" <<endl;
		file.write(&bits, 1);
	}
	range = path->size() % 8;
	if (range)
	{
		if (_uB == NULL)
		{
			_uB = new unwritedBits;
		}
		_uB->bits = 0;
		for (_uB->position = 0; _uB->position < range; _uB->position++)
		{
			if (path->at(path->size()  - range  + _uB->position) - '0')
			{
				_uB->bits |= 1 << (7 - _uB->position);
			}
		}
		//cout << bitset<8>(_uB->bits) << "_uB"<< endl;
	}
	else
	{
		delete _uB;
		_uB = NULL;
	}
	return _uB;
}

string* proccess(ifstream descriptor, Htree* decoder, size_t allocatedMemory) // Memory need written in byte
{
	string* path;
	char ch;
	path = new string;
	path->resize(allocatedMemory);
	for (size_t i = 0; i < allocatedMemory; i++)
	{
		descriptor.get(ch);
		*path += decoder->DecodeCh(ch);
	}
	return path;
}
void proccessManager(ifstream& descriptor, Htree* decoder, size_t countofBytes)
{
	/// реалізувати 2 випадки в залежності від розміру вхідного файла ( єдинноразове виділення памяті на поток , почергове створення потоків з певним обсягом даних 
}
/// <summary>
/// 
/// 



void main()
{
	
	ifstream descriptor;
	ofstream o;
    char ch ;
	size_t countofBytes;
	descriptor.open("C:\\Users\\omelk\\source\\repos\\myZipper\\myZipper\\my.txt",ios_base::binary);
	o.open("C:\\Users\\omelk\\source\\repos\\myZipper\\myZipper\\out.bin", ios_base::binary);
	bool isOp = descriptor.is_open();
	vector<_byteFreq>* result = _z_byte_counting(descriptor,countofBytes);
	
	Htree Tree;
	Tree.Create(result);
	Tree.Build();
	//Tree.prePrint();
	descriptor.close();
	descriptor.open("C:\\Users\\omelk\\source\\repos\\myZipper\\myZipper\\my.txt", ios_base::binary);
	unwritedBits* buff = NULL;
	int it = 0;
	while (descriptor.get(ch))
	{
		string val = Tree.DecodeCh(ch);
		buff = writeBits(&val,o,buff);
	// 	cout << "****" << endl;
		it++;
	}
	
	descriptor.close();
}
