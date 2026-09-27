// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:08 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function TableResInclude.EvoBaseMapUIMarkTableMap.TraversTable
// (Final, Native, Public)
// Parameters:
// class UUAEDataTable*           TableData                      (Parm, ZeroConstructor, IsPlainOldData)
// struct FName                   Key                            (Parm, ZeroConstructor, IsPlainOldData)

void UEvoBaseMapUIMarkTableMap::TraversTable(class UUAEDataTable* TableData, const struct FName& Key)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function TableResInclude.EvoBaseMapUIMarkTableMap.TraversTable");

	UEvoBaseMapUIMarkTableMap_TraversTable_Params params;
	params.TableData = TableData;
	params.Key = Key;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function TableResInclude.EvoBaseModTableTestTableMap.TraversTable
// (Final, Native, Public)
// Parameters:
// class UUAEDataTable*           TableData                      (Parm, ZeroConstructor, IsPlainOldData)
// struct FName                   Key                            (Parm, ZeroConstructor, IsPlainOldData)

void UEvoBaseModTableTestTableMap::TraversTable(class UUAEDataTable* TableData, const struct FName& Key)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function TableResInclude.EvoBaseModTableTestTableMap.TraversTable");

	UEvoBaseModTableTestTableMap_TraversTable_Params params;
	params.TableData = TableData;
	params.Key = Key;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

