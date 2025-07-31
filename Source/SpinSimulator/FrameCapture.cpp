#include "FrameCapture.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ImageUtils.h"
#include "HighResScreenshot.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"

AFrameCapture::AFrameCapture()
{
    PrimaryActorTick.bCanEverTick = false;

    // 컴포넌트 생성
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
    RootComponent = SceneCapture;


    static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> renderTarget(TEXT("/Game/StarterContent/Blueprints/SpinRenderTarget2D"));

    if (renderTarget.Succeeded()) 
        RenderTarget = renderTarget.Object;

    if (RenderTarget)
    {
        RenderTarget->InitAutoFormat(1920, 1080);
        RenderTarget->RenderTargetFormat = RTF_RGBA8;
        RenderTarget->UpdateResourceImmediate(true);

        SceneCapture->TextureTarget = RenderTarget;
        SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
        SceneCapture->bCaptureEveryFrame = false; // 매 프레임 캡처할지 여부
        SceneCapture->bCaptureOnMovement = false; // 위치,회전 변화 시 자동 캡처 여부
    }

    //// RenderTarget 객체 생성
    //RenderTarget = NewObject<UTextureRenderTarget2D>(this, UTextureRenderTarget2D::StaticClass());
    //RenderTarget->InitAutoFormat(1920, 1080);
    //RenderTarget->RenderTargetFormat = RTF_RGBA8;
    //RenderTarget->UpdateResourceImmediate(true);

    //SceneCapture->TextureTarget = RenderTarget;
    //SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    //SceneCapture->bCaptureEveryFrame = false; // 매 프레임 캡처할지 여부
    //SceneCapture->bCaptureOnMovement = false; // 위치,회전 변화 시 자동 캡처 여부
}

AFrameCapture::~AFrameCapture()
{
   
}

void AFrameCapture::BeginPlay()
{
    Super::BeginPlay();
    // 초기화 필요 시 여기서
}

void AFrameCapture::CaptureAndSave(int idx, FVector spinAxis)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("RenderTarget is NULL!"));
        return;
    }

    if (!SceneCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneCapture is NULL!"));
        return;
    }

	SceneCapture->CaptureScene();// 수동 캡처
	FString FileName = FString::Printf(TEXT("RotatedSpinAxis_%.4f_%.4f_%.4f_Frame%02d.png"), spinAxis.X, spinAxis.Y, spinAxis.Z, idx);
	FString SavePath = FPaths::ProjectSavedDir() + FileName;
	SaveRenderTargetToPNG( SavePath);
}

void AFrameCapture::SaveRenderTargetToPNG(const FString& FileName)
{
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
   
   if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("RTResource is NULL!"));
        return;
    }

    TArray<FColor> OutBMP;
    RTResource->ReadPixels(OutBMP);

    // 수직 반전 (언리얼은 상하 반전된 이미지 반환)
    int32 Width = RenderTarget->SizeX;
    int32 Height = RenderTarget->SizeY;
    for (int32 Row = 0; Row < Height / 2; ++Row)
    {
        int32 IndexA = Row * Width;
        int32 IndexB = (Height - 1 - Row) * Width;
        for (int32 Col = 0; Col < Width; ++Col)
        {
            OutBMP.SwapMemory(IndexA + Col, IndexB + Col);
        }
    }

    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(Width, Height, OutBMP, PNGData);

    FString AbsolutePath = FPaths::ProjectSavedDir() + FileName;
    FFileHelper::SaveArrayToFile(PNGData, *AbsolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *AbsolutePath);
}
