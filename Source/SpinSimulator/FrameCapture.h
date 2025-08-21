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
    void CaptureAndSave(int idx, FVector spinAxis, float rpm);
    void CaptureAndSave_CSV(int idx, FVector spinAxis, float rpm);
    bool CaptureCombinations(int pitchDeg, int yawDeg/*, FVector spinAxis*/ );

private:
    void SaveRenderTargetToPNG(const FString& FileName);

private:
    UPROPERTY(EditAnywhere)
    class USceneCaptureComponent2D* SceneCapture; // ĸó�� SceneCapture

    UPROPERTY(EditAnywhere)
    class UTextureRenderTarget2D* RenderTarget; // SceneCapture�� ���� RenderTarget

    // ����� �� ������ ��� ����(���� ����ȭ ������ġ)
    UPROPERTY(EditAnywhere, Category="Spin/Options")
    bool bWaitOneFrameBeforeCapture = true;

    // ��� ���� (��: C:/SpinOut)
    UPROPERTY(EditAnywhere, Category="Spin/Output")
    FString FolderDir;
};
