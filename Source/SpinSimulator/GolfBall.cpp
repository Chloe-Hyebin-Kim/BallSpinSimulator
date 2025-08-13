#include "GolfBall.h"
#include "DrawDebugHelpers.h"
#include "Math/RotationTranslationMatrix.h"

#include "SpinSimulator.h"
#include "Kismet/KismetMathLibrary.h"

AGolfBall::AGolfBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default spin values
	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축==0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator(); //m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	m_DegreesPerSecond = 360.f; //초당 1회전 = 60 RPM
	m_DegreesPerFrame = m_DegreesPerSecond* DPS2FPS;
	bSpin = false;

	m_PitchDeg = 0;
	m_RollDeg = 0;

	m_BallForward=GetActorForwardVector();
	m_BallRight=GetActorRightVector();
	m_BallUp=GetActorUpVector();

	///////////////////////////////////////////////////////////////

	if (!HasAnyFlags(RF_ClassDefaultObject)) 
	{
		bool bTitleistBall = false;

		if (bTitleistBall)
		{
			GolfBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TitleistBall"));
			static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/Titlelist/SM_TitleistBall"));
			//static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/SM_TitleistBall"));

			//RootComponent = GolfBallMesh;
			if (meshTitleistBall.Object != nullptr)
			{

				GolfBallMesh->SetupAttachment(RootComponent);

				GolfBallMesh->SetStaticMesh(meshTitleistBall.Object);
				GolfBallMesh->SetWorldLocation(BALL_LOCATION);
				GolfBallMesh->SetWorldRotation(FRotator::ZeroRotator);
				GolfBallMesh->SetRelativeScale3D(FVector::OneVector);
				GolfBallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GolfBallMesh->SetVisibility(true);

				//SetVisible(true);
			}

		} 
		else
		{
			GolfBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GolfzonParkBall"));
			static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/SM_GolfzonPark_Ball"));

			//RootComponent = GolfBallMesh;
			if (meshTitleistBall.Object != nullptr)
			{

				GolfBallMesh->SetupAttachment(RootComponent);

				GolfBallMesh->SetStaticMesh(meshTitleistBall.Object);
				GolfBallMesh->SetWorldLocation(BALL_LOCATION);
				GolfBallMesh->SetWorldRotation(FRotator::ZeroRotator);
				GolfBallMesh->SetRelativeScale3D(FVector::OneVector);
				GolfBallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GolfBallMesh->SetVisibility(true);

				//SetVisible(true);
			}

		}

	}


	//SetActorTickEnabled(true);
}

AGolfBall::~AGolfBall()
{
	m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축==0,0,1)
	m_DegreesPerSecond = 360.f; //초당 1회전 = 60 RPM
	m_DegreesPerFrame = m_DegreesPerSecond* DPS2FPS;
	bSpin = false;
}

void AGolfBall::BeginPlay()
{
	Super::BeginPlay();

	SpinAxes.Empty();

	SetActorLocation(BALL_LOCATION);

	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	//SetActorRotation(m_SpinAxisAsRot);
	SetActorRotation(FRotator::ZeroRotator);

	FQuat actorQuat = GetActorQuat();
}

void AGolfBall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Spin Axis는 Pitch + Yaw로 결정하고,
	//그 축을 따라 회전시키는 건 Roll(또는 FQuat)이 처리한다.
	
	if (bSpin)
	{
		// 월드 좌표계에서 회전축 계산 (로컬 축 기준)
		FQuat rotationQuat = FQuat(m_SpinAxisAsVec, FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime));
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


		// 디버그 라인 (회전축 시각화)
		DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

		//if (!m_SpinAxisAsVec.IsNearlyZero() && m_DegreesPerSecond != 0.0f)
		//{
		//	float deltaAngleRad = FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime);
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
	if (bTmp)//무조건 끄기
	{
		if (bSpin){
			bSpin =false;

			bSpin = false;//무조건 스핀 끄기

			m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
			m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
			SetActorRotation(m_SpinAxisAsRot);

			// FRotationMatrix
			m_SpinAxisAsVec = m_InputSpinAxis.GetSafeNormal();// 반드시 정규화
			m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z축을 주어진 방향에 정렬
			SetActorRotation(m_SpinAxisAsRot);

		}
	} 
	else //default
	{
		bSpin = !bSpin;
	}
}

void AGolfBall::SetSpinSpeed(float f32InputRPM)
{
	m_InputRPM = f32InputRPM;
	m_DegreesPerSecond = f32InputRPM * RPM2DPS; //RPM -> DegreesPerSecond
	m_DegreesPerFrame = m_DegreesPerSecond*DPS2FPS;//Degrees Per Second -> Degrees Per Frame

	UE_LOG(LogTemp, Log, TEXT("Set Spin Speed: %f RPM (%f deg/s, %f deg/frame)"), m_InputRPM, m_DegreesPerSecond, m_DegreesPerFrame);
}

void AGolfBall::SetSpinAxis(const FVector& NewSpinAxis)
{
	if (NewSpinAxis.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT(" Zero vector is not a valid spin axis."));
		return;
	}
	m_InputSpinAxis = NewSpinAxis;
	m_SpinAxisAsVec = NewSpinAxis.GetSafeNormal();// 반드시 정규화
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z축을 주어진 방향에 정렬
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector(0.f, 0.f, 1.f), m_SpinAxisAsVec);//FQuat 직접 회전 (차분 회전량만 적용하려면)

	UE_LOG(LogTemp, Log, TEXT("Normalized SpinAxis (%f, %f, %f)"), m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);

	AlignToSpinAxis();
}

void AGolfBall::AlignToSpinAxis()
{	
	//FRotator zRotation = FRotationMatrix::MakeFromZ(FVector(0.f, 0.f, 1.f)).Rotator();//(Z축 == 0,0,1)
	//SetActorRotation(zRotation);//초기화
	//SetActorRotation(m_SpinAxisAsRot);
	
	//SetActorRotation(m_SpinAxisAsQuat * GetActorQuat());//FQuat 직접 회전 (차분 회전량만 적용하려면)


	///////////////////////
	m_BallForward = GetActorForwardVector();
	m_BallRight = GetActorRightVector();
	m_BallUp = GetActorUpVector();

	///////////////////////
	/* OLD */
	//FVector(0.f, 0.f, 1.f)으로 정렬
	m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);
	//이후에 다시 축 세팅
	m_SpinAxisAsVec = m_InputSpinAxis.GetSafeNormal();// 반드시 정규화
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z축을 주어진 방향에 정렬
	SetActorRotation(m_SpinAxisAsRot);

	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector(0.f, 0.f, 1.f), m_SpinAxisAsVec);

}




void AGolfBall::RotateBallSpinAxis(int pitchDeg, int rollDeg)
{
	float rotateRad = FMath::DegreesToRadians(1.f);
	FQuat pitchQuat = FQuat(FVector(1.f, 0.f, 0.f), rotateRad);
	FQuat rollQuat = FQuat(FVector(0.f, 1.f, 0.f), rotateRad);

	if (pitchDeg == m_PitchDeg)
	{
		m_RollDeg = rollDeg;
		m_PitchDeg = pitchDeg;

		m_BallForward = GetActorForwardVector();
		m_BallRight = GetActorRightVector();
		m_BallUp = GetActorUpVector();

		FRotator tmpWorldRot=K2_GetActorRotation();

		AddActorWorldRotation(rollQuat, false, nullptr, ETeleportType::None);//World Space 기준 회전 적용
		m_SpinAxisAsQuat = GetActorQuat();


		m_BallForward = GetActorForwardVector();
		m_BallRight = GetActorRightVector();
		m_BallUp = GetActorUpVector();

		tmpWorldRot = K2_GetActorRotation();

		UE_LOG(LogTemp, Log, TEXT("[ pitchDeg = %3d, rollDeg = %3d ] "), pitchDeg, rollDeg);
	} 
	else
	{
		float pitchRad = FMath::DegreesToRadians(pitchDeg);
		FQuat tmpQuat = FQuat(GetActorForwardVector(), pitchRad);

		m_PitchDeg = pitchDeg;
		m_RollDeg = rollDeg;

		AddActorWorldRotation(tmpQuat, false, nullptr, ETeleportType::None);//World Space 기준 회전 적용
		AddActorWorldRotation(rollQuat, false, nullptr, ETeleportType::None);//World Space 기준 회전 적용
		m_SpinAxisAsQuat = GetActorQuat();

		
	}


}


void AGolfBall::AllCombinationsOfRotateAxis(FVector newSpinAxis)
{
	// 초기화
	FRotator zRotation = FRotationMatrix::MakeFromZ(FVector(0.f, 0.f, 1.f)).Rotator();//(Z축 == 0,0,1)
	SetActorRotation(zRotation);//초기화


	// 회전축 정렬
	FVector normSpinAxis = newSpinAxis.GetSafeNormal();
	FRotator newRot = FRotationMatrix::MakeFromZ(normSpinAxis).Rotator();
	SetActorRotation(newRot);




	/*
	for (int thetaDeg = 0; thetaDeg < 180; ++thetaDeg) // elevation (pitch)
	{
		for (int phiDeg = 0; phiDeg < 360; ++phiDeg)    // azimuth (yaw)
		{
			float thetaRad = FMath::DegreesToRadians(thetaDeg);
			float phiRad = FMath::DegreesToRadians(phiDeg);

			FVector newSpinAxis;
			newSpinAxis.X = FMath::Sin(thetaRad) * FMath::Cos(phiRad);
			newSpinAxis.Y = FMath::Sin(thetaRad) * FMath::Sin(phiRad);
			newSpinAxis.Z = FMath::Cos(thetaRad);

			FTimerHandle timerHandle;
			FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
				{
					// 초기화
					SetActorRotation(FRotator::ZeroRotator);

					// 회전축 정렬
					FVector normSpinAxis = newSpinAxis.GetSafeNormal();
					FRotator newRot = FRotationMatrix::MakeFromZ(normSpinAxis).Rotator();
					SetActorRotation(newRot);

					// 출력
					UE_LOG(LogTemp, Log, TEXT("[ θ = %3d, φ = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"),
						thetaDeg, phiDeg, newSpinAxis.X, newSpinAxis.Y, newSpinAxis.Z);
				});

			GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.01f * i, false);
			++i;
		}
	}
	*/
}


void AGolfBall::RotateBallForFrameCapture(int idx)
{
	// 월드 좌표계에서 회전축 계산 (로컬 축 기준)
	float AngleDeg = m_DegreesPerFrame * idx;
	float AngleRad = FMath::DegreesToRadians(AngleDeg);
	FQuat rotationQuat = FQuat(m_SpinAxisAsVec, AngleRad);// 회전 쿼터니언  (로컬 기준 회전 축을 기준으로 AngleRad만큼 회전을 액터에 누적 적용)
	AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);//World Space 기준 누적 회전 적용
	
	// 디버그 라인 (회전축 시각화)
	DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.5f, 0, 0.1f);

	UE_LOG(LogTemp, Log, TEXT("### %d Frame ###"), idx);
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Spin Axis (Local Vec) : (%f, %f, %f)"), idx, m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);//로컬 기준 회전 축
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotation Quaternion : X = %f, Y = %f, Z = %f, W = %f"), idx, rotationQuat.X, rotationQuat.Y, rotationQuat.Z, rotationQuat.W);//(X, Y, Z) = sin(θ/2) * 회전축 ,  W = cos(θ/2)
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotated Direction (World Vec): Forward (X) = %f, Right (Y) = %f, Up (Z) = %f"), idx, rotationQuat.RotateVector(FVector(1, 0, 0)).X, rotationQuat.RotateVector(FVector(0, 1, 0)).Y, rotationQuat.RotateVector(FVector(0, 0, 1)).Z);//회전을 적용했을 때의 새로운 방향 벡터
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Euler (deg): Pitch = %f, Yaw = %f, Roll = %f"), idx, rotationQuat.Rotator().Pitch, rotationQuat.Rotator().Yaw, rotationQuat.Rotator().Roll);//쿼터니언을 오일러 각(Euler angles) 으로 변환한 값
}

void AGolfBall::RotateSpinAxis(FVector newSpinAxis)
{

	for (int32 PitchDeg = 0; PitchDeg < 360; ++PitchDeg)
	{
		// Pitch를 적용한 회전 (X축 기준)
		FRotator PitchRotator(PitchDeg, 0, 0);
		FQuat PitchQuat = PitchRotator.Quaternion();

		// Pitch에 의해 새로 정의된 X축 방향
		FVector NewX = PitchQuat.RotateVector(FVector(1, 0, 0));

		for (int32 RollDeg = 0; RollDeg < 360; ++RollDeg)
		{
			// 새 X축(NewX)을 중심으로 RollDeg도 회전
			FQuat RollQuat = FQuat(NewX.GetSafeNormal(), FMath::DegreesToRadians(RollDeg));

			// 전체 회전 쿼터니언 = Pitch 먼저, Roll 나중 (RollQuat * PitchQuat)
			FQuat TotalQuat = RollQuat * PitchQuat;

			// 이 회전의 회전축 저장
			FVector SpinAxis = TotalQuat.GetRotationAxis(); // Normalize 되어 있음
			SpinAxes.Add(SpinAxis);

			// 로그 출력
			UE_LOG(LogTemp, Log, TEXT("Pitch: %d, Roll: %d → SpinAxis: %s"), PitchDeg, RollDeg, *SpinAxis.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("총 Spin Axis 수: %d"), SpinAxes.Num());

}

void AGolfBall::CaptureFrame()
{
	if (bSpin)
	{
		bSpin = false;//무조건 스핀 끄기

		m_SpinAxisAsVec = FVector::UpVector;//(로컬 Z축 == 0,0,1)
		m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
		SetActorRotation(m_SpinAxisAsRot);

		// FRotationMatrix
		m_SpinAxisAsVec = m_InputSpinAxis.GetSafeNormal();// 반드시 정규화
		m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z축을 주어진 방향에 정렬
		SetActorRotation(m_SpinAxisAsRot);
	}


	for (int i = 1; i <= FRAMECOUNT; ++i)
	{	
		FTimerHandle timerHandle;
		FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
			{
				float AngleDeg = m_DegreesPerFrame * i;
				float AngleRad = FMath::DegreesToRadians(AngleDeg);
				FQuat rotationQuat = FQuat(m_SpinAxisAsVec, AngleRad);// 회전 쿼터니언 (로컬 기준 회전 축을 기준으로 AngleRad만큼 회전하는 쿼터니언 회전 객체)
				AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

				UE_LOG(LogTemp, Log, TEXT("### %d Frame ###"), i);
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Spin Axis (Local Vec) : (%f, %f, %f)"),i, m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);//로컬 기준 회전 축
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotation Quaternion : X = %f, Y = %f, Z = %f, W = %f"),i, rotationQuat.X, rotationQuat.Y, rotationQuat.Z, rotationQuat.W);//(X, Y, Z) = sin(θ/2) * 회전축 ,  W = cos(θ/2)
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotated Direction (World Vec): Forward (X) = %f, Right (Y) = %f, Up (Z) = %f"), i, rotationQuat.RotateVector(FVector(1, 0, 0)).X, rotationQuat.RotateVector(FVector(0, 1, 0)).Y, rotationQuat.RotateVector(FVector(0, 0, 1)).Z);//회전을 적용했을 때의 새로운 방향 벡터
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Euler (deg): Pitch = %f, Yaw = %f, Roll = %f"),i, rotationQuat.Rotator().Pitch, rotationQuat.Rotator().Yaw, rotationQuat.Rotator().Roll);//쿼터니언을 오일러 각(Euler angles) 으로 변환한 값

				// 디버그 라인 (회전축 시각화)
				DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

			});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 2.5f * i, false);
	}

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
	FVector vecDir= m_SpinAxisAsVec - FVector::UpVector;

	FVector vecAxisX = BALL_LOCATION + FVector(vecDir.X, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = BALL_LOCATION + FVector(0.f, vecDir.Y, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = BALL_LOCATION + FVector(0.f, 0.f, vecDir.Y) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);


	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecAxisX.X, vecAxisX.Y, vecAxisX.Z);
	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecAxisY.X, vecAxisY.Y, vecAxisY.Z);
	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecAxisZ.X, vecAxisZ.Y, vecAxisZ.Z);


	//// 기준 방향을 월드 Forward(X) 기준으로 명확히 설정 
	//FVector ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector
	//// Forward가 Up과 거의 평행하면 다른 벡터 사용
	//if (FMath::Abs(FVector::DotProduct(ReferenceVec, m_SpinAxisAsVec)) > 0.99f)
	//{
	//	ReferenceVec = FVector(0.f, 1.f, 0.f);//== FVector::RightVector

	//	// 로컬 축 생성
	//	FVector vecForward = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //x
	//	FVector vecRight = FVector::CrossProduct(m_SpinAxisAsVec, vecForward).GetSafeNormal(); //y
	//	FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //z

	//	// 결과 확인
	//	UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	//}
	//else
	//{
	//	ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector

	//	// 로컬 축 생성
	//	FVector vecRight = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //Y
	//	FVector vecForward = FVector::CrossProduct(m_SpinAxisAsVec, vecRight).GetSafeNormal(); //X
	//	FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //Z

	//	// 결과 확인
	//	UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	//}

	/*
	FRotationMatrix AxisMatrix = FRotationMatrix(m_SpinAxisAsRot);

	//정렬된 좌표계 기준의 축 추출
	FVector vecRight = AxisMatrix.GetUnitAxis(EAxis::X); // 로컬 X축 (Right)
	FVector vecUp = AxisMatrix.GetUnitAxis(EAxis::Z); // 로컬 Z축 (Up)
	FVector vecForward = AxisMatrix.GetUnitAxis(EAxis::Y); // 로컬 Y축 (Forward)

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, YCOLOR,false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축

	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Right Vector (X):   (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Forward Vector (Y): (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	 */

}

void AGolfBall::DrawWorldGizmoAxis()
{
	//// 기준 방향을 명확히 설정 (예: 월드 Forward 기준)
	//FVector ReferenceForward =FVector::ForwardVector; // 또는FVector(1.f, 0.f, 0.f)

	//// Forward가 Up과 거의 평행하면 다른 벡터 사용
	//if (FMath::Abs(FVector::DotProduct(ReferenceForward, FVector::UpVector)) > 0.99f)
	//{
	//	ReferenceForward = FVector::RightVector;
	//}

	//// 로컬 축 생성
	//FVector vecRight = FVector::CrossProduct(ReferenceForward, FVector::UpVector).GetSafeNormal(); //Y
	//FVector vecForward = FVector::CrossProduct(FVector::UpVector, vecRight).GetSafeNormal(); //X
	//FVector vecUp = FVector::UpVector; //Z

	//// 결과 확인
	//UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X축
	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y축
	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z축



	FVector vecAxisX = BALL_LOCATION + FVector(1.f, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = BALL_LOCATION + FVector(0.f, 1.f, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = BALL_LOCATION + FVector(0.f, 0.f, 1.f) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);


	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecAxisX.X, vecAxisX.Y, vecAxisX.Z);
	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Right Vector (Y): (%f, %f, %f)"), vecAxisY.X, vecAxisY.Y, vecAxisY.Z);
	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecAxisZ.X, vecAxisZ.Y, vecAxisZ.Z);
}