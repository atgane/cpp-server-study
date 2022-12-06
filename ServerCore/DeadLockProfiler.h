#pragma once

#include <stack>
#include <map>
#include <vector>

/*-------------------------
		DeadLockProfiler
-------------------------*/

class DeadLockProfiler
{
public:
	// 1. �⺻������ �̸��� ��� ����������. 
	void PushLock(const char * name); // � ���� ����ϴ���
	
	void PopLock(const char* name);

	void CheckCycle();
private: 
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32> _nameToId;
	unordered_map<int32, const char*> _idToName;
	// �� ������ �������� ����
	stack<int32> _lockStack;
	// � ���� ��� ���� ��Ҵ��� �����丮�� ����.
	// ex) 0�� ���� ��� 1�� ���� ������ �����丮 �߰�. 
	map<int32, set<int32>> _lockHistory;

	Mutex _lock;

// ����Ŭ üũ�� ���� �ӽ����� ��
private:
	vector<int32> _discoveredOrder; // ��� �߰� ���� ���
	int32 _discoveredCount = 0; // ��尡 �߰ߵ� ����
	vector<bool> _finished; // Dfs�� ���� �Ǿ����� ����
	vector<int32> _parent;
};

