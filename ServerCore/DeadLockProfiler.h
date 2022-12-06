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
	// 1. 기본적으로 이름을 지어서 구별해주자. 
	void PushLock(const char * name); // 어떤 락을 사용하는지
	
	void PopLock(const char* name);

	void CheckCycle();
private: 
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32> _nameToId;
	unordered_map<int32, const char*> _idToName;
	// 락 실행을 스택으로 추정
	stack<int32> _lockStack;
	// 어떤 락이 몇번 락을 잡았는지 히스토리를 관리.
	// ex) 0번 락을 잡고 1번 락을 잡으면 히스토리 추가. 
	map<int32, set<int32>> _lockHistory;

	Mutex _lock;

// 사이클 체크를 위한 임시적인 값
private:
	vector<int32> _discoveredOrder; // 노드 발견 순서 기록
	int32 _discoveredCount = 0; // 노드가 발견된 순서
	vector<bool> _finished; // Dfs가 종료 되었는지 여부
	vector<int32> _parent;
};

