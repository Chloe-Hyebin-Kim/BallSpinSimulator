#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
        UCameraComponent* TopCameraComponent;

    // ���� ���� �ڵ� ��ġ ���� (������)
    void SetTarget(AActor* Target);
    
private:
    AActor* TargetActor;
};
