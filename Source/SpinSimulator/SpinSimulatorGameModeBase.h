// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Engine/SpotLight.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraActor.h"


//#include "UObject/ConstructorHelpers.h"
//#include "Components/SceneCaptureComponent2D.h"

#include "SpinSimulatorGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SPINSIMULATOR_API ASpinSimulatorGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpinSimulatorGameModeBase();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_vecBallLocation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FQuat m_quatSpinAxis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FRotator m_rotSpinAxis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_vecSpinAxis;
	
	//UPROPERTY(EditAnywhere) TSubclassOf<AGolfBall> GolfBallActorClass;
};
