// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:17 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function ClusterReplication.ClusterReplicationSubsystem.SetAutoClearCache
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// bool                           Val                            (Parm, ZeroConstructor, IsPlainOldData)

void UClusterReplicationSubsystem::SetAutoClearCache(bool Val)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function ClusterReplication.ClusterReplicationSubsystem.SetAutoClearCache");

	UClusterReplicationSubsystem_SetAutoClearCache_Params params;
	params.Val = Val;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function ClusterReplication.ClusterReplicationSubsystem.SetAutoCache
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// bool                           Val                            (Parm, ZeroConstructor, IsPlainOldData)

void UClusterReplicationSubsystem::SetAutoCache(bool Val)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function ClusterReplication.ClusterReplicationSubsystem.SetAutoCache");

	UClusterReplicationSubsystem_SetAutoCache_Params params;
	params.Val = Val;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function ClusterReplication.ClusterReplicationSubsystem.RemoveAllCachedObjectData
// (Final, Native, Public, BlueprintCallable)

void UClusterReplicationSubsystem::RemoveAllCachedObjectData()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function ClusterReplication.ClusterReplicationSubsystem.RemoveAllCachedObjectData");

	UClusterReplicationSubsystem_RemoveAllCachedObjectData_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

