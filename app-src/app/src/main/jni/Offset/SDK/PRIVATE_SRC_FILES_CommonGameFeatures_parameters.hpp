#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:22 2026
 
#include "../SDK.hpp"

namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Parameters
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Function CommonGameFeatures.ActorRepControlComponent.ToggleGroupedRepControlByUID
struct UActorRepControlComponent_ToggleGroupedRepControlByUID_Params
{
	int64_t                                            UID;                                                      // (Parm, ZeroConstructor, IsPlainOldData)
	int                                                ControlMark;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	int                                                RepControlGroup;                                          // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function CommonGameFeatures.ActorRepControlComponent.ShouldBlockRepByUID
struct UActorRepControlComponent_ShouldBlockRepByUID_Params
{
	int64_t                                            UID;                                                      // (Parm, ZeroConstructor, IsPlainOldData)
	int                                                RepControlGroup;                                          // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

// Function CommonGameFeatures.ActorRepControlComponent.GetControlGroupData
struct UActorRepControlComponent_GetControlGroupData_Params
{
	int                                                RepControlGroup;                                          // (Parm, ZeroConstructor, IsPlainOldData)
	struct FRepControlGroupData                        OutData;                                                  // (Parm, OutParm)
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

// Function CommonGameFeatures.RepControlActorBase.IsNetRelevantForCustomCheck
struct ARepControlActorBase_IsNetRelevantForCustomCheck_Params
{
	class AActor*                                      RealViewer;                                               // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
	class AActor*                                      ViewTarget;                                               // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
	struct FVector                                     SrcLocation;                                              // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

}

