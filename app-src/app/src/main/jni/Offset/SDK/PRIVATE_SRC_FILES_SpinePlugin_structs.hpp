#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:25 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Script Structs
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// ScriptStruct SpinePlugin.SpineAnimationStateMixData
// 0x0028
struct FSpineAnimationStateMixData
{
	struct FString                                     From;                                                     // 0x0000(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	struct FString                                     To;                                                       // 0x0010(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	float                                              MIX;                                                      // 0x0020(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x0024(0x0004) MISSED OFFSET
};

// ScriptStruct SpinePlugin.SpineEvent
// 0x0030
struct FSpineEvent
{
	struct FString                                     Name;                                                     // 0x0000(0x0010) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor)
	struct FString                                     StringValue;                                              // 0x0010(0x0010) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor)
	int                                                IntValue;                                                 // 0x0020(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
	float                                              FloatValue;                                               // 0x0024(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
	float                                              Time;                                                     // 0x0028(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x002C(0x0004) MISSED OFFSET
};

}

