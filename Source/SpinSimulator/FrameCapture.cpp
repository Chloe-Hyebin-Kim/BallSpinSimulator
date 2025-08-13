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
    //RootComponent = SceneCapture;
    SceneCapture->SetupAttachment(RootComponent);

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

void AFrameCapture::CaptureAndSave(int idx, FVector spinAxis, float rpm)
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
	FString fileName = FString::Printf(TEXT("/SpinAxis/(%d,%d,%d)_%dRPM/Frame%02d.png"), (int)spinAxis.X, (int)spinAxis.Y, (int)spinAxis.Z,(int)rpm, idx);
    
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();

    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("RTResource is NULL!"));
        return;
    }

    TArray<FColor> OutBMP;
    RTResource->ReadPixels(OutBMP);

    // PNG로 인코딩
    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, OutBMP, PNGData);

    // 파일 경로 지정
    FString AbsolutePath = FPaths::ProjectSavedDir() + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *AbsolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *AbsolutePath);
}

void AFrameCapture::CaptureAndSave_AI(int idx, FVector spinAxis, float rpm)
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
    FString fileName = FString::Printf(TEXT("/SpinAxis_AI/%.1fRPM_(%f,%f,%f)/Frame%02d.png"), rpm, spinAxis.X, spinAxis.Y, spinAxis.Z,  idx);

    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();

    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("RTResource is NULL!"));
        return;
    }

    TArray<FColor> OutBMP;
    RTResource->ReadPixels(OutBMP);

    // PNG로 인코딩
    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, OutBMP, PNGData);

    // 파일 경로 지정
    FString AbsolutePath = FPaths::ProjectSavedDir() + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *AbsolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *AbsolutePath);
}

bool AFrameCapture::CaptureCombinations(int pitchDeg, int rollDeg/*, FVector spinAxis*/)
{
    if (!SceneCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneCapture is NULL!"));
        return false;
    }

    SceneCapture->CaptureScene();// 수동 캡처
    FString FileName = FString::Printf(TEXT("/Pitch_Roll/%d_%d.png"), pitchDeg, rollDeg);
    
    //FString FileName = FString::Printf(TEXT("(%.6f, %.6f, %.6f)_%3d_%3d.png"), spinAxis.X, spinAxis.Y, spinAxis.Z, pitchDeg, rollDeg);
    //SaveRenderTargetToPNG(FileName);

    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("RenderTarget is NULL!"));
        return false;
    }

    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();

    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("RTResource is NULL!"));
        return false;
    }

    TArray<FColor> OutBMP;
    RTResource->ReadPixels(OutBMP);

    // PNG로 인코딩
    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, OutBMP, PNGData);

    // 파일 경로 지정
    FString AbsolutePath = FPaths::ProjectSavedDir() + FileName;
    bool rerult = FFileHelper::SaveArrayToFile(PNGData, *AbsolutePath);
    
	/* if (rerult)
	 {
		 UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *AbsolutePath);
	 }*/

    return rerult;
}


void AFrameCapture::SaveRenderTargetToPNG(const FString& fileName)
{
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();

    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("RTResource is NULL!"));
        return;
    }
    TArray<FColor> OutBMP;
    RTResource->ReadPixels(OutBMP);

    //int32 Width = RenderTarget->SizeX;
    //int32 Height = RenderTarget->SizeY;
    //// 수직 반전
    //for (int32 Row = 0; Row < Height / 2; ++Row)
    //{
    //    int32 IndexA = Row * Width;
    //    int32 IndexB = (Height - 1 - Row) * Width;
    //    for (int32 Col = 0; Col < Width; ++Col)
    //    {
    //        OutBMP.SwapMemory(IndexA + Col, IndexB + Col);
    //    }
    //}
    //// 수평 반전
    //for (int32 Row = 0; Row < Height; ++Row)
    //{
    //    int32 RowStart = Row * Width;
    //    for (int32 Col = 0; Col < Width / 2; ++Col)
    //    {
    //        int32 IndexA = RowStart + Col;
    //        int32 IndexB = RowStart + (Width - 1 - Col);
    //        OutBMP.SwapMemory(IndexA, IndexB);
    //    }
    //}

    // PNG로 인코딩
    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, OutBMP, PNGData);

    // 파일 경로 지정
    FString AbsolutePath = FPaths::ProjectSavedDir() + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *AbsolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *AbsolutePath);
}