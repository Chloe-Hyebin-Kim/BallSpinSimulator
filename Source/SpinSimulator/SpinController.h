#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GolfBall.h"

#include "SpinController.generated.h"

class AGolfBall;

UCLASS()
class SPINSIMULATOR_API ASpinController : public APlayerController
{
    GENERATED_BODY()

public:
    ASpinController();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    AGolfBall* GetBallActor() {return ControlledBallActor;};

protected:
    /** 콘솔에서 호출될 SetAxis 명령 함수 */
    void OnSwitchBallSpinCommand(const TArray<FString>& Args);
    void OnSetSpinSpeedCommand(const TArray<FString>& Args);
    void OnSetAxisCommand(const TArray<FString>& Args);
    void OnShowOriginAxisCommand(const TArray<FString>& Args);
    void OnShowBallAxisCommand(const TArray<FString>& Args);
    

protected:
    /** 골프공 액터 참조 (에디터 또는 런타임에서 설정) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spin")
        AGolfBall* ControlledBallActor;
};