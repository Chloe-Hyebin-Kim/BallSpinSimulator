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
	void RotateBallForFrameCapture(int idx);

public:
	const FVector& GetInputSpinAxis() { return m_InputSpinAxis; }
	const FVector& GetBallSpinAxis() { return m_SpinAxisAsVec; }
	const FRotator& GetBallRotator() { return m_SpinAxisAsRot; }
	const FQuat& GetBallQuaternion() { return m_SpinAxisAsQuat; }
	float GetDegreesPerFrame() { return m_DegreesPerFrame; }
	float GetDegreesPerSecond() { return m_DegreesPerSecond; }

private:
	void SetVisible(bool isVisible);
	void AlignToSpinAxis();
	
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		bool bSpin = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GolfBallMesh;


	  FVector m_BallForward;
	  FVector m_BallRight;
	  FVector m_BallUp;

	  	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	  FVector m_InputSpinAxis;//Unnormalized

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FVector m_SpinAxisAsVec;//m_vecSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FQuat m_SpinAxisAsQuat;//m_quatSpinAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FRotator m_SpinAxisAsRot;//m_rotSpinAxis;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float m_DegreesPerFrame;//(1프레임 당 회전 하는 각도)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerSecond;//(1초당 회전 하는 각도)
};
