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
	void CaptureFrame();

public:
	const FVector& GetBallSpinAxis() { return m_SpinAxisAsVec; }
	const FRotator& GetBallRotator() { return m_SpinAxisAsRot; }
	const FQuat& GetBallQuaternion() { return m_SpinAxisAsQuat; }

private:
	void SetVisible(bool isVisible);
	void AlignToSpinAxis();
	
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		bool bSpin = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GolfBallMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_vecBallLocation;

	  FVector m_BallForward;
	  FVector m_BallRight;
	  FVector m_BallUp;
	  
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_SpinAxisAsVec;//m_vecSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FQuat m_SpinAxisAsQuat;//m_quatSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FRotator m_SpinAxisAsRot;//m_rotSpinAxis;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float m_DegreesPerFrame;//(1������ �� ȸ�� �ϴ� ����)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerSecond;//(1�ʴ� ȸ�� �ϴ� ����)
};
