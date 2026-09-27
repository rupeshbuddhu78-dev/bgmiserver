#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:26 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Classes
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Class OodleNetworkHandlerComponent.OodleNetworkTrainerCommandlet
// 0x0018 (0x0098 - 0x0080)
class UOodleNetworkTrainerCommandlet : public UCommandlet
{
public:
	int                                                DictionarySize;                                           // 0x0080(0x0004) (ZeroConstructor, Config, IsPlainOldData)
	int                                                DictionaryTrials;                                         // 0x0084(0x0004) (ZeroConstructor, Config, IsPlainOldData)
	int                                                TrialRandomness;                                          // 0x0088(0x0004) (ZeroConstructor, Config, IsPlainOldData)
	int                                                TrialGenerations;                                         // 0x008C(0x0004) (ZeroConstructor, Config, IsPlainOldData)
	bool                                               bNoTrials;                                                // 0x0090(0x0001) (ZeroConstructor, Config, IsPlainOldData)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0091(0x0007) MISSED OFFSET

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class OodleNetworkHandlerComponent.OodleNetworkTrainerCommandlet");
		return pStaticClass;
	}

};


}

