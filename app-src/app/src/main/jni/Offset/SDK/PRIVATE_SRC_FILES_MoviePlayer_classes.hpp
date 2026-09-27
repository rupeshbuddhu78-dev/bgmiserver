#pragma once

// SDK GENERATE BY TELEGRAM ;- (@PRIVATE_SRC_FILES) (4.6.0 -- 64Bit) DM TO BUY TOOL AVLABLE FOR SELL
// Telegram:- @PRIVATE_SRC_FILES
// GEN ONWed Sep 16 07:42:12 2026
 
namespace SDK
{
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------
//Classes
//---------------------By Tg @PRIVATE_SRC_FILES---------------------------

// Class MoviePlayer.MoviePlayerSettings
// 0x0028 (0x0050 - 0x0028)
class UMoviePlayerSettings : public UObject
{
public:
	bool                                               bWaitForMoviesToComplete;                                 // 0x0028(0x0001) (Edit, ZeroConstructor, Config, GlobalConfig, IsPlainOldData)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0029(0x0007) MISSED OFFSET
	TArray<struct FString>                             bMoviesAreSkippable;                                      // 0x0030(0x0010) (Edit, ZeroConstructor, Config, GlobalConfig)
	TArray<struct FString>                             StartupMovies;                                            // 0x0040(0x0010) (Edit, ZeroConstructor, Config, GlobalConfig)

	static UClass* StaticClass()
	{
        static UClass *pStaticClass = 0;
        if (!pStaticClass)
            pStaticClass = UObject::FindClass("Class MoviePlayer.MoviePlayerSettings");
		return pStaticClass;
	}

};


}

