#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "CineCameraActor.h"

#include "GolfBall.h"

#include "TopCameraActor.generated.h"

UCLASS()
class SPINSIMULATOR_API ATopCameraActor : public AActor
{
    GENERATED_BODY()

public:
    ATopCameraActor();
    virtual ~ATopCameraActor();

protected:
    virtual void BeginPlay() override;

public:
    //UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
       // UCameraComponent* TopCameraComponent;//UCameraComponent
        
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
        UCineCameraComponent* TopCameraComponent;

    // 공을 따라 자동 위치 조정 (선택적)
    void SetTarget(AGolfBall* Target);

    
private:
    AGolfBall* TargetActor;

    void CaptureFrame();
};
