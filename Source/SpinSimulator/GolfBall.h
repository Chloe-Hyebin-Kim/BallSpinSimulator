#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"

#include "UObject/NoExportTypes.h"


#include "GolfBall.generated.h"

/**
 * 
 */
UCLASS()
class SPINSIMULATOR_API AGolfBall : public AActor
{
	GENERATED_BODY()

public:
	AGolfBall();
	virtual ~AGolfBall();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

public:
	void SetIsSpin(bool bTmp = false);
	void SetSpinSpeed(float DegreesPerSecond);
	void SetSpinAxis(const FVector& NewSpinAxis);

	void DrawBallSpinAxis();
	void DrawWorldGizmoAxis();

public:
	const FVector& GetBallSpinAxis() { return m_vecSpinAxis; }
	const FRotator& GetBallRotator() { return m_rotSpinAxis; }

private:
	void SetVisible(bool isVisible);
	void AlignToSpinAxis();
	
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		bool bSpin = false;
		
	//UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		//class USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GolfBallMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_vecBallLocation;
	  
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_vecSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FQuat m_quatSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FRotator m_rotSpinAxis;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_f32SpinSpeedDegPerSec;//SpinSpeed ( 360.f == 초당 1회전)
};
