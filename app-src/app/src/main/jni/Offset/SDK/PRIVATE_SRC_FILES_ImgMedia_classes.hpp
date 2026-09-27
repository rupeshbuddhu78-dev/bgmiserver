#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:27 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Classes
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Class ImgMedia.ImgMediaSource
// 0x0028 (0x0060 - 0x0038)
class UImgMediaSource : public UBaseMediaSource
{
public:
	float                                              FramesPerSecondOverride;                                  // 0x0038(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x003C(0x0004) MISSED OFFSET
	struct FString                                     ProxyOverride;                                            // 0x0040(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	struct FDirectoryPath                              SequencePath;                                             // 0x0050(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly)

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class ImgMedia.ImgMediaSource");
		return pStaticClass;
	}


	void SetSequencePath(const struct FString& Path);
	struct FString GetSequencePath();
	void GetProxies(TArray<struct FString>* OutProxies);
};


}

