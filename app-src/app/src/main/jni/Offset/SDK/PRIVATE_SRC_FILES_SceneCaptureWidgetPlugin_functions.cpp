// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:15 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Functions
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function SceneCaptureWidgetPlugin.SceneCaptureWidget.SetSceneCaptureCameraActor
// (Final, Native, Public, BlueprintCallable)
// Parameters:
// class ASceneCaptureCameraActor* InSceneCaptureCameraActor      (Parm, ZeroConstructor, IsPlainOldData)

void USceneCaptureWidget::SetSceneCaptureCameraActor(class ASceneCaptureCameraActor* InSceneCaptureCameraActor)
{
	static UFunction *pFunc = 0;
	if (!pFunc)
		pFunc  = UObject::FindObject<UFunction>("Function SceneCaptureWidgetPlugin.SceneCaptureWidget.SetSceneCaptureCameraActor");

	USceneCaptureWidget_SetSceneCaptureCameraActor_Params params;
	params.InSceneCaptureCameraActor = InSceneCaptureCameraActor;

	auto flags = pFunc->FunctionFlags;
	pFunc->FunctionFlags |= 0x400;

	UObject *currentObj = (UObject *) this;
	currentObj->ProcessEvent(pFunc, &params);

	pFunc->FunctionFlags = flags;
}


}

