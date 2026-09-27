#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:17 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Classes
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Class ClusterReplication.ClusterReplicationSubsystem
// 0x0590 (0x05B8 - 0x0028)
class UClusterReplicationSubsystem : public UObject
{
public:
	unsigned char                                      UnknownData00[0x590];                                     // 0x0028(0x0590) MISSED OFFSET

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class ClusterReplication.ClusterReplicationSubsystem");
		return pStaticClass;
	}


	void SetAutoClearCache(bool Val);
	void SetAutoCache(bool Val);
	void RemoveAllCachedObjectData();
};


// Class ClusterReplication.ClusterEntityInterface
// 0x0000 (0x0028 - 0x0028)
class UClusterEntityInterface : public UInterface
{
public:

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class ClusterReplication.ClusterEntityInterface");
		return pStaticClass;
	}

};


}

