#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

// 8. 나중에 여러 메니저끼리 순서를 지정하는 경우 CoreGlobal에서
// 관리할 수 있도록 한다.
// 이렇게 작업하면 후에 main에서 CoreGlobal Core를 전역으로 실행하면
// 알아서 매니저를 호출한다.
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