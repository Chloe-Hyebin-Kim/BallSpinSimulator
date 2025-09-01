#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Components/SceneCaptureComponent2D.h"

#include "FrameCapture.generated.h"



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
