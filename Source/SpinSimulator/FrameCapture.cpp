#include "FrameCapture.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraTypes.h"
#include "SceneView.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/FileHelper.h"
#include "ImageUtils.h"
#include "StaticMeshResources.h"
#include "Engine/World.h"
#include "HighResScreenshot.h"
#include "Misc/Paths.h"

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

    bool bAI= true;
    FString folderName ="";
    if (bAI)
    {
        folderName = FString::Printf(TEXT("/SpinSimulator_AI/"));
    } 
    else
    {
        folderName = FString::Printf(TEXT("/SpinSimulator/"));
	}

    FolderDir = FPaths::ProjectSavedDir() + folderName;
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
    FString fileName = FString::Printf(TEXT("(%f,%f,%f)_%dRPM/Frame%02d.png"), spinAxis.X, spinAxis.Y, spinAxis.Z, (int)rpm, idx);
    FString absolutePath = FolderDir + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *absolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *absolutePath);
}

void AFrameCapture::CaptureAndSave_CSV(int idx, FVector spinAxis, float rpm)
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

    FString fileName = FString::Printf(TEXT("SpinAxis_IMG/%.1fRPM_(%f,%f,%f)/Frame%02d.png"), rpm, spinAxis.X, spinAxis.Y, spinAxis.Z, idx);
    FString absolutePath = FolderDir + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *absolutePath);
    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *absolutePath);
}

bool AFrameCapture::CaptureCombinations(int pitchDeg, int rollDeg/*, FVector spinAxis*/)
{
    if (!SceneCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneCapture is NULL!"));
        return false;
    }

    SceneCapture->CaptureScene();// 수동 캡처
    
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

    FString fileName = FString::Printf(TEXT("Pitch_Roll/%d_%d.png"), pitchDeg, rollDeg);
    FString absolutePath = FolderDir + fileName;
    bool rerult = FFileHelper::SaveArrayToFile(PNGData, *absolutePath);
    
	/* if (rerult)
	 {
		 UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *absolutePath);
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
    FString absolutePath = FPaths::ProjectSavedDir() + fileName;
    FFileHelper::SaveArrayToFile(PNGData, *absolutePath);

    UE_LOG(LogTemp, Log, TEXT("Saved image to: %s"), *absolutePath);
}



bool AFrameCapture::ProjectWorldToRenderTargetPixel(const FVector& WorldPos,FVector2D& OutPixel /* 캡처 이미지 상의 (x,y) 픽셀 좌표 */)
{ 

	if (!SceneCapture || !RenderTarget)
		return false;

	// 1) SceneCapture2D 카메라 파라미터를 FMinimalViewInfo로 획득
	FMinimalViewInfo ViewInfo;
	SceneCapture->GetCameraView(/*DeltaTime=*/0.f, /*out*/ViewInfo);  // SceneCapture2D 전용 구현 존재 :contentReference[oaicite:1]{index=1}

	// 2) 투영 데이터 컨테이너 준비 (+ 캡처 타깃 해상도 기준 ViewRect 설정)
	/*FSceneViewProjectionData ProjData;
	const int32 W = RenderTarget->SizeX;
	const int32 H = RenderTarget->SizeY;
	const FIntRect ViewRect(0, 0, W, H);
	ProjData.SetViewRectangle(ViewRect);*/

	// 3) ViewInfo 기반으로 ProjectionMatrix / ViewMatrix 계산
    const int32 W = RenderTarget->SizeX;
    const int32 H = RenderTarget->SizeY;
    const FMatrix View = MakeViewMatrix_FromCapture();
    const FMatrix Proj = MakeProjectionMatrix_FromCapture(W, H);

    // 4) View*Projection 합성
    const FMatrix ViewProj = View * Proj;


	// 5) 월드→스크린(픽셀) 좌표 변환
	//    bShouldCalcOutsideViewPosition=false: 프러스텀 밖/뒤에 있으면 false 반환

    const FVector4 Clip = ViewProj.TransformFVector4(FVector4(WorldPos, 1.f));
    if (Clip.W <= KINDA_SMALL_NUMBER)  // 카메라 뒤/분모 0
        return false;

    const float InvW = 1.f / Clip.W;
    const float ndcX = Clip.X * InvW;   // [-1, +1]
    const float ndcY = Clip.Y * InvW;   // [-1, +1]

    // NDC -> 픽셀 (좌상단 원점, 아래로 +Y)
    FVector2D ScreenPixel;
    ScreenPixel.X = (ndcX * 0.5f + 0.5f) * (float)W;
    ScreenPixel.Y = (1.f - (ndcY * 0.5f + 0.5f)) * (float)H;

    OutPixel = ScreenPixel; // 예: RenderTarget가 100x100이면 (0..99, 0..99) 범위

    // 프러스텀 밖 여부는 ndcX/Y 범위로 체크
    const bool bOK = (ndcX >= -1.f && ndcX <= 1.f && ndcY >= -1.f && ndcY <= 1.f);
	return true;
}




bool AFrameCapture::GetVertexPixelOnCapture(UStaticMeshComponent* MeshComp,int32 VertexIdx,FVector2D& OutPixel)
{

    //월드 -> 뷰 ->클립 -> NDC -> 픽셀
    //View = 카메라의 역변환(회전->평행이동)
    //Projection = 수직 FOV 기반 표준 퍼스펙티브(UE의 Reversed - Z를 쓰든 말든 x, y는 동일)
    //NDC -> 픽셀 :   sx = (ndc.x * 0.5 + 0.5) * W, 
    //                sy = (1 - (ndc.y * 0.5 + 0.5)) * H      (텍스처 V가 아래로 증가하므로 y 뒤집기)
    //메시가 월드원점에 정확히 놓였더라도 일반성을 위해 ComponentTransform로 월드 변환하는 코드를 쓰는 게 안전



    //1. USceneCaptureComponent2D의 월드 변환(위치,회전) 으로 ViewMatrix를 만듦
    //2. 캡처 FOV / Ortho 설정과 RT 해상도로 ProjectionMatrix를 만듦
    //3. ViewProj = View * Proj 로 3D->2D(픽셀) 투영을 직접 수행


    // 1)  USceneCaptureComponent2D의 월드 변환(위치,회전) 으로 ViewMatrix를 획득
    // 2) 투영 데이터 컨테이너 준비 (+ 캡처 타깃 해상도 기준 ViewRect 설정)
    // 3) ViewInfo 기반으로 ProjectionMatrix / ViewMatrix 계산
    // 4) View*Projection 합성
    // 5) 월드→스크린(픽셀) 좌표 변환


    if (!MeshComp) 
        return false;

    // 1) LOD0에서 로컬 정점 좌표
    FStaticMeshRenderData* RD = MeshComp->GetStaticMesh()->GetRenderData();
    if (!RD || RD->LODResources.Num() == 0) 
        return false;

    const FStaticMeshLODResources& LOD = RD->LODResources[0];
    const FPositionVertexBuffer& VB = LOD.VertexBuffers.PositionVertexBuffer;

    if ((uint32)VertexIdx >= VB.GetNumVertices()) 
        return false;

    const FVector VLocal = VB.VertexPosition(VertexIdx);//const FVector3f VLocal = VB.VertexPosition(VertexIdx);

    // 2) 컴포넌트 변환으로 월드 좌표
    const FVector VWorld = MeshComp->GetComponentTransform().TransformPosition((FVector)VLocal);

    // 3) 캡처 RT 픽셀 좌표로 투영
    return ProjectWorldToCapturePixel(VWorld, OutPixel);
}

// 월드 3D -> 캡처 RT 픽셀 좌표
bool AFrameCapture::ProjectWorldToCapturePixel(const FVector& WorldPos, FVector2D& OutPixel)
{
    if (!RenderTarget)
        return false;

    const int32 W = RenderTarget->SizeX;
    const int32 H = RenderTarget->SizeY;

    const FMatrix View = MakeViewMatrix_FromCapture();
    const FMatrix Proj = MakeProjectionMatrix_FromCapture(W, H);

    const FMatrix ViewProj = View * Proj;

    const FVector4 Clip = ViewProj.TransformFVector4(FVector4(WorldPos, 1.f));
    if (Clip.W <= KINDA_SMALL_NUMBER)  // 카메라 뒤/분모 0
        return false;

    const float InvW = 1.f / Clip.W;
    const float ndcX = Clip.X * InvW;   // [-1, +1]
    const float ndcY = Clip.Y * InvW;   // [-1, +1]

    // NDC -> 픽셀 (좌상단 원점, 아래로 +Y)
    const float sx = (ndcX * 0.5f + 0.5f) * (float)W;
    const float sy = (1.f - (ndcY * 0.5f + 0.5f)) * (float)H;

    OutPixel = FVector2D(sx, sy);
    // 프러스텀 밖 여부는 ndcX/Y 범위로 체크
    return (ndcX >= -1.f && ndcX <= 1.f && ndcY >= -1.f && ndcY <= 1.f);
}

FMatrix AFrameCapture::MakeViewMatrix_FromCapture()
{
    //1.  USceneCaptureComponent2D의 월드 변환(위치, 회전) 으로 ViewMatrix를 만듦
    const FTransform& T = SceneCapture->GetComponentTransform();
    const FVector    CamLoc = T.GetLocation();
    const FRotator   CamRot = T.Rotator();

    // 월드->뷰: 회전의 역행렬 뒤에 평행이동(-CamLoc)
    const FMatrix ViewRotMat = FInverseRotationMatrix(CamRot);
    const FMatrix ViewTrans = FTranslationMatrix(-CamLoc);

    // 언리얼 좌표계(X+전방,Y+우측,Z+상)에서 뷰 공간 정렬
    // (보통 x=우, y=상, z=전방 기반의 그래픽스 뷰로 보정할 때 축 스왑/부호행렬을 곱하지만,
    // x,y 픽셀 투영은 회전/평행이동만 맞으면 동일하게 계산)
    return ViewRotMat * ViewTrans;
}

FMatrix AFrameCapture::MakeProjectionMatrix_FromCapture(int32 RTWidth, int32 RTHeight, float NearZ/* = 10.f*/, float FarZ /*= 1000000.f*/)
{
    //2. 캡처 FOV / Ortho 설정과 RT 해상도로 ProjectionMatrix를 만듦
    if (SceneCapture->ProjectionType == ECameraProjectionMode::Orthographic)
    {
        const float OrthoWidth = SceneCapture->OrthoWidth;             // 월드 단위 폭
        const float OrthoHeight = OrthoWidth * (float)RTHeight / (float)RTWidth;
        const float Left = -OrthoWidth * 0.5f;
        const float Right = OrthoWidth * 0.5f;
        const float Bottom = -OrthoHeight * 0.5f;
        const float Top = OrthoHeight * 0.5f;

        // 표준 오쏘 투영(행렬요소는 Unreal FMatrix(행우선) 기준)
        return FMatrix(
            FPlane(2.f / (Right - Left), 0, 0, 0),
            FPlane(0, 2.f / (Top - Bottom), 0, 0),
            FPlane(0, 0, 1.f / (FarZ - NearZ), 0),   // z는 나중에 안 씀 (픽셀 투영엔 x,y만 필요)
            FPlane(-(Right + Left) / (Right - Left), -(Top + Bottom) / (Top - Bottom), -NearZ / (FarZ - NearZ), 1)
        );
    }
    else
    {
        // 원근 투영
        const float Aspect = (float)RTWidth / (float)RTHeight;
        const float HFovRad = FMath::DegreesToRadians(SceneCapture->FOVAngle * 0.5f);
        const float f = 1.f / FMath::Tan(HFovRad); // vertical fov 기준

        // 표준 D3D식(좌표 뒤집기 없이 x,y만 사용) - x,y 투영엔 충분히 일치
        return FMatrix(
            FPlane(f / Aspect, 0, 0, 0),
            FPlane(0, f, 0, 0),
            FPlane(0, 0, (FarZ) / (FarZ - NearZ), 1),
            FPlane(0, 0, (-FarZ * NearZ) / (FarZ - NearZ), 0)
        );
    }
}
