#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"

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
    void CaptureAndSave(int idx, FVector spinAxis);
    void CaptureCombinations(int pitchDeg, int yawDeg, FVector spinAxis );

private:
    void SaveRenderTargetToPNG(const FString& FileName);

private:
    UPROPERTY()
    class USceneCaptureComponent2D* SceneCapture;

    UPROPERTY()
    class UTextureRenderTarget2D* RenderTarget;
};
