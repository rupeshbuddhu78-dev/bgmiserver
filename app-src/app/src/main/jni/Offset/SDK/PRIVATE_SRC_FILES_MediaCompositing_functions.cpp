// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:28 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function MediaCompositing.MediaPlaneComponent.SetMediaPlane
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// struct FMediaPlaneParameters   Plane                          (Parm)

void UMediaPlaneComponent::SetMediaPlane(const struct FMediaPlaneParameters& Plane)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function MediaCompositing.MediaPlaneComponent.SetMediaPlane");

	UMediaPlaneComponent_SetMediaPlane_Params params;
	params.Plane = Plane;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function MediaCompositing.MediaPlaneComponent.OnRenderTextureChanged
// (Final, Native, Public)

void UMediaPlaneComponent::OnRenderTextureChanged()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function MediaCompositing.MediaPlaneComponent.OnRenderTextureChanged");

	UMediaPlaneComponent_OnRenderTextureChanged_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function MediaCompositing.MediaPlaneComponent.GetPlane
// (Final, Native, Public, BlueprintCallable, BlueprintPure, Const)
// Parameters:
// struct FMediaPlaneParameters   ReturnValue                    (Parm, OutParm, ReturnParm)

struct FMediaPlaneParameters UMediaPlaneComponent::GetPlane()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function MediaCompositing.MediaPlaneComponent.GetPlane");

	UMediaPlaneComponent_GetPlane_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;

	return params.ReturnValue;
}


}

