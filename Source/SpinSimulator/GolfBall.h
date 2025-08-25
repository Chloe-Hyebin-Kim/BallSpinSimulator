#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"

#include "UObject/NoExportTypes.h"
#include "FrameCapture.h"

#include "GolfBall.generated.h"

/**
 * 
 */

USTRUCT()
struct FSpinDOE
{
	GENERATED_BODY()
	UPROPERTY() FName CircleId;     // e.g., "circle_001"
	UPROPERTY() FVector LocalPos;   // Ball local (skeletal component space at ref-pose)
};

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
	void SetIsSpin(bool bSpin, bool bDefault = false);
	void SetSpinSpeed(float DegreesPerSecond);
	void SetSpinAxis(const FVector& NewSpinAxis);

	void DrawBallSpinAxis();
	void DrawWorldGizmoAxis();
	void CaptureFrame();
	void RotateSpinAxis(FVector newSpinAxis);
	void AllCombinationsOfRotateAxis(FVector newSpinAxis);//FRotator newRotator);
	void RotateBallSpinAxis(int pitchDeg, int rollDeg);

	void RotateBallForFrameCapture(int idx);
	void VirtualSpinCapture(AFrameCapture* CaptureActor, const FVector spinAxis, const float rpm, int idx);
	
public:
	const FVector& GetInputSpinAxis() { return m_InputSpinAxis; }
	const FVector& GetBallSpinAxis() { return m_SpinAxisAsVec; }
	const FRotator& GetBallRotator() { return m_SpinAxisAsRot; }
	const FQuat& GetBallQuaternion() { return m_SpinAxisAsQuat; }
	float GetDegreesPerFrame() { return m_DegreesPerFrame; }
	float GetDegreesPerSecond() { return m_DegreesPerSecond; }
	float GetInputRPM() { return m_InputRPM; }

	// ȸ��(rx,ry,rz, �� ����) ���� ��, �� ���� "���� ��ǥ"�� ����ؼ� CSV �� �ٷ� ������
	FString FormatCsvRow(const FString& ImageName, const FRotator& Rotator, const FSpinDOE& DotInfo, const FVector& WorldPos)const;

	void CheckVertexPosition();
	void LogUsedVerticesOnly();
	void LogAndDrawUsedVertices();

	// ��� �����
	TArray<FVector> SpinAxes;

private:
	void SetVisible(bool isVisible);
	void AlignToSpinAxis();
	void ScanBonesOnce();

	TArray<FVector> VertexLocalPos;
	TArray<FVector> VertexWorldPos;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		bool bSpin = false;

		// ��(�޽ð� ���� ����). �� ������ �ڽ����� �� ���۵��� �־�� �� 
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GolfBallMesh;

		//�� �ð�ȭ �޽�...
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* AxisBall;

	//UPROPERTY(EditAnywhere, Category="SpinLabel")
	//class USkeletalMeshComponent* BallMeshComp = nullptr;

	// �� �̸� �����Ƚ� (FBX���� �� �̸� ���� ����)
	UPROPERTY(EditAnywhere, Category="SpinLabel")
	FString CircleBonePrefix = TEXT("circle_");

	// ����� ����(������ǥ)
	UPROPERTY(VisibleAnywhere, Category="SpinLabel")
	TArray<FSpinDOE> Dots;





		int m_PitchDeg;
		int m_RollDeg;

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
		float m_InputRPM;//(�Է¹��� RPM)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerFrame;//(1������ �� ȸ�� �ϴ� ����)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerSecond;//(1�ʴ� ȸ�� �ϴ� ����)
};
