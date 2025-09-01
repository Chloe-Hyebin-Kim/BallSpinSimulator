#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Components/SceneCaptureComponent2D.h"

#include "FrameCapture.generated.h"


// Project an array of local-space vertices to 2D pixel coords of a RenderTarget
// RTSize = (100,100) 등 캡처 타겟 크기
struct FProjectedPoint
{
    bool   bOnScreen = false;  // NDC가 [-1,1] 범위 내 & w>0
    FVector2D Pixel;           // (0,0) = 좌상단, (W-1,H-1) = 우하단
    float   Depth = 0.f;       // 선택: 클립 w 또는 NDC z 등 필요시
};

UCLASS()
class AFrameCapture : public AActor
{
    GENERATED_BODY()

public:
    AFrameCapture();
    virtual ~AFrameCapture();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void CaptureAndSave(int idx, FVector spinAxis, float rpm);
    void CaptureAndSave_CSV(int idx, FVector spinAxis, float rpm);
    bool CaptureCombinations(int pitchDeg, int yawDeg/*, FVector spinAxis*/ );
    bool ProjectWorldToRenderTargetPixel(const FVector& WorldPos, FVector2D& OutPixel /* 캡처 이미지 상의 (x,y) 픽셀 좌표 */);

private:
    void SaveRenderTargetToPNG(const FString& FileName);

    FMatrix MakeViewMatrix_FromCapture();
    bool GetVertexPixelOnCapture(UStaticMeshComponent* MeshComp, int32 VertexIdx, FVector2D& OutPixel);
    bool ProjectWorldToCapturePixel(const FVector& WorldPos, FVector2D& OutPixel);
    FMatrix MakeProjectionMatrix_FromCapture(int32 RTWidth, int32 RTHeight, float NearZ = 10.f, float FarZ = 1000000.f);

public:
    void ProjectLocalVerticesArray_ToRTPixels(const USceneComponent* MeshComponent, const TArray<FVector>& LocalVertices, const FVector& CamWorldLocation,const FRotator& CamWorldRotation,float CameraFOV_Vertical_Deg, int32 RTWidth, int32 RTHeight,TArray<FProjectedPoint>& OutPixels);

    void ProjectVertices(UStaticMeshComponent* MeshComp, const TArray<FVector>& localVerts);

private:
    FMatrix MakeViewMatrix(const FVector& CamLocation, const FRotator& CamRotation);
    FMatrix MakePerspectiveMatrix_VertFOV(float FovY_Deg, float Aspect, float NearZ = 10.f, float FarZ = 1e6f);
    FMatrix MakePerspectiveMatrix_XForward(float FovY_Deg, float Aspect, float NearZ = 10.f, float FarZ = 1e6f);
    FProjectedPoint ProjectWorldToRTPixel(const FVector& WorldPos,const FMatrix& ViewMatrix,const FMatrix& ProjMatrix,int32 RTWidth, int32 RTHeight);

private:
    UPROPERTY(EditAnywhere)
    class USceneCaptureComponent2D* SceneCapture; // 캡처용 SceneCapture

    UPROPERTY(EditAnywhere)
    class UTextureRenderTarget2D* RenderTarget; // SceneCapture가 쓰는 RenderTarget

    // 저장시 한 프레임 대기 여부(렌더 동기화 안전장치)
    UPROPERTY(EditAnywhere, Category="Spin/Options")
    bool bWaitOneFrameBeforeCapture = true;

    // 출력 폴더 (예: C:/SpinOut)
    UPROPERTY(EditAnywhere, Category="Spin/Output")
    FString FolderDir;
};
