#include "pch.h"
#include "DeadLockProfiler.h"

/*-------------------------
		DeadLockProfiler
-------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 2. ���̵� ã�ų� �߱�
	int32 lockId = 0;

	// �� ��ȸ
	auto findIt = _nameToId.find(name);
	
	// ����� �ȵƴٸ�
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); // ID ���� �߱�. ����� �־��ش�.
		_nameToId[name] = lockId; // �׸��� ������Ų��.
		_idToName[lockId] = name;
	}
	else
	{
		// �̹� �����ϴ� ���, ���ͷ������� second�� �̿��Ͽ� id�� �����Ѵ�.
		lockId = findIt->second;
	}

	// ��� �ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽���� ����� ���� üũ
		const int32 prevId = _lockStack.top();
		
		// ���� ������ ����ȣ�� ���� �������� ID�� �ٸ��ٸ�
		if (lockId != prevId)
		{
			// �����丮�� �����Ѵ�.
			set<int32>& history = _lockHistory[prevId];

			// prevId���� ������ lockId�� ���ؼ� ���� history�� ���� ���
			// (ó�� �߰ߵ� ������ ���)
			// �߰����ְ� ����Ŭ�� üũ���ش�. 
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
	
	// 3. ������� ���� ����
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

	// 4. ����Ŭ üũ ����. ���� �ʱ�ȭ

	// �� ī��Ʈ ��ŭ�� �迭�� ����� �ʱⰪ�� -1�� �д�. -1�̶�� ���� ������ �湮���� ���� �����̴�. 
	_discoveredOrder = vector<int32>(lockCount, -1);
	// ���°�� �߰ߵƴ���?
	_discoveredCount = 0;
	// �ش� DFS�� �Ϻ��� ��������
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	// ��� ������ ���� DFS. �ѹ��� ���� ���� �� �ֱ⵵ ��.
	for (int32 lockId = 0; lockId < lockCount; lockId++) {
		Dfs(lockId);
	}

	// ���� ������ ����.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	// 5. �湮�� ���̶��
	if (_discoveredOrder[here] != -1)
		return;

	// �湮 ��ȣ�� �ο�
	_discoveredOrder[here] = _discoveredCount++;

	// ��� ���� ���� ��ȸ

	// here�� ���� �����丮 ��ȸ
	auto findIt = _lockHistory.find(here);

	// �����丮�� ���ٸ� finished üũ �� ����
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	// here�� ����� ������ ���鼭
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ���ٸ� �湮
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here�� there���� ���� �߰ߵƴٸ� there�� here�� �ļ� (������)
		if (_discoveredOrder[here] < _discoveredOrder[there]) continue;

		// �׷��� ���� ������� ���� ���, �������̴�.
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
