#include "GolfBall.h"
#include "DrawDebugHelpers.h"
#include "Math/RotationTranslationMatrix.h"

#include "SpinSimulator.h"
#include "Kismet/KismetMathLibrary.h"

AGolfBall::AGolfBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	

	// Default spin values
	m_vecBallLocation = BALL_LOCATION;
	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축==0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator(); //m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	m_f32SpinSpeedDegPerSec = 360.f; //초당 1회전
	bSpin = false;


	///////////////////////////////////////////////////////////////

	// Root
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent = Root;
	
	///////////////////////////////////////////////////////////////

	if (!HasAnyFlags(RF_ClassDefaultObject)) 
	{

		GolfBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TitleistBall"));
		//RootComponent = GolfBallMesh;
		static ConstructorHelpers::FObjectFinder<UStaticMesh> ballMeshAsset(TEXT("/Game/StarterContent/SM_TitleistBall"));
		if (ballMeshAsset.Object != nullptr)
		{

			GolfBallMesh->SetupAttachment(RootComponent);

			GolfBallMesh->SetStaticMesh(ballMeshAsset.Object);
			GolfBallMesh->SetWorldLocation(BALL_LOCATION);
			GolfBallMesh->SetWorldRotation(m_SpinAxisAsRot);
			GolfBallMesh->SetRelativeScale3D(FVector::OneVector);
			GolfBallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GolfBallMesh->SetVisibility(true);

			//SetVisible(true);
		}
	}

	//SetActorTickEnabled(true);
}

AGolfBall::~AGolfBall()
{
	m_vecBallLocation = BALL_LOCATION;
	m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축==0,0,1)
	m_f32SpinSpeedDegPerSec = 360.f; //초당 1회전
	bSpin = false;
}

void AGolfBall::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(m_vecBallLocation);

	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);
}

void AGolfBall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSpin)
	{
		// 월드 좌표계에서 회전축 계산 (로컬 축 기준)
		FQuat rotationQuat = FQuat(m_SpinAxisAsVec, FMath::DegreesToRadians(m_f32SpinSpeedDegPerSec * DeltaTime));
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


		// 디버그 라인 (회전축 시각화)
		DrawDebugLine(GetWorld(), m_vecBallLocation - m_SpinAxisAsVec * 1000.f, m_vecBallLocation + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

		//if (!m_SpinAxisAsVec.IsNearlyZero() && m_f32SpinSpeedDegPerSec != 0.0f)
		//{
		//	float deltaAngleRad = FMath::DegreesToRadians(m_f32SpinSpeedDegPerSec * DeltaTime);
		//	FQuat deltaRotation = FQuat(m_SpinAxisAsVec.GetSafeNormal(), deltaAngleRad);

		//	// 메시 회전 업데이트 (로컬 회전)
		//	FQuat curQuat = GolfBallMesh->GetComponentQuat();
		//	m_SpinAxisAsRot = (deltaRotation * curQuat).Rotator();
		//	GolfBallMesh->SetWorldRotation((deltaRotation * curQuat).Rotator());
		//}
	}
	
}

void AGolfBall::SetIsSpin(bool bTmp/* = false*/)
{
	bSpin = !bSpin;
	//if (bTmp)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Spin Off for New Command."));
	//	bSpin = false;
	//	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
	//	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	//	SetActorRotation(m_SpinAxisAsRot);
	//	m_f32SpinSpeedDegPerSec = 360.f; //초당 1회전

	//	DrawWorldGizmoAxis();
	//}
	//else
	//{
	//	if (bSpin)//bSpin = !bSpin;
	//	{
	//		m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
	//		m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	//		SetActorRotation(m_SpinAxisAsRot);
	//		m_f32SpinSpeedDegPerSec = 360.f; //초당 1회전
	//		bSpin = false;
	//		UE_LOG(LogTemp, Log, TEXT("Spin Off"));

	//		DrawWorldGizmoAxis();
	//	}
	//	else
	//	{
	//		bSpin = true;
	//		UE_LOG(LogTemp, Log, TEXT("Spin On"));
	//	}
	//}
	
}

void AGolfBall::SetSpinSpeed(float f32InputRPM)
{
	m_f32SpinSpeedDegPerSec = f32InputRPM * RPM2DPS; //RPM -> DegreesPerSecond
	UE_LOG(LogTemp, Log, TEXT("Set Spin Speed: %f RPM (%f deg/s)"), f32InputRPM, m_f32SpinSpeedDegPerSec);
}

void AGolfBall::SetSpinAxis(const FVector& NewSpinAxis)
{
	if (NewSpinAxis.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT(" Zero vector is not a valid spin axis."));
		return;
	}

	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);

	m_SpinAxisAsVec = NewSpinAxis.GetSafeNormal();// 반드시 정규화

	UE_LOG(LogTemp, Log, TEXT("Normalized SpinAxis (%f, %f, %f)"), m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);

	AlignToSpinAxis();
}

void AGolfBall::AlignToSpinAxis()
{
	if (m_SpinAxisAsVec.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Axis is Nearly Zero."));
		return;
	}

	// FQuat 직접 회전 (차분 회전량만 적용하려면)
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	//SetActorRotation(m_SpinAxisAsQuat * GetActorQuat());

	// FRotationMatrix
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z축을 주어진 방향에 정렬
	SetActorRotation(m_SpinAxisAsRot);

	///////////////////////




	///////////////////////

	// 회전 쿼터니언 생성
	//FQuat RotX = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(45));
	//FQuat RotY = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(45));
	//FQuat CombinedRot = RotY * RotX;

	//// 회전된 Z축
	//FVector RotatedAxis = CombinedRot.RotateVector(FVector(0, 0, 1));

	//// 출력
	//UE_LOG(LogTemp, Log, TEXT("[결과]"));
	//UE_LOG(LogTemp, Log, TEXT("FVector 축 방향: (%f, %f, %f)"), RotatedAxis.X, RotatedAxis.Y, RotatedAxis.Z);
	//UE_LOG(LogTemp, Log, TEXT("FQuat 회전: (X=%f, Y=%f, Z=%f, W=%f)"), CombinedRot.X, CombinedRot.Y, CombinedRot.Z, CombinedRot.W);
	//FRotator Euler = CombinedRot.Rotator();
	//UE_LOG(LogTemp, Log, TEXT("FRotator 오일러: Pitch=%f, Yaw=%f, Roll=%f"), Euler.Pitch, Euler.Yaw, Euler.Roll);

}

void AGolfBall::SetVisible(bool isVisible)
{
	if (!GolfBallMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
		return;
	}

	GolfBallMesh->SetVisibility(isVisible);
}

void AGolfBall::DrawBallSpinAxis()
{
	// 기준 방향을 월드 Forward(X) 기준으로 명확히 설정 
	FVector ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector

	// Forward가 Up과 거의 평행하면 다른 벡터 사용
	if (FMath::Abs(FVector::DotProduct(ReferenceVec, m_SpinAxisAsVec)) > 0.99f)
	{
		ReferenceVec = FVector(0.f, 1.f, 0.f);//== FVector::RightVector

		// 로컬 축 생성
		FVector vecForward = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //Y
		FVector vecRight = FVector::CrossProduct(m_SpinAxisAsVec, vecForward).GetSafeNormal(); //X
		FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //Z

		// 결과 확인
		UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	}
	else
	{
		ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector

		// 로컬 축 생성
		FVector vecRight = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //Y
		FVector vecForward = FVector::CrossProduct(m_SpinAxisAsVec, vecRight).GetSafeNormal(); //X
		FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //Z

		// 결과 확인
		UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
		DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
		UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	}

	/*
	FRotationMatrix AxisMatrix = FRotationMatrix(m_SpinAxisAsRot);

	//정렬된 좌표계 기준의 축 추출
	FVector vecRight = AxisMatrix.GetUnitAxis(EAxis::X); // 로컬 X축 (Right)
	FVector vecUp = AxisMatrix.GetUnitAxis(EAxis::Z); // 로컬 Z축 (Up)
	FVector vecForward = AxisMatrix.GetUnitAxis(EAxis::Y); // 로컬 Y축 (Forward)

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, YCOLOR,false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Right Vector (X):   (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Forward Vector (Y): (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	 */

}

void AGolfBall::DrawWorldGizmoAxis()
{

	// 기준 방향을 명확히 설정 (예: 월드 Forward 기준)
	FVector ReferenceForward =FVector::ForwardVector; // 또는FVector(1.f, 0.f, 0.f)

	// Forward가 Up과 거의 평행하면 다른 벡터 사용
	if (FMath::Abs(FVector::DotProduct(ReferenceForward, FVector::UpVector)) > 0.99f)
	{
		ReferenceForward = FVector::RightVector;
	}

	// 로컬 축 생성
	FVector vecRight = FVector::CrossProduct(ReferenceForward, FVector::UpVector).GetSafeNormal(); //Y
	FVector vecForward = FVector::CrossProduct(FVector::UpVector, vecRight).GetSafeNormal(); //X
	FVector vecUp = FVector::UpVector; //Z

	// 결과 확인
	UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

	UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	UE_LOG(LogTemp, Log, TEXT("[WorldGizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);


	/*FVector vecAxisX = m_vecBallLocation + FVector(1.f, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = m_vecBallLocation + FVector(0.f, 1.f, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = m_vecBallLocation + FVector(0.f, 0.f, 1.f) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);*/
}