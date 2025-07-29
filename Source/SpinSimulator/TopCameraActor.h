#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "CineCameraActor.h"

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

    // ���� ���� �ڵ� ��ġ ���� (������)
    void SetTarget(AActor* Target);

    void CaptureFrame();
    
private:
    AActor* TargetActor;
};
