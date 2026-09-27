// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:28 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function NiagaraUIRenderer.NiagaraSystemWidget.SetIsActivated
// (Native, Public, BlueprintCallable)
// Parameters:
// bool                           bInIsActive                    (Parm, ZeroConstructor, IsPlainOldData)

void UNiagaraSystemWidget::SetIsActivated(bool bInIsActive)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function NiagaraUIRenderer.NiagaraSystemWidget.SetIsActivated");

	UNiagaraSystemWidget_SetIsActivated_Params params;
	params.bInIsActive = bInIsActive;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function NiagaraUIRenderer.NiagaraSystemWidget.GetNiagaraComponent
// (Final, Native, Public, BlueprintCallable, BlueprintPure)
// Parameters:
// class UNiagaraUIComponent*     ReturnValue                    (ExportObject, Parm, OutParm, ZeroConstructor, ReturnParm, InstancedReference, IsPlainOldData)

class UNiagaraUIComponent* UNiagaraSystemWidget::GetNiagaraComponent()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function NiagaraUIRenderer.NiagaraSystemWidget.GetNiagaraComponent");

	UNiagaraSystemWidget_GetNiagaraComponent_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;

	return params.ReturnValue;
}


// Function NiagaraUIRenderer.NiagaraSystemWidget.GetIsActivated
// (Final, Native, Public, BlueprintCallable, BlueprintPure, Const)
// Parameters:
// bool                           ReturnValue                    (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)

bool UNiagaraSystemWidget::GetIsActivated()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function NiagaraUIRenderer.NiagaraSystemWidget.GetIsActivated");

	UNiagaraSystemWidget_GetIsActivated_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;

	return params.ReturnValue;
}


// Function NiagaraUIRenderer.NiagaraSystemWidget.DeactivateSystem
// (Final, Native, Public, BlueprintCallable)

void UNiagaraSystemWidget::DeactivateSystem()
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function NiagaraUIRenderer.NiagaraSystemWidget.DeactivateSystem");

	UNiagaraSystemWidget_DeactivateSystem_Params params;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


// Function NiagaraUIRenderer.NiagaraSystemWidget.ActivateSystem
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// bool                           Reset                          (Parm, ZeroConstructor, IsPlainOldData)

void UNiagaraSystemWidget::ActivateSystem(bool Reset)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function NiagaraUIRenderer.NiagaraSystemWidget.ActivateSystem");

	UNiagaraSystemWidget_ActivateSystem_Params params;
	params.Reset = Reset;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

