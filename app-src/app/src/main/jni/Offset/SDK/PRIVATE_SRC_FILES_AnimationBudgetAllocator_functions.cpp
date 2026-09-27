// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:07 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function AnimationBudgetAllocator.SkeletalMeshComponentBudgeted.SetAutoRegisterWithBudgetAllocator
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// bool                           bInAutoRegisterWithBudgetAllocator (Parm, ZeroConstructor, IsPlainOldData)

void USkeletalMeshComponentBudgeted::SetAutoRegisterWithBudgetAllocator(bool bInAutoRegisterWithBudgetAllocator)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function AnimationBudgetAllocator.SkeletalMeshComponentBudgeted.SetAutoRegisterWithBudgetAllocator");

	USkeletalMeshComponentBudgeted_SetAutoRegisterWithBudgetAllocator_Params params;
	params.bInAutoRegisterWithBudgetAllocator = bInAutoRegisterWithBudgetAllocator;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function AnimationBudgetAllocator.AnimationBudgetBlueprintLibrary.SetAnimationBudgetParameters
// (Final, Native, Static, Private, HasOutParms, BlueprintCallable)
// Parameters:
// class UObject*                 WorldContextObject             (Parm, ZeroConstructor, IsPlainOldData)
// struct FAnimationBudgetAllocatorParameters InParameters                   (ConstParm, Parm, OutParm, ReferenceParm)

void UAnimationBudgetBlueprintLibrary::SetAnimationBudgetParameters(class UObject* WorldContextObject, const struct FAnimationBudgetAllocatorParameters& InParameters)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function AnimationBudgetAllocator.AnimationBudgetBlueprintLibrary.SetAnimationBudgetParameters");

	UAnimationBudgetBlueprintLibrary_SetAnimationBudgetParameters_Params params;
	params.WorldContextObject = WorldContextObject;
	params.InParameters = InParameters;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	static auto defaultObj = StaticClass()->GetDefaultObject();
	defaultObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function AnimationBudgetAllocator.AnimationBudgetBlueprintLibrary.EnableAnimationBudget
// (Final, Native, Static, Private, BlueprintCallable)
// Parameters:
// class UObject*                 WorldContextObject             (Parm, ZeroConstructor, IsPlainOldData)
// bool                           bEnabled                       (Parm, ZeroConstructor, IsPlainOldData)

void UAnimationBudgetBlueprintLibrary::EnableAnimationBudget(class UObject* WorldContextObject, bool bEnabled)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function AnimationBudgetAllocator.AnimationBudgetBlueprintLibrary.EnableAnimationBudget");

	UAnimationBudgetBlueprintLibrary_EnableAnimationBudget_Params params;
	params.WorldContextObject = WorldContextObject;
	params.bEnabled = bEnabled;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	static auto defaultObj = StaticClass()->GetDefaultObject();
	defaultObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

