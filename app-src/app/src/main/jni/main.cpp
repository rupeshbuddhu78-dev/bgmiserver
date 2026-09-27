#include "openssl/md5.h"
#include "Offset/SDK.hpp"
using namespace SDK;
using namespace std;
#include <android/native_window_jni.h>
#include <android/log.h>
#include "Main/Tools.h"
#include "Main/Dobby/dobby.h"
#include "Main/Includes.h"
#include "Main/StrEnc.h"
#include "Main/Vector3.hpp"
#include "Main/Vector2.hpp"
#include "Main/MemoryTools.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include "Main/android_native_app_glue.h"
#include "Tools/shadowhook/shadowhook.h"
//#include "Main/obfuscate.h"
#include "Main/Eagle_GUI.h"
#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <thread>
#include <chrono>
#include "Main/json.hpp"
#include "Main/oxorany.cpp"

// Global variables - defined BEFORE includes that use them
bool bValid = false;
bool isLogin = false;
bool Expiry = false;
static std::string EXP = "Key Expiry";
std::string g_Token, g_Auth;
using json = nlohmann::json;
android_app *g_App = 0;
ASTExtraPlayerCharacter *g_LocalPlayer=0;
ASTExtraPlayerController *g_PlayerController =0;

#include "Bypass.h"
#include "LoginKey.h"
#define SLEEP_TIME 1000LL / 120LL
#define TSL_FONT_DEFAULT_SIZE 12
#define PI 3.14159265358979323846
#define RAD2DEG(x) ((float)(x) * (float)(180.f / PI))

uintptr_t GNames_Offset = 0x89750B0;
uintptr_t GUObject_Offset = 0xEB04D00;
uintptr_t GetActorArray = 0xA71EA80;
uintptr_t GNativeAndroidApp_Offset = 0xE8338A8;
uintptr_t Actors_Offset = 0xA0;
#define PostRender 0xA62830C

int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;
uintptr_t UE4;

bool Bullet[20];

size_t UE4_size;

float screenSizeX = 0;
float screenSizeY = 0;
float FOVsize = 200;
float Speed_Aim = 4.0;
float recoilCompensationFactor = 1.1;
float Range = 600;
bool head;
bool AimHead = true;
bool AimBody = false;
int trackingType = 1;
int scopeAndFire = 0;
float FOVSizea;



enum EAimMode {
    AimBullet = 0,
    Pbullet = 1,
    AimBot = 2
};
enum EAimTarget {
    Chest = 0,
    Head = 1
};
enum EAimBy {
    FOV = 0,
    Distance = 1
};
enum EAimTrigger {
    Shooting = 0,
    None = 1,
    Scoping = 2,
    Both = 3,
    Any = 4
};





std::map<int, bool> itemConfig;

struct sConfig {
    bool Bypass;
    bool Enable;
    struct sPlayerESP {
        bool Line;
        bool Health;
        bool Skeleton;
        bool Name;
        bool Distance;
        bool TeamID;
        bool Grenade;
        bool Alert;
        bool Weapon;
        bool ItemEsp;
        bool Vehicle;
        bool OneClickEsp;
        bool MessageBox;
    };
    sPlayerESP PlayerESP{0};

    struct sVehicleESP {
        bool ShowVehicle;
        bool ShowDistance;
    };
    sVehicleESP VehicleESP{0};



    struct sAimMenu {
        bool Enable;
        bool AimBot;
        EAimMode Mode;
        EAimBy AimBy;
        EAimTarget Target;
        EAimTrigger Trigger;
        bool RecoilComparison;
        float Recc;
        bool RecoilSet;
        bool Radius;
        float Line;
        bool Prediction;
        float Cross;
        float Crosss;
        bool IgnoreKnocked;
        bool VisCheck;
        bool IgnoreBot;
        float AimSmooth = 1.0f;
    };
    sAimMenu SilentAim{0};
    sAimMenu AimBot{0};
};
sConfig Config{0};


std::string Filepath = "/sdcard/Android/obb/com.pubg.imobile/key.lic";


static bool isHead, isNeck, isPelvis, isLeftClavicle, isRightClavicle, 
isLeftUpperArm, isLeftLowerArm, isLeftHand, isLeftThigh, 
isLeftCalf, isLeftFoot, isRightUpperArm, isRightLowerArm, 
isRightHand, isRightThigh, isRightCalf, isRightFoot, 
isSpine1, isSpine2, isSpine3;
static int algorithm = 0;

struct sRegion
{
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;

bool isEqual(std::string s1, const char* check) {
    std::string s2(check);
    return (s1 == s2);
}

bool isObjectInvalid(UObject *obj)
{
    if (!Tools::IsPtrValid(obj))
    {
        return true;
    }
    if (!Tools::IsPtrValid(obj->ClassPrivate))
    {
        return true;
    }
    if (obj->InternalIndex <= 0)
    {
        return true;
    }
    if (obj->NamePrivate.ComparisonIndex <= 0)
    {
        return true;
    }
    if ((uintptr_t)(obj) % sizeof(uintptr_t) != 0x0 && (uintptr_t)(obj) % sizeof(uintptr_t) != 0x4)
    {
        return true;
    }
    if (std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj) >= region.start && ((uintptr_t)obj) <= region.end; }) ||
        std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj->ClassPrivate) >= region.start && ((uintptr_t)obj->ClassPrivate) <= region.end; }))
    {
        return true;
    }
    return false;
}

bool UrlLink;
int OpenURL(const char* url)
{
    JavaVM* java_vm = g_App->activity->vm;
    JNIEnv* java_env = NULL;

    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;

    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK)
        return -2;

    jclass native_activity_clazz = java_env->GetObjectClass(g_App->activity->clazz);
    if (native_activity_clazz == NULL)
        return -3;

    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, "AndroidThunkJava_LaunchURL", "(Ljava/lang/String;)V");
    if (method_id == NULL)
        return -4;
        
    jstring retStr = java_env->NewStringUTF(url);
    java_env->CallVoidMethod(g_App->activity->clazz, method_id, retStr);

    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;

    return 0;
}



template<typename T>
void Write(uintptr_t addr, T value) {
WriteAddr((void *) addr, &value, sizeof(T));
}
void MemoryD_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),4);
}
void MemoryQ_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),32);
}

UWorld *GEWorld;
int GWorldNum = 0;
TUObjectArray gobjects;
UWorld *GetFullWorld()
{
    if(GWorldNum == 0) {
        gobjects = UObject::GUObjectArray->ObjObjects;
        for (int i=0; i< gobjects.Num(); i++)
            if (auto obj = gobjects.GetByIndex(i)) {
                if(obj->IsA(UEngine::StaticClass())) {
                    auto GEngine = (UEngine *) obj;
                    if(GEngine) {
                        auto ViewPort = GEngine->GameViewport;
                        if (ViewPort)
                        {
                            GEWorld = ViewPort->World;
                            GWorldNum = i;
                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                            return ViewPort->World;
                        }
                    }
                }
            }
    }else {
        auto GEngine = (UEngine *) (gobjects.GetByIndex(GWorldNum));
        if(GEngine) {
            auto ViewPort = GEngine->GameViewport;
            if(ViewPort) {
                GEWorld = ViewPort->World;
                                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                return ViewPort->World;
            }
        }
    }
    return 0;
}

static UGameViewportClient *GameViewport = 0;
UGameViewportClient *GetGameViewport() {
    while (!GameViewport) {
        GameViewport = UObject::FindObject<UGameViewportClient>("GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
        sleep(1);
    }
    if (GameViewport) {
        return GameViewport;
    }
    return 0;
}



std::vector<AActor *> GetActors() {
    auto World = GetFullWorld();
    if (!World)
    return std::vector<AActor *>();
    auto PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel)
    return std::vector<AActor *>();
    struct GovnoArray {
    uintptr_t base;
    int32_t count;
    int32_t max;
    };
    static thread_local GovnoArray Actors{};
    Actors = *(((GovnoArray*(*)(uintptr_t))(UE4 + GetActorArray))(reinterpret_cast<uintptr_t>(PersistentLevel)));
    if (Actors.count <= 0) {
    return {};
    }
    std::vector<AActor *> actors;
    for (int i = 0; i < Actors.count; i++) {
    auto Actor = *(uintptr_t *) (Actors.base + (i * sizeof(uintptr_t)));
    if (Actor) {
    actors.push_back(reinterpret_cast<AActor *const>(Actor));
    }
    }
  return actors;
}

const char *GetVehicleName(ASTExtraVehicleBase *Vehicle) {
    switch (Vehicle->VehicleShapeType) {
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike_SideCart:
            return "Motorbike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Dacia:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyDacia:
            return "Dacia";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MiniBus:
            return "Mini Bus";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyPickup:
            return "Pick Up";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Buggy:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyBuggy:
            return "Buggy";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ02:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ03:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUAZ:
            return "UAZ";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PG117:
            return "PG117";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Aquarail:
            return "Aquarail";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado01:
            return "Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Rony:
            return "Rony";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Scooter:
            return "Scooter";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowMobile:
            return "Snow Mobile";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_TukTukTuk:
            return "Tuk Tuk";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowBike:
            return "Snow Bike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Surfboard:
            return "Surf Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Snowboard:
            return "Snow Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Amphibious:
            return "Amphibious";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_LadaNiva:
            return "Lada Niva";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAV:
            return "UAV";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MegaDrop:
            return "Mega Drop";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini01:
            return "Lamborghini";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_GoldMirado:
            return "Gold Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_BigFoot:
            return "Big Foot";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUH60:
            return "UH60";
            break;
        default:
            return "Vehicle";
            break;
    }
    return "Vehicle";
}



TNameEntryArray *GetGNames()
{
    return ((TNameEntryArray * (*)()) (UE4 + GNames_Offset))();
}

template <class T>
void GetAllActors(std::vector<T *> &Actors)
{
    UGameplayStatics *gGameplayStatics = (UGameplayStatics *)gGameplayStatics->StaticClass();
    auto GWorld = GetFullWorld();
    if (GWorld)
    {
        TArray<AActor *> Actors2;
        gGameplayStatics->GetAllActorsOfClass((UObject *)GWorld, T::StaticClass(), &Actors2);
        for (int i = 0; i < Actors2.Num(); i++)
        {
            Actors.push_back((T *)Actors2[i]);
        }
    }
}
////==========================================================================================================//
FVector GetBoneLocationByName(ASTExtraPlayerCharacter *Actor, const char *BoneName)
{
    return Actor->GetBonePos(BoneName, FVector());
}

#define COLOR_IN FLinearColor(0, 0, 0, 0)
#define COLOR_BLUE FLinearColor(0, 0, 0, 1.f)
#define COLOR_WHITE FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_RED FLinearColor(1.f, 0, 0, 1.f)
#define COLOR_CAR FLinearColor(1.f, 0.5f, 1.f, 1.f)
#define COLOR_GREEN FLinearColor(0.0f, 1.0f, .0f, 1.0f)
#define COLOR_ORANGE FLinearColor(1.f, 0.4f, 0, 1.f)
#define COLOR_ROSE FLinearColor(0.929f, 0.682f, 0.753f, 1.0f)
#define COLOR_YELLOW FLinearColor(1.f, 1.f, 0, 1.f)
#define COLOR_LIME FLinearColor(0, 1.f, 0, 1.f)
#define COLOR_BLUE FLinearColor(0, 0, 1.f, 1.f)
#define COLOR_THISTLE FLinearColor(1.0f, 0.74f, 0.84f, 1.0f)
#define COLOR_PINK FLinearColor(1.0f, 0.75f, 0.8f, 1.0f)
#define COLOR_CYAN FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_OUTLINE FLinearColor(0.0f, 0.0f, 0.0f, 0.25f)
#define Black FLinearColor(0, 0, 0, 1.f)
#define COLOR_BLUE FLinearColor(0, 0, 0, 0.f)
#define White FLinearColor(1.f, 1.f, 1.f, 1.f)
#define Red   FLinearColor(1.f, 0, 0, 1.f)
#define Gray  FLinearColor(0, 1.f, 0, 1.f)
#define Blue  FLinearColor(0, 0, 1.f, 1.f)
#define Pink   FLinearColor(1.f, 0.5f, 1.f, 1.f)
#define Red FLinearColor(1.f, 0, 0, 0.7f)
#define Green FLinearColor(0, 0.5f, 0, 1.f)
#define Orange FLinearColor(1.f, 0.4f, 0, 1.f)
#define Purple FLinearColor(0.67f, 0.57f, 0.81f, 1.0f)
#define Yellow FLinearColor(1.f, 1.f, 0, 1.f)
#define BB FLinearColor(36 / 255.f, 249 / 255.f, 217 / 255.f, 0.0f);



SDK::FVector SubtractVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X - b.X;
    result.Y = a.Y - b.Y;
    result.Z = a.Z - b.Z;
    return result;
}



SDK::FVector AddVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
    return result;
}

SDK::FVector MultiplyVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X * b.X;
    result.Y = a.Y * b.Y;
    result.Z = a.Z * b.Z;
    return result;
}

SDK::FVector DivideVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X / b.X;
    result.Y = a.Y / b.Y;
    result.Z = a.Z / b.Z;
    return result;
}

SDK::FVector MultiplyVectorFloat(SDK::FVector a, float scalar) {
    SDK::FVector result;
    result.X = a.X * scalar;
    result.Y = a.Y * scalar;
    result.Z = a.Z * scalar;
    return result;
}

FRotator ToRotator2(FVector local, FVector target) {
    FVector Lund = SubtractVectors(target, local);
    FRotator newViewAngle;
    newViewAngle.Pitch =
            -std::atan2(Lund.Z, std::sqrt(Lund.X * Lund.X + Lund.Y * Lund.Y)) * (180.f / M_PI);
    newViewAngle.Yaw = std::atan2(Lund.Y, Lund.X) * (180.f / M_PI);
    newViewAngle.Roll = 0.f;
    if (newViewAngle.Yaw < 0.f) {
        newViewAngle.Yaw += 360.f;
    }
    return newViewAngle;
}

void VectorAnglesRadar(Vector3 &forward, FVector &angles) {
    if (forward.X == 0.f && forward.Y == 0.f) {
        angles.X = forward.Z > 0.f ? -360.f : 360.f;
        angles.Y = 0.f;
    } else {
        angles.X = RAD2DEG(atan2(-forward.Z, forward.Magnitude(forward)));
        angles.Y = RAD2DEG(atan2(forward.Y, forward.X));
    }
    angles.Z = 360.f;
}



FRotator ToRotator(FVector local, FVector target) {
FVector rotation = UKismetMathLibrary::Subtract_VectorVector(local, target);

float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);

FRotator newViewAngle = {0};
newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Roll = (float) 0.f;

if (rotation.X >= 0.f)
newViewAngle.Yaw += 180.0f;

return newViewAngle;
}
void AimAngle(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}

bool W2S2(FVector worldPos, FVector2D *screenPos) {
return g_PlayerController->ProjectWorldLocationToScreen(worldPos, true, screenPos);
}








////==========================================================================================================//
void *LoadFont(void *)
{
    while (!tslFontUI || !robotoFont)
    {
        tslFontUI = UObject::FindObject<UFont>("Font Roboto.Roboto");
        robotoFont = UObject::FindObject<UFont>("Font RobotoDistanceField.RobotoDistanceField");
        sleep(1);
    }
    return 0;
}



void NekoHook(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}
void NekoHook(float *angles) {
    if (angles[0] > 180)
        angles[0] -= 360;
    if (angles[0] < -180)
        angles[0] += 360;

    if (angles[0] < -75.f)
        angles[0] = -75.f;
    else if (angles[0] > 75.f)
        angles[0] = 75.f;

    while (angles[1] < -180.0f)
        angles[1] += 360.0f;
    while (angles[1] > 180.0f)
        angles[1] -= 360.0f;
}

void NekoHook(FVector2D angles) {
    if (angles.X > 180)
        angles.X -= 360;
    if (angles.X < -180)
        angles.X += 360;

    if (angles.X < -75.f)
        angles.X = -75.f;
    else if (angles.X > 75.f)
        angles.X = 75.f;

    while (angles.Y < -180.0f)
        angles.Y += 360.0f;
    while (angles.Y > 180.0f)
        angles.Y -= 360.0f;
}
////==========================================================================================================//
#define W2S(w, s) UGameplayStatics::ProjectWorldToScreen(g_PlayerController, w, true, s)


bool isInsideFOV(int x, int y) {
    int circle_x = screenWidth / 2;
    int circle_y = screenHeight / 2;
    int rad = (int) FOVsize;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

auto GetTargetForAim()
{
    ASTExtraPlayerCharacter *result = 0;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    
    auto localPlayer = g_LocalPlayer;
    auto localPlayerController = g_PlayerController;
    
    if (localPlayer)
    {
        for (auto Actor : Actors)
        {
            if (isObjectInvalid(Actor))
                continue;
            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                auto Player = (ASTExtraPlayerCharacter *)Actor;
                
                float dist = localPlayer->GetDistanceTo(Player) / 100.0f;    
                   if (dist > Range)
                    continue;
                    
                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;

                if (Player->TeamID == localPlayer->TeamID)
                    continue;

                if (Player->bDead)
                    continue;

                if (Config.AimBot.IgnoreKnocked) {
                if (Player->Health == 0.0f)
                continue;
                }
                                                  
                if (Config.AimBot.IgnoreBot) {
                if (Player->bEnsure)
                continue;
                }
                        
                if (Config.AimBot.VisCheck) {
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("Head", {0, 0, 0}), false))//头
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("neck_01", {0, 0, 0}), false))//Neck
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_r", {0, 0, 0}), false))//上面的肩膀右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_l", {0, 0, 0}), false))//上面的肩膀左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_r", {0, 0, 0}), false))//上面的手臂右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_l", {0, 0, 0}), false))//上面的手臂左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_03", {0, 0, 0}), false))//脊柱3
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_02", {0, 0, 0}), false))//脊柱2
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_01", {0, 0, 0}), false))//脊柱2
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("pelvis", {0, 0, 0}), false))//骨盆
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_l", {0, 0, 0}), false))//大腿左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_r", {0, 0, 0}), false))//大腿右
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_l", {0, 0, 0}), false))//小腿左
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_r", {0, 0, 0}), false))//小腿右
                continue;

                static bool isSelected = false;
                algorithm = 0;
                isSelected = false;
                 

                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {0, 0, 0}),  false)) {//头
                 isHead = false;
                }else{
                 isHead = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("pelvis", {0, 0, 0}),  false))
                {//骨盆
                 isPelvis = false;
                }else{
                 isPelvis = true;
                }
                                               
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("neck_01", {0, 0, 0}),  false))
                {//Neck
                 isNeck = false;
                }else{
                 isNeck = true;
                }
                                                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_l", {0, 0, 0}),  false))
                {//左手
                 isLeftHand = false;
                }else{
                 isLeftHand = true;
                }
                                                
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_r", {0, 0, 0}),  false))
                {//右手
                 isRightHand = false;
                }else{
                 isRightHand = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_l", {0, 0, 0}),  false))
                {//左脚
                 isLeftFoot = false;
                }else{
                 isLeftFoot = true;
                }
                     
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_r", {0, 0, 0}),  false))
                {//右脚
                 isRightFoot = false;
                }else{
                 isRightFoot = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_l", {0, 0, 0}),  false))
                {//左小腿
                 isLeftCalf = false;
                }else{
                 isLeftCalf = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_r", {0, 0, 0}),  false))
                {//右小腿
                 isRightCalf = false;
                }else{
                 isRightCalf = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_l", {0, 0, 0}),  false))
                {//左小臂
                 isLeftLowerArm = false;
                }else{
                 isLeftLowerArm = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_r", {0, 0, 0}),  false))
                {//右小臂
                 isRightLowerArm = false;
                }else{
                 isRightLowerArm = true;
                }
                        
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_l", {0, 0, 0}),  false))
                {//左上臂
                 isLeftThigh = false;
                }else{
                 isLeftThigh = true;
                }
                if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_r", {0, 0, 0}),  false))
                {//左上臂
                 isRightThigh = false;
                }else{
                 isRightThigh = true;
                }
                                                                                                
                if (!isSelected)
                if(isHead)
                {
                  algorithm = 1;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isPelvis)
                {
                  algorithm = 2;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftCalf)
                {
                  algorithm = 3;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightCalf)
                {
                  algorithm = 4;   
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftLowerArm)
                {
                  algorithm = 5;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightLowerArm)
                {
                  algorithm = 6;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftUpperArm)
                {
                  algorithm = 7;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightUpperArm)
                {
                  algorithm = 8;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftThigh)
                {
                  algorithm = 9;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightThigh)
                {
                  algorithm = 10;
                  isSelected = true;
                 }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isLeftFoot)
                {
                  algorithm = 11;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                if (!isSelected)
                if(isRightFoot)
                {
                  algorithm = 12;
                  isSelected = true;
                }else{
                  isSelected = false;
                }
                }

                if (trackingType == 0) {
                   float dist = localPlayer->GetDistanceTo(Player);
                   if (dist < max) {
                      max = dist;
                      result = Player;
                    }
                }
                
                if (trackingType == 1) {
                   auto Root = Player->GetBonePos("Root", {});
                   auto Head = Player->GetBonePos("Head", {});

                    FVector2D RootSc, HeadSc;
                     if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
                      {
                         float height = abs(HeadSc.Y - RootSc.Y);
                         float width = height * 0.65f;
                        FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
                        if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
                        {
                        FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
                        FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
                        if(isInsideFOV((int)middlePoint.X, (int)middlePoint.Y)) {
                           float dist = FVector2D::Distance(v2Middle, v2Loc);
                           if (dist < max) {
                                max = dist;
                                result = Player;
                               }                           
                            }
                        }
                    }
                }                
            }
        }
    }    
    return result;
}

bool isInsideFOVs(int x, int y)
{
    if (!Config.AimBot.Radius)
        return true;

    int circle_x = glWidth / 2;
    int circle_y = glHeight / 2;
    int rad = Config.AimBot.Radius;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

auto GetTargetForAimBot()
{
    ASTExtraPlayerCharacter *result = nullptr;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    if (g_LocalPlayer)
    {
        for (int i = 0; i < Actors.size(); i++)
        {
            auto Actor = Actors[i];
            if (isObjectInvalid(Actor))
                continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                auto Player = (ASTExtraPlayerCharacter *)Actor;

                float dist = g_LocalPlayer->GetDistanceTo(Player) / 100.0f;
                if (dist > 150.0f)
                    continue;

                if (Player->PlayerKey == g_PlayerController->PlayerKey)
                    continue;
                if (Player->TeamID == g_PlayerController->TeamID)
                    continue;
                if (Player->bDead)
                    continue;

                if (Config.AimBot.IgnoreKnocked)
                {
                    if (Player->Health == 0.0f)
                        continue;
                }

                if (Config.AimBot.VisCheck)
                {
                    if (!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {}), true))
                        continue;
                }

                if (trackingType == 0) {
                   float dist = g_LocalPlayer->GetDistanceTo(Player);
                   if (dist < max) {
                      max = dist;
                      result = Player;
                    }
                }
                
                if (trackingType == 1) {
                   auto Root = Player->GetBonePos("Root", {});
                   auto Head = Player->GetBonePos("Head", {});

                    FVector2D RootSc, HeadSc;
                     if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
                      {
                         float height = abs(HeadSc.Y - RootSc.Y);
                         float width = height * 0.65f;
                        FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
                        if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
                        {
                        FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
                        FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
                        if(isInsideFOVs((int)middlePoint.X, (int)middlePoint.Y)) {
                           float dist = FVector2D::Distance(v2Middle, v2Loc);
                           if (dist < max) {
                                max = dist;
                                result = Player;
                               }                           
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}



void RenderESP(UCanvas* Canvas, int ScreenWidth, int ScreenHeight)
{

    ASTExtraPlayerCharacter *localPlayer = 0;
    ASTExtraPlayerController *localPlayerController = 0;

    screenWidth = ScreenWidth;
    screenHeight = ScreenHeight;


            auto Actors = GetActors();
            UGameplayStatics *gGameplayStatics = (UGameplayStatics *)UGameplayStatics::StaticClass();
            auto GWorld = GetFullWorld();
            if (GWorld)
            {
                UNetDriver *NetDriver = GWorld->NetDriver;
                if (NetDriver)
                {
                    UNetConnection *ServerConnection = NetDriver->ServerConnection;
                if (ServerConnection)
                {
                    localPlayerController = (ASTExtraPlayerController *)ServerConnection->PlayerController;
                }
            }
            }
            if (localPlayerController) {
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;             
                GetAllActors(PlayerCharacter);
            for (auto actor = PlayerCharacter.begin();
                actor != PlayerCharacter.end(); actor++) {
                 auto Actor = *actor;
            if (Actor->PlayerKey ==((ASTExtraPlayerController *) localPlayerController)->PlayerKey) {
                 localPlayer = Actor;
                 break;
                }
            }
            if (localPlayer) {
            
if (localPlayer->PartHitComponent) {
auto ConfigCollisionDistSqAngles = localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) {
ConfigCollisionDistSqAngles[j].Angle = 90.0f;
}
localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = ConfigCollisionDistSqAngles;
}

           if (Config.AimBot.Enable) {
           DrawCircle(Canvas, (screenWidth / 2), (screenHeight / 2), FOVsize, 100, COLOR_RED);     
           ASTExtraPlayerCharacter *Target = GetTargetForAim();
				if (Target)
				{
   bool triggerOk = false;
if (scopeAndFire == 0) {
   triggerOk = localPlayer->bIsWeaponFiring;
 } else if (scopeAndFire == 1) {
triggerOk = localPlayer->bIsWeaponFiring || localPlayer->bIsGunADS;
  } else triggerOk = true;
		  	if (triggerOk){
   FVector targetAimPos;
 if (AimHead) {
   if(algorithm == 0) {
targetAimPos=Target->GetBonePos("Head", {});
   } else if(algorithm == 1) {
targetAimPos = Target->GetBonePos("Head", {});
   }else if(algorithm == 2){
targetAimPos = Target->GetBonePos("pelvis", {});//锁骨
   }else if(algorithm == 3){
targetAimPos = Target->GetBonePos("calf_l", {});//左小腿
   }else if(algorithm == 4){
targetAimPos = Target->GetBonePos("calf_r", {});//右小腿
   }else if(algorithm == 5){
targetAimPos = Target->GetBonePos("lowerarm_l", {});//左小臂
   }else if(algorithm == 6){
targetAimPos = Target->GetBonePos("lowerarm_r", {});//右小臂
   }else if(algorithm == 7){
targetAimPos = Target->GetBonePos("upperarm_l", {});//左上臂
   }else if(algorithm == 8){
targetAimPos = Target->GetBonePos("upperarm_r", {});//右上臂
   }else if(algorithm == 9) {
targetAimPos = Target->GetBonePos("thigh_l", {});//左大腿
   }else if(algorithm == 10) {
targetAimPos = Target->GetBonePos("thigh_r", {});//右大腿
   }else if(algorithm == 11) {
targetAimPos = Target->GetBonePos("foot_l", {});//左脚
   }else if(algorithm == 12){
targetAimPos = Target->GetBonePos("foot_r", {});//右脚
   }
 }
 if(AimBody){
   if(algorithm == 0) {
targetAimPos = Target->GetBonePos("Head", {});//头
   }else if(algorithm == 1) {
targetAimPos = Target->GetBonePos("spine_03", {});//Neck
   }else if(algorithm == 2){
targetAimPos = Target->GetBonePos("pelvis", {});//屁股
   }else if(algorithm == 3){
targetAimPos = Target->GetBonePos("calf_l", {});//左小腿
   }else if(algorithm == 4){
targetAimPos = Target->GetBonePos("calf_r", {});//右小腿
   }else if(algorithm == 5){
targetAimPos = Target->GetBonePos("lowerarm_l", {});//左小臂
   }else if(algorithm == 6){
targetAimPos = Target->GetBonePos("lowerarm_r", {});//右小臂
   }else if(algorithm == 7){
targetAimPos = Target->GetBonePos("upperarm_l", {});//左上臂
   }else if(algorithm == 8){
targetAimPos = Target->GetBonePos("upperarm_r", {});//右上臂
   }else if(algorithm == 9) {
targetAimPos = Target->GetBonePos("thigh_l", {});//左大腿
   }else if(algorithm == 10) {
targetAimPos = Target->GetBonePos("thigh_r", {});//右大腿
   }else if(algorithm == 11) {
targetAimPos = Target->GetBonePos("foot_l", {});//左脚
   }else if(algorithm == 12){
targetAimPos = Target->GetBonePos("foot_r", {});//右脚
}
 }
			 auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
 if (WeaponManagerComponent)
 {
auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
if ((int)propSlot.GetValue() >= 1 && (int)propSlot.GetValue() <= 3)
{
  auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
  if (CurrentWeaponReplicated)
  {
auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
  if (ShootWeaponComponent)
  {
UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
  if (ShootWeaponEntityComponent)
  {
 ASTExtraVehicleBase *CurrentVehicle = Target->CurrentVehicle;
 float dist = localPlayer->GetDistanceTo(Target);
 auto timeToTravel = dist / ShootWeaponEntityComponent->BulletRange;
  if (CurrentVehicle)
  {
 FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
 targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
   }else{
 FVector Velocity = Target->GetVelocity();
 targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
   }

   if (localPlayer->bIsWeaponFiring)
   {
 float dist = localPlayer->GetDistanceTo(Target) / 100.f;
 targetAimPos.Z -= dist * recoilCompensationFactor;
}

 FVector fDir = UKismetMathLibrary::Subtract_VectorVector(targetAimPos, g_PlayerController->PlayerCameraManager->CameraCache.POV.Location);
 FRotator Yaptr = UKismetMathLibrary::Conv_VectorToRotator(fDir);

 FRotator CpYaT = g_PlayerController->PlayerCameraManager->CameraCache.POV.Rotation;

 Yaptr.Pitch -= CpYaT.Pitch;
 Yaptr.Yaw -= CpYaT.Yaw;
 Yaptr.Roll = 0.f;
 NekoHook(Yaptr);

 CpYaT.Pitch += Yaptr.Pitch / Speed_Aim; // Aim X Speed Make Float : Xs
 CpYaT.Yaw += Yaptr.Yaw / Speed_Aim; // Aim Y Speed Make Float : Ys
 CpYaT.Roll = 0.f;

localPlayer->AddControllerYawInput(Yaptr.Yaw);
localPlayer->AddControllerPitchInput(Yaptr.Pitch);
}}}}}}}}                   

                    
                    
                    
               
                int totalEnemies = 0, totalBots = 0;
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;
                GetAllActors(PlayerCharacter);
                for (auto actor = PlayerCharacter.begin(); actor != PlayerCharacter.end(); actor++)
                {

                auto Player = *actor;
                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;
                if (Player->TeamID == localPlayer->TeamID)
                    continue;
                if (Player->bDead)
                    continue;
                if (Player->bHidden)
                    continue;
                                                                
                if (!Player->RootComponent)
                    continue;
                
                if (Player->bEnsure)
                 totalBots++;
                 else totalEnemies++;
                        
                float Distance = localPlayer->GetDistanceTo(Player) / 100.0f;
                if (Distance > 500)
                continue;            
                                                
                FVector HeadPos = GetBoneLocationByName(Player,"Head");
                FVector2D HeadPosSC;
                FVector RootPos = GetBoneLocationByName(Player,"Root");
                FVector2D RootPosSC;
                FVector Root = GetBoneLocationByName(Player,"Root");
                FVector Spin = GetBoneLocationByName(Player,"pelvis");
                FVector Spin2 = GetBoneLocationByName(Player,"spine_03");
                FVector pelvis = GetBoneLocationByName(Player,"pelvis");
                FVector2D pelvisPoSC;
                FVector upper_r = GetBoneLocationByName(Player,"upperarm_r");
                FVector2D upper_rPoSC;
                FVector lowerarm_r = GetBoneLocationByName(Player,"lowerarm_r");
                FVector2D lowerarm_rPoSC;
                FVector lowerarm_l = GetBoneLocationByName(Player,"lowerarm_l");
                FVector2D lowerarm_lSC;
                FVector hand_r = GetBoneLocationByName(Player,"hand_r");
                FVector2D hand_rPoSC;
                FVector upper_l = GetBoneLocationByName(Player,"upperarm_l");
                FVector2D upper_lPoSC;
                FVector hand_l = GetBoneLocationByName(Player,"hand_l");
                FVector2D hand_lPoSC;
                FVector thigh_l = GetBoneLocationByName(Player,"thigh_l");
                FVector2D thigh_lPoSC;
                FVector calf_l = GetBoneLocationByName(Player,"calf_l");
                FVector2D calf_lPoSC;
                FVector foot_l = GetBoneLocationByName(Player,"foot_l");
                FVector2D foot_lPoSC;
                FVector thigh_r = GetBoneLocationByName(Player,"thigh_r");
                FVector2D thigh_rPoSC;
                FVector calf_r = GetBoneLocationByName(Player,"calf_r");
                FVector2D calf_rPoSC;
                FVector foot_r = GetBoneLocationByName(Player,"foot_r");
                FVector2D foot_rPoSC;
                FVector neck_01 = GetBoneLocationByName(Player,"neck_01");
                FVector2D neck_01PoSC;
                FVector spine_01 = GetBoneLocationByName(Player,"spine_01");
                FVector2D spine_01PoSC;
                FVector spine_02 = GetBoneLocationByName(Player,"spine_02");
                FVector2D spine_02PoSC;
                FVector spine_03 = GetBoneLocationByName(Player,"spine_03");
                FVector2D spine_03PoSC;               
////==========================================================================================================//
                if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, HeadPos, false, &HeadPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_l, false, &lowerarm_lSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_r, false, &upper_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_l, false, &upper_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_r, false, &lowerarm_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_r, false, &hand_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_l, false, &hand_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_l, false, &thigh_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_l, false, &calf_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_l, false, &foot_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_r, false, &thigh_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_r, false, &calf_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_r, false, &foot_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, neck_01, false, &neck_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, pelvis, false, &pelvisPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, RootPos, false, &RootPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_01, false, &spine_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_02, false, &spine_02PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_03, false, &spine_03PoSC)) {
////==========================================================================================================//

                bool IsVisible = g_PlayerController->LineOfSightTo(Player, {0,0,0}, true);
                

                if (Config.PlayerESP.Skeleton) {
    bool IsVisible = g_PlayerController && 
                     g_PlayerController->LineOfSightTo(Player, FVector(0,0,0), false);

    static std::vector<std::string> right_arm{"neck_01", "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r"};
    static std::vector<std::string> left_arm{"neck_01", "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l"};
    static std::vector<std::string> spine{"Head", "neck_01", "spine_01", "pelvis"};
    static std::vector<std::string> lower_right{"pelvis", "thigh_r", "calf_r", "foot_r"};
    static std::vector<std::string> lower_left{"pelvis", "thigh_l", "calf_l", "foot_l"};
    static std::vector<std::vector<std::string>> skeleton{right_arm, left_arm, spine, lower_right, lower_left};

    if (!Player || !Canvas || !g_PlayerController) {
        return;
    }

    static std::unordered_map<std::string, int> boneToIndex;
    static std::vector<std::string> boneNames;
    static bool bInitialized = false;

    if (!bInitialized) {
        int index = 0;
        boneNames.reserve(32);
        for (const auto& boneStructure : skeleton) {
            for (const auto& bone : boneStructure) {
                if (boneToIndex.emplace(bone, index).second) {
                    boneNames.push_back(bone);
                    ++index;
                }
            }
        }
        bInitialized = true;
    }

    std::vector<FVector> boneLocations(boneNames.size());
    for (auto &v : boneLocations) {
        v = FVector(0,0,0);
    }

    for (size_t i = 0; i < boneNames.size(); ++i) {
        boneLocations[i] = GetBoneLocationByName(Player, boneNames[i].c_str());
    }

    std::vector<std::pair<FVector2D, bool>> screenPositions(
        boneNames.size(), {FVector2D(0, 0), false}
    );

    for (size_t i = 0; i < boneNames.size(); ++i) {
        FVector2D screenPos;
        bool bProjected = gGameplayStatics->ProjectWorldToScreen(
            g_PlayerController, boneLocations[i], false, &screenPos
        );

        bool bOnScreen = bProjected &&
            screenPos.X >= -200 && screenPos.X <= Canvas->SizeX + 200 &&
            screenPos.Y >= -200 && screenPos.Y <= Canvas->SizeY + 200;

        screenPositions[i] = {screenPos, bOnScreen};
    }

    FLinearColor lineColor = IsVisible
        ? FLinearColor(0.f, 1.f, 0.f, 1.f)
        : FLinearColor(1.f, 0.f, 0.f, 1.f); 

    for (auto& boneStructure : skeleton) {
        std::string lastBone;

        for (const std::string& currentBone : boneStructure) {
            if (!lastBone.empty()) {

                int lastBoneIndex = boneToIndex[lastBone];
                int currentBoneIndex = boneToIndex[currentBone];

                if (screenPositions[lastBoneIndex].second &&
                    screenPositions[currentBoneIndex].second) {

                    DrawLine(
                        Canvas,
                        screenPositions[lastBoneIndex].first,
                        screenPositions[currentBoneIndex].first,
                        1.0f,
                        lineColor
                    );
                }
            }
            lastBone = currentBone;
        }
    }

    constexpr float HeadOffsetY = 10.0f;
    constexpr float StartLineY = 80.0f;

    FVector2D targetPos = { HeadPosSC.X, HeadPosSC.Y - HeadOffsetY };
    FVector2D screenCenter = {
        static_cast<float>(screenWidth) / 2.0f,
        StartLineY
    };

    FLinearColor lineColor2 = IsVisible
        ? FLinearColor(0.f, 1.f, 0.f, 1.f)
        : FLinearColor(1.f, 0.f, 0.f, 1.f);

    DrawLine(Canvas, screenCenter, targetPos, 1.5f, lineColor2);
}


////==========================================================================================================//                             
if (Config.PlayerESP.Name) {
    FVector BelowRoot = Root;
    BelowRoot.Z -= 55.0f;

    FVector2D BelowRootSc;
    if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, BelowRoot, false, &BelowRootSc)) {
        std::wstring wsName;
        std::wstring wsDist;
        std::wstring wsFullInfo;

        if (Player->bEnsure) {
            // Bot
            wsName = L"Bot";
            wsDist = std::to_wstring((int)Distance) + L"M";   // ✅ bot ke liye bhi distance set
        } else {
            // Player
            std::wstring wsTeam = L"(" + std::to_wstring(Player->TeamID) + L")";
            wsName = Player->PlayerName.ToWString();
            wsFullInfo = wsTeam + L" - " + wsName;
            wsDist = std::to_wstring((int)Distance) + L"M";
        }

        // Adjust font size by distance
        tslFontUI->LegacyFontSize = std::max(5, 12 - (int)(Distance / 80));

        if (Player->bEnsure) {
            // Draw Bot name
            DrawOutlinedTextFPS(Canvas,
                FString(wsName),   // "Bot"
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_YELLOW, COLOR_IN, true);

            // Bot distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_WHITE, COLOR_IN, true);
            }

        } else {
            // Player Name + Team
            DrawOutlinedTextFPS(Canvas,
                FString(wsFullInfo),
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_CYAN, COLOR_IN, true);

            // Player distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    COLOR_WHITE, COLOR_IN, true);
            }

            // Weapon Info
            if (Config.PlayerESP.Grenade) {
                std::string wep = "Fist";

                if (auto WeaponManagerComponent = Player->WeaponManagerComponent) {
                    auto PropSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                    if ((int)PropSlot.GetValue() >= 1 && (int)PropSlot.GetValue() <= 3) {
                        if (auto CurrentWeaponReplicated = WeaponManagerComponent->CurrentWeaponReplicated) {
                            wep = CurrentWeaponReplicated->GetWeaponName().ToString();
                        }
                    }
                }

                // Convert std::string → std::wstring
                std::wstring wwep(wep.begin(), wep.end());

                // Default green
                FLinearColor color = FLinearColor(0.f, 1.f, 1.f, 1.f);

                // Cyan for actual weapons
                if (
                    wep != "Fist" &&
                    wep != "BP_ShotGun_M1014_C" &&
                    wep != "BP_WEP_Sickle_C" &&
                    wep != "BP_WEP_Machete_C" &&
                    wep != "BP_WEP_Cowbar_C" &&
                    wep != "BP_WEP_Pan_C" &&
                    wep != "BP_WEP_Zombie59_Gloves_C" &&
                    wep != "BP_Grenade_Apple_Weapon_C" &&
                    wep != "BP_Other_CrossBow_C" &&
                    wep != "BP_Grenade_Shoulei_Weapon_C" &&
                    wep != "BP_Grenade_Smoke_Weapon_C" &&
                    wep != "BP_Grenade_Burn_Weapon_C"
                ) {
                    color = FLinearColor(0.f, 1.f, 1.f, 1.f);  // Cyan
                }

                DrawOutlinedTextFPS(Canvas,
                    FString(wwep),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 50),
                    color, COLOR_IN, true);
            }
        }
    }
}

////==========================================================================================================//
    if (Config.PlayerESP.Health) {
    float CurHP = std::max(0.f, std::min(Player->Health, Player->HealthMax));
    float MaxHP = Player->HealthMax;
    float KnockHP = Player->NearDeathBreath;
    float HPPercent = CurHP > 0 ? CurHP / MaxHP : KnockHP / MaxHP;

    // 🔹 Step health (10%)
    HPPercent = floor(HPPercent * 10.0f) / 10.0f;

    // 🔹 Visibility (FIXED - 3 args)
    bool isVisible = g_PlayerController && 
                     g_PlayerController->LineOfSightTo(Player, FVector(0,0,0), false);

    // 🔹 Color
    FLinearColor HPColor;

    if (CurHP <= 0) {
        HPColor = FLinearColor(0.5f, 0.f, 0.f, 1.f);
    } 
    else if (isVisible) {
        HPColor = FLinearColor(0.f, 1.f, 0.f, 1.f); // Green
    } 
    else {
        HPColor = FLinearColor(1.f, 0.f, 0.f, 1.f); // Red
    }

    FVector Head = Player->GetHeadLocation(true);
    Head.Z += 25.f;
    FVector2D Head2D;

    if (W2S(Head, &Head2D)) {
        float w = 32.f, h = 2.f;
        Head2D.X -= w / 2;
        Head2D.Y -= h * 1.3f;

        DrawFilledRect(Canvas, Head2D, w * HPPercent, h, HPColor);
    }
}
}
} 

 
if (Config.PlayerESP.Vehicle) {                
    std::vector<ASTExtraVehicleBase*> VehicleBase;
    GetAllActors(VehicleBase);
    
    for (auto actor = VehicleBase.begin(); actor != VehicleBase.end(); actor++) {
        auto Vehicle = *actor;
        if (!Vehicle->Mesh)
            continue;
        if (!Vehicle->RootComponent)
            continue;
        
        float Distance = Vehicle->GetDistanceTo(localPlayer) / 100.f;
        if (Distance > 500)
            continue;
        
        FVector2D VehiclePos;
        if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, Vehicle->RootComponent->RelativeLocation, false, &VehiclePos)) {
            std::string vehicleText = GetVehicleName(Vehicle); // Get only vehicle name
            std::string distanceText = std::to_string((int)Distance) + "M"; // Vehicle distance
           
            // Adjust font size based on distance
            tslFontUI->LegacyFontSize = max(6, 12 - (int)(Distance / 80));
            
            // Draw vehicle distance (on top)
            DrawOutlinedTextFPS(Canvas, FString(distanceText), 
                FVector2D(VehiclePos.X, VehiclePos.Y +20), 
                COLOR_CYAN, COLOR_IN, true);

            // Draw vehicle name (below distance)
            DrawOutlinedTextFPS(Canvas, FString(vehicleText), 
                FVector2D(VehiclePos.X, VehiclePos.Y), // Move name down by 15 pixels
                COLOR_WHITE, COLOR_IN, true);
        }
    }
}
                                    
                                    
                         
                                    
if (Config.PlayerESP.ItemEsp) {
    std::vector<APickUpListWrapperActor*> LootboxBase;
    GetAllActors(LootboxBase);
                   
    for (auto actor = LootboxBase.begin(); actor != LootboxBase.end(); actor++) {
        auto Pick = *actor;                                             
                                                
        if (!Pick->RootComponent)
            continue;

        float Distance = Pick->GetDistanceTo(localPlayer) / 100.0f;
                                                                    
        if (Distance > 150.0)
            continue;

        FVector2D PickUpListsPos;                  
                    
        if (W2S(Pick->K2_GetActorLocation(), &PickUpListsPos)) {
            std::string lootBoxText = "LootBox";
            std::string distanceText = std::to_string((int)Distance) + "M";

            tslFontUI->LegacyFontSize = max(7, 12 - (int)(Distance / 80));

            // LootBox naam draw karo
            DrawOutlinedTextFPS(Canvas, FString(distanceText), {PickUpListsPos.X, PickUpListsPos.Y +20}, COLOR_GREEN, COLOR_IN, true);
            
            // Distance naam ke niche draw karo
            DrawOutlinedTextFPS(Canvas, FString(lootBoxText), {PickUpListsPos.X, PickUpListsPos.Y }, COLOR_GREEN, COLOR_IN, true);
        }
    }
}
                
	
g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;

                           
std::wstring s;	
std::wstring a = L"0";
					s += std::to_wstring(totalEnemies + totalBots);
					if (totalEnemies + totalBots == 0) {
				
				    DrawFilledRect(Canvas, { (float)screenWidth / 2 - 15, 85 }, 30, 34, White);
					Drawtext(Canvas, FString(a), { (float)(float)screenWidth / 2, 90 }, Red,White, 15, true);
					}
					else
					{
					
					    DrawFilledRect(Canvas, { (float)screenWidth / 2 - 15, 85 }, 30, 34, White);
					    Drawtext(Canvas, FString(s), { (float)(float)screenWidth / 2, 90 }, Black,White,15, true);
					}
}
}

tslFontUI->LegacyFontSize = 15;


std::string Aimbot = "JOIN @PRIVATE_SRC_FILES ";//Drawtext
tslFontUI->LegacyFontSize = 15;
DrawOutlinedTextFPS(Canvas, FString(Aimbot.c_str()), { (float)screenWidth /10 + screenWidth/1.3f, 680 }, COLOR_RED, COLOR_RED,true);
    
    
    



if (g_LocalPlayer && g_PlayerController) {
        Config.PlayerESP.Line = true;
        Config.PlayerESP.Skeleton = true;
        Config.PlayerESP.Health = true;
        Config.PlayerESP.Name = true;
        Config.PlayerESP.Distance = true;
        Config.PlayerESP.TeamID = true;
        Config.PlayerESP.Vehicle = true;
     // Config.PlayerESP.Grenade = true;
      //Config.PlayerESP.Weapon = true;
      //Config.PlayerESP.ItemEsp = true;
     // Config.PlayerESP.Alert = true;

        Config.AimBot.Enable = true;
     
        Config.AimBot.Radius = true;
        Config.AimBot.RecoilSet = 1.1f;             
        Config.AimBot.IgnoreKnocked = true;
        Config.AimBot.VisCheck = true;
        Config.AimBot.Trigger = EAimTrigger::Any;
        FOVSizea = 200.f;
        
        
        //BULLET TRACK 
        
        
   //     Bullet[1] = true;
        
        

		
				}

        Config.Bypass = true;
}





bool isChut = false;
bool showLoginScreen = true; // Show login GUI until verified
std::string loginStatusMsg = "Paste key in clipboard";
int loginAttemptCount = 0;

bool fileExists(const std::string &filePath)
{

    std::ifstream file(filePath);

    return file.good();
}

bool isFileEmpty(const std::string &filePath)
{
    std::ifstream file(filePath);
    return file.peek() == std::ifstream::traits_type::eof();
}

bool directoryExists(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
    {
        return false;
    }
}

static char keyForLogin[64];
void GetKey()
{
    char keypath[256];

    sprintf(keypath, "/sdcard/Android/obb/%s/key.lic", GamePackage);

    int fd = open(keypath, O_RDONLY);
    read(fd, &keyForLogin, sizeof(keyForLogin));
    close(fd);
}


void logError(const char *errorMessage)
{
    char filePath[256];
    sprintf(filePath, "/sdcard/Android/obb/%s/Error.txt", GamePackage);

    int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fileDescriptor != -1)
    {
        write(fileDescriptor, errorMessage, strlen(errorMessage));
        write(fileDescriptor, "\n", 1);

        close(fileDescriptor);
    }
}

bool saveClipboardTextToFile(const char *filePath, const char *clipboardText)
{
    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        return false;
    }

    size_t len = std::strlen(clipboardText);
    ssize_t written = write(fd, clipboardText, len);

    if (written != len)
    {
        return false;
    }
    close(fd);

    return true;
}

int DownloadFile(const char *url, const char *outputPath)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return 1;
    }
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            std::ofstream outputFile(outputPath, std::ios::binary);
            if (!outputFile)
            {
                logError(("Couldn't Download File"));
            }
            else
            {
                outputFile.write(chunk.memory, chunk.size);
                outputFile.close();

                return 0;
            }
        }
        else
        {
            logError(("libcurl Failed ~ Check Your Internet Connection ~"));
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}
 


void *LoginThread(void *arg)
{
    // Wait for g_App to be set by Chameli thread
    while (!g_App)
    {
        sleep(1);
    }
    sleep(2); // Extra wait for game to stabilize

    std::string ClipboardText;
    std::string Keystatus;

    // Show initial toast
    showToast("Paste your license key in clipboard");
    sleep(2);

    while (!isChut)
    {
        // Read clipboard
        ClipboardText = getClipboardText();

        if (ClipboardText.empty() || ClipboardText.length() < 5)
        {
            loginStatusMsg = "Clipboard empty - paste key!";
            if (loginAttemptCount == 0) {
                showToast("Paste license key in clipboard");
            }
            loginAttemptCount++;
            if (loginAttemptCount % 10 == 0) {
                showToast("Waiting for key in clipboard...");
            }
            sleep(3);
            continue;
        }

        // Save clipboard to file
        std::ofstream keyFile(Filepath);
        if (!keyFile)
        {
            loginStatusMsg = "Cannot write key file!";
            showToast("Error: Cannot write key file");
            sleep(5);
            continue;
        }
        keyFile << ClipboardText;
        keyFile.close();

        // Read key back
        GetKey();

        if (strlen(keyForLogin) < 5)
        {
            loginStatusMsg = "Invalid key format";
            showToast("Invalid key - try again");
            sleep(3);
            continue;
        }

        // Attempt login
        loginStatusMsg = "Validating key...";
        showToast("Checking key...");
        Keystatus = Login(keyForLogin);

        if (Keystatus == "OK")
        {
            isChut = bloda && g_Auth == g_Token;
            if (isChut)
            {
                loginStatusMsg = "Login successful!";
                showToast("Key verified! Loading...");
                showLoginScreen = false;
                LOGI("Login successful - bypass loading");
                break;
            }
            else
            {
                loginStatusMsg = "Token mismatch!";
                showToast("Token verification failed");
                // Delete bad key file
                system(("rm -rf " + Filepath).c_str());
                memset(keyForLogin, 0, sizeof(keyForLogin));
                sleep(5);
            }
        }
        else
        {
            loginStatusMsg = "Failed: " + Keystatus;
            std::string toastMsg = "Key invalid: " + Keystatus;
            if (toastMsg.length() > 60) toastMsg = toastMsg.substr(0, 60);
            showToast(toastMsg.c_str());
            logError(Keystatus.c_str());
            // Delete bad key file
            system(("rm -rf " + Filepath).c_str());
            memset(keyForLogin, 0, sizeof(keyForLogin));
            sleep(5);
            showToast("Paste correct key in clipboard");
        }
    }

    return nullptr;
}

//INNER BT 

void clampAngles(FRotator &angles)
{
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;
    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;
    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}

int GetIndex(int current, int max) {
    if (current >= max - 1) return 0;
    return current + 1;
}

void (*ThunderBulletInner)(uintptr_t Weapon, FVector StartLoc, FRotator StartRot, int ShootID);
void xThunderBulletInner(uintptr_t Weapon, FVector StartLoc, FRotator StartRot, int ShootID)
{
    if (Bullet[1]) {
        auto Target = GetTargetForAim();
        if (Target != 0) {
            FVector targetAimPos = Target->GetBonePos("Head", {});
            
            if (!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, targetAimPos, false)) {
                targetAimPos = Target->GetBonePos("neck_01", {});
            }
            
            if (auto WeaponManagerComponent = g_LocalPlayer->WeaponManagerComponent) {
                if (auto CurrentWeaponReplicated = (ASTExtraShootWeapon*)WeaponManagerComponent->CurrentWeaponReplicated) {

                    float distance = g_LocalPlayer->GetDistanceTo(Target);
                    float BulletFireSpeed = CurrentWeaponReplicated->GetBulletFireSpeedFromEntity();

                    if (BulletFireSpeed > 0.0f) {
                        float timeToTravel = distance / BulletFireSpeed;
                        auto CurrentVehicle = Target->CurrentVehicle;
                        if (CurrentVehicle) {
                            FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
                            targetAimPos.X += LinearVelocity.X * timeToTravel;
                            targetAimPos.Y += LinearVelocity.Y * timeToTravel;
                            targetAimPos.Z += LinearVelocity.Z * timeToTravel;
                        } else {
                            FVector LinearVelocity = Target->GetVelocity();
                            targetAimPos.X += LinearVelocity.X * timeToTravel;
                            targetAimPos.Y += LinearVelocity.Y * timeToTravel;
                            targetAimPos.Z += LinearVelocity.Z * timeToTravel;
                        }
                    }
                    
                    FRotator gunrotaton = StartRot;
                    FRotator aimrotation = ToRotator(StartLoc, targetAimPos);
                    
                    gunrotaton.Pitch = aimrotation.Pitch;
                    gunrotaton.Yaw = aimrotation.Yaw;
                    gunrotaton.Roll = 0.0f;
                    
                    clampAngles(gunrotaton);
                    
                    return ThunderBulletInner(Weapon, StartLoc, gunrotaton, ShootID);
                }
            }
        }
    }
    return ThunderBulletInner(Weapon, StartLoc, StartRot, ShootID);
}

void *(*orig_esprender)(UGameViewportClient* ViewportClient, UCanvas* Canvas);
void *new_esprender(UGameViewportClient* ViewportClient, UCanvas* Canvas) {
    // Draw login screen if not logged in
    if (showLoginScreen && tslFontUI) {
        float sw = Canvas->SizeX;
        float sh = Canvas->SizeY;
        
        // Dark overlay
        DrawFilledRect(Canvas, FVector2D{0, 0}, sw, sh, FLinearColor(0, 0, 0, 0.85f));
        
        // Login box
        float boxW = sw * 0.6f;
        float boxH = sh * 0.45f;
        float boxX = (sw - boxW) / 2;
        float boxY = (sh - boxH) / 2;
        
        // Box background
        DrawFilledRect(Canvas, FVector2D{boxX, boxY}, boxW, boxH, FLinearColor(0.08f, 0.08f, 0.12f, 0.95f));
        
        // Box border
        DrawRectangle(Canvas, FVector2D{boxX, boxY}, boxW, boxH, 2.0f, FLinearColor(0.2f, 0.6f, 1.0f, 1.0f));
        
        // Title bar
        DrawFilledRect(Canvas, FVector2D{boxX, boxY}, boxW, 40.0f, FLinearColor(0.15f, 0.35f, 0.85f, 1.0f));
        
        // Title text
        DrawText(Canvas, FString("LICENSE KEY LOGIN"), FVector2D{boxX + boxW/2 - 80, boxY + 12}, FLinearColor(1,1,1,1), FLinearColor(0,0,0,1), 16, true);
        
        // Instructions
        DrawText(Canvas, FString("1. Copy your license key"), FVector2D{boxX + 20, boxY + 55}, FLinearColor(0.8f,0.8f,0.8f,1), FLinearColor(0,0,0,1), 12, false);
        DrawText(Canvas, FString("2. Paste in phone clipboard"), FVector2D{boxX + 20, boxY + 75}, FLinearColor(0.8f,0.8f,0.8f,1), FLinearColor(0,0,0,1), 12, false);
        DrawText(Canvas, FString("3. App will auto-detect and verify"), FVector2D{boxX + 20, boxY + 95}, FLinearColor(0.8f,0.8f,0.8f,1), FLinearColor(0,0,0,1), 12, false);
        
        // Clipboard preview box
        DrawFilledRect(Canvas, FVector2D{boxX + 20, boxY + 120}, boxW - 40, 35.0f, FLinearColor(0.02f, 0.02f, 0.05f, 1.0f));
        DrawRectangle(Canvas, FVector2D{boxX + 20, boxY + 120}, boxW - 40, 35.0f, 1.0f, FLinearColor(1.0f, 0.5f, 0, 1.0f));
        
        std::string clipPreview = getClipboardText();
        if (clipPreview.length() > 35) clipPreview = clipPreview.substr(0, 35) + "...";
        if (clipPreview.empty()) clipPreview = "(empty - paste key here)";
        DrawText(Canvas, FString(clipPreview.c_str()), FVector2D{boxX + 28, boxY + 130}, FLinearColor(1,1,1,1), FLinearColor(0,0,0,1), 11, false);
        
        // Status message
        FLinearColor statusColor = FLinearColor(1.0f, 0.8f, 0, 1.0f); // Yellow default
        if (loginStatusMsg.find("successful") != std::string::npos) {
            statusColor = FLinearColor(0, 1, 0, 1); // Green
        } else if (loginStatusMsg.find("Failed") != std::string::npos || loginStatusMsg.find("invalid") != std::string::npos || loginStatusMsg.find("Invalid") != std::string::npos) {
            statusColor = FLinearColor(1, 0.3f, 0.3f, 1); // Red
        } else if (loginStatusMsg.find("Validating") != std::string::npos || loginStatusMsg.find("Checking") != std::string::npos) {
            statusColor = FLinearColor(0.5f, 0.8f, 1.0f, 1); // Blue
        }
        DrawText(Canvas, FString(loginStatusMsg.c_str()), FVector2D{boxX + 20, boxY + 170}, statusColor, FLinearColor(0,0,0,1), 13, false);
        
        // Footer
        DrawText(Canvas, FString("Contact admin for license key"), FVector2D{boxX + boxW/2 - 90, boxY + boxH - 25}, FLinearColor(0.4f,0.4f,0.5f,1), FLinearColor(0,0,0,1), 10, false);
    }
    
    RenderESP(Canvas, Canvas->SizeX, Canvas->SizeY);
    return orig_esprender(ViewportClient, Canvas);
}

int GetAndroidSdkVersion() {
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", prop_value);
    return atoi(prop_value);
}        




int32_t (*orig_ANativeWindow_getWidth)(ANativeWindow *window);
int32_t _ANativeWindow_getWidth(ANativeWindow *window)
{
    screenSizeX = orig_ANativeWindow_getWidth(window);
    return orig_ANativeWindow_getWidth(window);
}

int32_t (*orig_ANativeWindow_getHeight)(ANativeWindow *window);
int32_t _ANativeWindow_getHeight(ANativeWindow *window)
{
    screenSizeY = orig_ANativeWindow_getHeight(window);
    return orig_ANativeWindow_getHeight(window);
}
void (*orig_onInputEvent)(void *inputEvent, void *ex_ab, void *ex_ac);
void onInputEvent(void *inputEvent, void *ex_ab, void *ex_ac) {
    orig_onInputEvent(inputEvent, ex_ab, ex_ac);
    EagleGUI::onEvent((AInputEvent*)inputEvent, {(float)(screenSizeX) / (float)screenWidth, (float)(screenSizeY) / (float)screenHeight });
}






void FixGameCrash() {
        system("rm -rf /data/data/com.pubg.imobile/files/");
        
        system("rm -rf /data/data/com.pubg.imobile/files/obblib");
        system("touch /data/data/com.pubg.imobile/files/obblib");
        system("chmod 000 /data/data/com.pubg.imobile/files/obblib");
        system("rm -rf /data/data/com.pubg.imobile/files/xlog");
        system("touch /data/data/com.pubg.imobile/files/xlog");
        system("chmod 000 /data/data/com.pubg.imobile/files/xlog");
        system("rm -rf /data/data/com.pubg.imobile/app_bugly");
        system("touch /data/data/com.pubg.imobile/app_bugly");
        system("chmod 000 /data/data/com.pubg.imobile/app_bugly");
        system("rm -rf /data/data/com.pubg.imobile/app_crashrecord");
        system("touch /data/data/com.pubg.imobile/app_crashrecord");
        system("chmod 000 /data/data/com.pubg.imobile/app_crashrecord");
        system("rm -rf /data/data/com.pubg.imobile/app_crashSight");
        system("touch /data/data/com.pubg.imobile/app_crashSight");
        system("chmod 000 /data/data/com.pubg.imobile/app_crashSight");
  
  system("rm -rf /data/data/com.pubg.imobile/files/ano_tmp");
        system("touch /data/data/com.pubg.imobile/files/ano_tmp");
        system("chmod 000 /data/data/com.pubg.imobile/files/ano_tmp");
  }

  


  
  
void *Chameli(void *) {

FixGameCrash();

    while (!UE4) {
        UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }    

while (!g_App) {
		g_App = *(android_app**)(UE4 + GNativeAndroidApp_Offset);
		sleep(1);
	}
	
	FName::GNames = GetGNames();
	while (!FName::GNames) {
		FName::GNames = GetGNames();
		sleep(1);
	}
	
	UObject::GUObjectArray = (FUObjectArray*)(UE4 + GUObject_Offset);
//	Tools::Hook((void *)(UE4 + 0x6BB0CFC), (void *)xThunderBulletInner, (void **)&ThunderBulletInner);
	static bool loadFont = false;
    if (!loadFont)
    {
	    pthread_t t;
	    pthread_create(&t, 0, LoadFont, 0);
	    loadFont = true;
    }	
    
    libUE4Alloc = (DWORD)malloc(UE4_size);
    memmove((void *)libUE4Alloc, (void *)UE4, UE4_size);
    shadowhook_init(shadowhook_mode_t::SHADOWHOOK_MODE_UNIQUE, 0);    
	shadowhook_hook_func_addr((void*)(UE4 + PostRender), (void*)new_esprender, (void**)&orig_esprender);		
	shadowhook_hook_sym_name("/system/lib64/libandroid.so", "ANativeWindow_getWidth",(void *)_ANativeWindow_getWidth,(void **)&orig_ANativeWindow_getWidth);
    shadowhook_hook_sym_name("/system/lib64/libandroid.so", "ANativeWindow_getHeight",(void *)_ANativeWindow_getHeight,(void **)&orig_ANativeWindow_getHeight);		
	if (GetAndroidSdkVersion() < 35) {
        shadowhook_hook_sym_name("/system/lib64/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE",(void *)onInputEvent,(void **)&orig_onInputEvent);
    }
    if (GetAndroidSdkVersion() > 34) {
        shadowhook_hook_sym_name("/system/lib64/libinput.so", "_ZN7android12_GLOBAL__N_121initializeMotionEventERNS_11MotionEventERKNS_12InputMessageE",(void *)onInputEvent,(void **)&orig_onInputEvent);
    }
	return 0;
}

__attribute__((constructor)) void _init() {
    pthread_t Ptid;
    pthread_create(&Ptid, 0,Chameli, 0);
	pthread_create(&Ptid, 0, LoginThread, 0);
}
