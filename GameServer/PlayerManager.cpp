#include "pch.h"
#include "PlayerManager.h"
#include "AccountManager.h"

PlayerManager GPlayerManager; 

void PlayerManager::PaylerThenAccount() 
{
	WRITE_LOCK;
	GAccountManager.Lock();
}

void PlayerManager::Lock()
{
	WRITE_LOCK;
}