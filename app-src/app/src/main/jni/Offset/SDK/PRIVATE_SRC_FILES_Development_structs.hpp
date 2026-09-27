#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:14 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Enums
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Enum Development.EImGuiWindowDisplayMode
enum class EImGuiWindowDisplayMode : uint8_t
{
	EImGuiWindowDisplayMode__Embedded = 0,
	EImGuiWindowDisplayMode__Standalone = 1,
	EImGuiWindowDisplayMode__EImGuiWindowDisplayMode_MAX = 2
};



//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Script Structs
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// ScriptStruct Development.PropertyItemData
// 0x0028
struct FPropertyItemData
{
	struct FString                                     PropertyName;                                             // 0x0000(0x0010) (ZeroConstructor)
	class UEditableTextBox*                            EditableTextBox;                                          // 0x0010(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	class UWidget*                                     ContainerWidget;                                          // 0x0018(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	class UButton*                                     NameButton;                                               // 0x0020(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
};

}

