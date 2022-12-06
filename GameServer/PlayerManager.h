#pragma once
class PlayerManager
{
	USE_LOCK;

public:
	void PaylerThenAccount();
	void Lock();
};

extern PlayerManager GPlayerManager;
