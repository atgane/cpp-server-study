#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

// 8. ���߿� ���� �޴������� ������ �����ϴ� ��� CoreGlobal����
// ������ �� �ֵ��� �Ѵ�.
// �̷��� �۾��ϸ� �Ŀ� main���� CoreGlobal Core�� �������� �����ϸ�
// �˾Ƽ� �Ŵ����� ȣ���Ѵ�.
class CoreGlobal
{
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;