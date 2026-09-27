#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:10 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Enums
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Enum MaterialShaderQualitySettings.EMobileCSMQuality
enum class EMobileCSMQuality : uint8_t
{
	EMobileCSMQuality__NoFiltering = 0,
	EMobileCSMQuality__PCF_1x1     = 1,
	EMobileCSMQuality__PCF_2x2     = 2,
	EMobileCSMQuality__EMobileCSMQuality_MAX = 3
};



//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Script Structs
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// ScriptStruct MaterialShaderQualitySettings.MaterialQualityOverrides
// 0x0008
struct FMaterialQualityOverrides
{
	bool                                               bEnableOverride;                                          // 0x0000(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	bool                                               bForceFullyRough;                                         // 0x0001(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	bool                                               bForceNonMetal;                                           // 0x0002(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	bool                                               bForceDisableLMDirectionality;                            // 0x0003(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	bool                                               bForceLQReflections;                                      // 0x0004(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	EMobileCSMQuality                                  MobileCSMQuality;                                         // 0x0005(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	EMobileCSMQuality                                  MobilePointLightShadowQuality;                            // 0x0006(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
	EMobileCSMQuality                                  MobilePhotonShadowQuality;                                // 0x0007(0x0001) (Edit, ZeroConstructor, Config, IsPlainOldData)
};

}

