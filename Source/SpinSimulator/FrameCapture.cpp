#include "FrameCapture.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "HighResScreenshot.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "SpinSimulator.h"
#include "TopCameraActor.h"



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
        // 렌더타겟 포맷
        RenderTarget->InitAutoFormat(512, 512);
        RenderTarget->RenderTargetFormat = RTF_RGBA16f;
        RenderTarget->ClearColor = FLinearColor::Black;
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

    ATopCameraActor* TopCam = Cast<ATopCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopCameraActor::StaticClass()));
    if (TopCam)
    {
        FVector CamLoc = TopCam->GetActorLocation();
        FRotator CamRot = TopCam->GetActorRotation();

        //FVector CaptureLocation = BALL_LOCATION + FVector(0.f, 0.f, 45.f);
       // SceneCapture->SetWorldLocation(CaptureLocation);
       // SceneCapture->SetWorldRotation(FRotator(-90.f, 0.f, 0.f)); // 정면에서 아래를 향하도록

        SceneCapture->SetWorldLocation(CamLoc);
        SceneCapture->SetWorldRotation(CamRot);

        SceneCapture->FOVAngle = TopCam->TopCameraComponent->FieldOfView;

        UE_LOG(LogTemp, Warning, TEXT("[SceneCapture Sync] Location = %s, Rotation = %s, FOV = %.2f"), *CamLoc.ToString(), *CamRot.ToString(), SceneCapture->FOVAngle);


        float FocalLength = TopCam->TopCameraComponent->CurrentFocalLength;
        float SensorWidth = TopCam->TopCameraComponent->Filmback.SensorWidth;

        float FOV = FMath::RadiansToDegrees(2.0f * FMath::Atan(SensorWidth / (2.0f * FocalLength)));
        SceneCapture->FOVAngle = FOV;

        UE_LOG(LogTemp, Warning, TEXT("Filmback & Focal Length : FOV = %.2f"), *CamLoc.ToString(), *CamRot.ToString(), SceneCapture->FOVAngle);
    }

    // 렌더타겟 포맷
    RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    RenderTarget->ClearColor = FLinearColor::Black;
    RenderTarget->UpdateResourceImmediate(true);

    // ShowFlags 활성화
    SceneCapture->ShowFlags.SetPostProcessing(true);
    SceneCapture->ShowFlags.SetTonemapper(true);
    SceneCapture->ShowFlags.SetLighting(true); 
    SceneCapture->ShowFlags.SetBloom(true);
    SceneCapture->ShowFlags.SetAmbientOcclusion(true);
    SceneCapture->ShowFlags.SetGlobalIllumination(true);


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
	FString FileName = FString::Printf(TEXT("RotatedSpinAxis_%d,%d,%d_Frame%02d.png"), (int)spinAxis.X, (int)spinAxis.Y, (int)spinAxis.Z, idx);
	SaveRenderTargetToPNG(FileName);
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
