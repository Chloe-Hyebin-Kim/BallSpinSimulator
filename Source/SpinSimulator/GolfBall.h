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

	// 회전(rx,ry,rz, 도 단위) 적용 후, 각 점의 "월드 좌표"를 계산해서 CSV 한 줄로 포맷팅
	FString FormatCsvRow(const FString& ImageName, const FRotator& Rotator, const FSpinDOE& DotInfo, const FVector& WorldPos)const;

	void CheckVertexPosition();
	void LogUsedVerticesOnly();
	void LogAndDrawUsedVertices();

	// 결과 저장용
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

		// 공(메시가 붙은 액터). 이 액터의 자식으로 점 헬퍼들이 있어야 함 
	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GolfBallMesh;

		//축 시각화 메쉬...
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* AxisBall;

	//UPROPERTY(EditAnywhere, Category="SpinLabel")
	//class USkeletalMeshComponent* BallMeshComp = nullptr;

	// 본 이름 프리픽스 (FBX에서 본 이름 패턴 통일)
	UPROPERTY(EditAnywhere, Category="SpinLabel")
	FString CircleBonePrefix = TEXT("circle_");

	// 추출된 점들(로컬좌표)
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
		float m_InputRPM;//(입력받은 RPM)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerFrame;//(1프레임 당 회전 하는 각도)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float m_DegreesPerSecond;//(1초당 회전 하는 각도)
};
