#include "pch.h"
#include "DeadLockProfiler.h"

/*-------------------------
		DeadLockProfiler
-------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 2. 아이디를 찾거나 발급
	int32 lockId = 0;

	// 락 조회
	auto findIt = _nameToId.find(name);
	
	// 등록이 안됐다면
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); // ID 새로 발급. 사이즈를 넣어준다.
		_nameToId[name] = lockId; // 그리고 대응시킨다.
		_idToName[lockId] = name;
	}
	else
	{
		// 이미 존재하는 경우, 이터레이터의 second를 이용하여 id를 추출한다.
		lockId = findIt->second;
	}

	// 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스라면 데드락 여부 체크
		const int32 prevId = _lockStack.top();
		
		// 기존 마지막 락번호가 지금 잡으려는 ID와 다르다면
		if (lockId != prevId)
		{
			// 히스토리를 참조한다.
			set<int32>& history = _lockHistory[prevId];

			// prevId에서 이전에 lockId에 대해서 잡은 history가 없는 경우
			// (처음 발견된 간선인 경우)
			// 추가해주고 사이클을 체크해준다. 
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);
	
	// 3. 빈곳에서 빼면 오류
	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId)
		CRASH("INVALID_ID_UNLOCK");

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());

	// 4. 사이클 체크 시작. 변수 초기화

	// 락 카운트 만큼의 배열을 만들고 초기값을 -1로 둔다. -1이라는 것은 정점이 방문되지 않은 상태이다. 
	_discoveredOrder = vector<int32>(lockCount, -1);
	// 몇번째로 발견됐는지?
	_discoveredCount = 0;
	// 해당 DFS가 완벽히 끝났는지
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	// 모든 정점을 돌며 DFS. 한번만 돌고 끝날 수 있기도 함.
	for (int32 lockId = 0; lockId < lockCount; lockId++) {
		Dfs(lockId);
	}

	// 연산 끝내고 정리.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	// 5. 방문된 점이라면
	if (_discoveredOrder[here] != -1)
		return;

	// 방문 번호를 부여
	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접 정점 순회

	// here에 대한 히스토리 조회
	auto findIt = _lockHistory.find(here);

	// 히스토리가 없다면 finished 체크 후 종료
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	// here와 연결된 정점을 돌면서
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here가 there보다 먼저 발견됐다면 there는 here의 후손 (순방향)
		if (_discoveredOrder[here] < _discoveredOrder[there]) continue;

		// 그런데 아직 종료되지 않은 경우, 역방향이다.
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there) 
					break;
			}
			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;
}
