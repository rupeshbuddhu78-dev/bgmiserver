// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:28 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function AndroidPermission.AndroidPermissionFunctionLibrary.CheckPermission
// (Final, Native, Static, Public, BlueprintCallable)
// Parameters:
// struct FString                 permission                     (Parm, ZeroConstructor)
// bool                           ReturnValue                    (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)

bool UAndroidPermissionFunctionLibrary::CheckPermission(const struct FString& permission)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function AndroidPermission.AndroidPermissionFunctionLibrary.CheckPermission");

	UAndroidPermissionFunctionLibrary_CheckPermission_Params params;
	params.permission = permission;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	static auto defaultObj = StaticClass()->GetDefaultObject();
	defaultObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;

	return params.ReturnValue;
}


// Function AndroidPermission.AndroidPermissionFunctionLibrary.AcquirePermissions
// (Final, Native, Static, Public, HasOutParms, BlueprintCallable)
// Parameters:
// TArray<struct FString>         Permissions                    (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
// class UAndroidPermissionCallbackProxy* ReturnValue                    (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)

class UAndroidPermissionCallbackProxy* UAndroidPermissionFunctionLibrary::AcquirePermissions(TArray<struct FString> Permissions)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function AndroidPermission.AndroidPermissionFunctionLibrary.AcquirePermissions");

	UAndroidPermissionFunctionLibrary_AcquirePermissions_Params params;
	params.Permissions = Permissions;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	static auto defaultObj = StaticClass()->GetDefaultObject();
	defaultObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;

	return params.ReturnValue;
}


}

