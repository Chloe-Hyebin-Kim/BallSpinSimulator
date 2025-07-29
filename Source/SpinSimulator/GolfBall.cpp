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
	m_SpinAxisAsVec = FVector::UpVector;//(���� Z��==0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator(); //m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	m_DegreesPerSecond = 360.f; //�ʴ� 1ȸ�� = 60 RPM
	m_DegreesPerFrame = m_DegreesPerSecond* DPS2FPS;
	bSpin = false;


	m_BallForward=GetActorForwardVector();
	m_BallRight=GetActorRightVector();
	m_BallUp=GetActorUpVector();

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
	m_SpinAxisAsVec = FVector::UpVector;//(���� Z��==0,0,1)
	m_DegreesPerSecond = 360.f; //�ʴ� 1ȸ�� = 60 RPM
	m_DegreesPerFrame = m_DegreesPerSecond* DPS2FPS;
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
		// ���� ��ǥ�迡�� ȸ���� ��� (���� �� ����)
		FQuat rotationQuat = FQuat(m_SpinAxisAsVec, FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime));
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


		// ����� ���� (ȸ���� �ð�ȭ)
		DrawDebugLine(GetWorld(), m_vecBallLocation - m_SpinAxisAsVec * 1000.f, m_vecBallLocation + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

		//if (!m_SpinAxisAsVec.IsNearlyZero() && m_DegreesPerSecond != 0.0f)
		//{
		//	float deltaAngleRad = FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime);
		//	FQuat deltaRotation = FQuat(m_SpinAxisAsVec.GetSafeNormal(), deltaAngleRad);

		//	// �޽� ȸ�� ������Ʈ (���� ȸ��)
		//	FQuat curQuat = GolfBallMesh->GetComponentQuat();
		//	m_SpinAxisAsRot = (deltaRotation * curQuat).Rotator();
		//	GolfBallMesh->SetWorldRotation((deltaRotation * curQuat).Rotator());
		//}
	}
	
}

void AGolfBall::SetIsSpin(bool bTmp/* = false*/)
{
	bSpin = !bSpin;
}

void AGolfBall::SetSpinSpeed(float f32InputRPM)
{
	m_DegreesPerSecond = f32InputRPM * RPM2DPS; //RPM -> DegreesPerSecond
	m_DegreesPerFrame = m_DegreesPerSecond*DPS2FPS;//Degrees Per Second -> Degrees Per Frame

	UE_LOG(LogTemp, Log, TEXT("Set Spin Speed: %f RPM (%f deg/s)"), f32InputRPM, m_DegreesPerSecond);
}

void AGolfBall::SetSpinAxis(const FVector& NewSpinAxis)
{
	if (NewSpinAxis.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT(" Zero vector is not a valid spin axis."));
		return;
	}

	m_SpinAxisAsVec = FVector::UpVector;//(���� Z�� == 0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);

	m_SpinAxisAsVec = NewSpinAxis.GetSafeNormal();// �ݵ�� ����ȭ
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z���� �־��� ���⿡ ����
	SetActorRotation(m_SpinAxisAsRot);
	UE_LOG(LogTemp, Log, TEXT("Normalized SpinAxis (%f, %f, %f)"), m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);

	// ���
	UE_LOG(LogTemp, Log, TEXT("[���]"));
	UE_LOG(LogTemp, Log, TEXT("FVector �� ����: (%f, %f, %f)"), RotatedAxis.X, RotatedAxis.Y, RotatedAxis.Z);
	UE_LOG(LogTemp, Log, TEXT("FQuat ȸ��: (X=%f, Y=%f, Z=%f, W=%f)"), CombinedRot.X, CombinedRot.Y, CombinedRot.Z, CombinedRot.W);
	FRotator Euler = CombinedRot.Rotator();
	UE_LOG(LogTemp, Log, TEXT("FRotator ���Ϸ�: Pitch=%f, Yaw=%f, Roll=%f"), Euler.Pitch, Euler.Yaw, Euler.Roll);
	AlignToSpinAxis();
}

void AGolfBall::AlignToSpinAxis()
{
	if (m_SpinAxisAsVec.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Axis is Nearly Zero."));
		return;
	}

	// FQuat ���� ȸ�� (���� ȸ������ �����Ϸ���)
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	//SetActorRotation(m_SpinAxisAsQuat * GetActorQuat());

	// FRotationMatrix
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z���� �־��� ���⿡ ����
	SetActorRotation(m_SpinAxisAsRot);

	///////////////////////

	m_BallForward = GetActorForwardVector();
	m_BallRight = GetActorRightVector();
	m_BallUp = GetActorUpVector();



	///////////////////////

	// ȸ�� ���ʹϾ� ����
	//FQuat RotX = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(45));
	//FQuat RotY = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(45));
	//FQuat CombinedRot = RotY * RotX;

	//// ȸ���� Z��
	//FVector RotatedAxis = CombinedRot.RotateVector(FVector(0, 0, 1));

	//// ���
	//UE_LOG(LogTemp, Log, TEXT("[���]"));
	//UE_LOG(LogTemp, Log, TEXT("FVector �� ����: (%f, %f, %f)"), RotatedAxis.X, RotatedAxis.Y, RotatedAxis.Z);
	//UE_LOG(LogTemp, Log, TEXT("FQuat ȸ��: (X=%f, Y=%f, Z=%f, W=%f)"), CombinedRot.X, CombinedRot.Y, CombinedRot.Z, CombinedRot.W);
	//FRotator Euler = CombinedRot.Rotator();
	//UE_LOG(LogTemp, Log, TEXT("FRotator ���Ϸ�: Pitch=%f, Yaw=%f, Roll=%f"), Euler.Pitch, Euler.Yaw, Euler.Roll);

}




void AGolfBall::CaptureFrame()
{
	if (bSpin)
	{
		SetIsSpin();
	}


	for (int i = 1; i < FRAMECOUNT; ++i)
	{	
		// ���� ��ǥ�迡�� ȸ���� ��� (���� �� ����)

		float AngleDeg = m_DegreesPerFrame * i;
		float AngleRad = FMath::DegreesToRadians(AngleDeg);
		FQuat rotationQuat = FQuat(m_SpinAxisAsVec, AngleRad);// ȸ�� ���ʹϾ�
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

		UE_LOG(LogTemp, Log, TEXT("### ȸ�� �� : (%f, %f, %f)"), m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);
		UE_LOG(LogTemp, Log, TEXT("### ȸ�� ���ʹϾ�: X=%f, Y=%f, Z=%f, W=%f"), rotationQuat.X, rotationQuat.Y, rotationQuat.Z, rotationQuat.W);
		UE_LOG(LogTemp, Log, TEXT("### �������忡�� ȸ�� ��: (%f, %f, %f)"), rotationQuat.RotateVector(FVector(1,0, 0)), rotationQuat.RotateVector(FVector(0, 1,0)), rotationQuat.RotateVector(FVector(0, 0, 1)));
		UE_LOG(LogTemp, Log, TEXT("### ���Ϸ� ��: Pitch=%f, Yaw=%f, Roll=%f"),rotationQuat.Rotator().Pitch, rotationQuat.Rotator().Yaw, rotationQuat.Rotator().Roll);


		// ����� ���� (ȸ���� �ð�ȭ)
		DrawDebugLine(GetWorld(), m_vecBallLocation - m_SpinAxisAsVec * 1000.f, m_vecBallLocation + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);


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

	FVector vecAxisX = m_vecBallLocation + FVector(vecDir.X, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = m_vecBallLocation + FVector(0.f, vecDir.Y, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = m_vecBallLocation + FVector(0.f, 0.f, vecDir.Y) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);


	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecAxisX.X, vecAxisX.Y, vecAxisX.Z);
	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecAxisY.X, vecAxisY.Y, vecAxisY.Z);
	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecAxisZ.X, vecAxisZ.Y, vecAxisZ.Z);


	//// ���� ������ ���� Forward(X) �������� ��Ȯ�� ���� 
	//FVector ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector
	//// Forward�� Up�� ���� �����ϸ� �ٸ� ���� ���
	//if (FMath::Abs(FVector::DotProduct(ReferenceVec, m_SpinAxisAsVec)) > 0.99f)
	//{
	//	ReferenceVec = FVector(0.f, 1.f, 0.f);//== FVector::RightVector

	//	// ���� �� ����
	//	FVector vecForward = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //x
	//	FVector vecRight = FVector::CrossProduct(m_SpinAxisAsVec, vecForward).GetSafeNormal(); //y
	//	FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //z

	//	// ��� Ȯ��
	//	UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	//}
	//else
	//{
	//	ReferenceVec = FVector(1.f, 0.f, 0.f);//== FVector::ForwardVector

	//	// ���� �� ����
	//	FVector vecRight = FVector::CrossProduct(ReferenceVec, m_SpinAxisAsVec).GetSafeNormal(); //Y
	//	FVector vecForward = FVector::CrossProduct(m_SpinAxisAsVec, vecRight).GetSafeNormal(); //X
	//	FVector vecUp = FVector::CrossProduct(vecRight, vecForward).GetSafeNormal(); //Z

	//	// ��� Ȯ��
	//	UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Right Vector (Y): (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	//	UE_LOG(LogTemp, Log, TEXT("[Ball Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	//}

	/*
	FRotationMatrix AxisMatrix = FRotationMatrix(m_SpinAxisAsRot);

	//���ĵ� ��ǥ�� ������ �� ����
	FVector vecRight = AxisMatrix.GetUnitAxis(EAxis::X); // ���� X�� (Right)
	FVector vecUp = AxisMatrix.GetUnitAxis(EAxis::Z); // ���� Z�� (Up)
	FVector vecForward = AxisMatrix.GetUnitAxis(EAxis::Y); // ���� Y�� (Forward)

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, YCOLOR,false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Right Vector (X):   (%f, %f, %f)"), vecRight.X, vecRight.Y, vecRight.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Forward Vector (Y): (%f, %f, %f)"), vecForward.X, vecForward.Y, vecForward.Z);
	UE_LOG(LogTemp, Log, TEXT("[BallSpinAxis] Up Vector (Z):      (%f, %f, %f)"), vecUp.X, vecUp.Y, vecUp.Z);
	 */

}

void AGolfBall::DrawWorldGizmoAxis()
{
	//// ���� ������ ��Ȯ�� ���� (��: ���� Forward ����)
	//FVector ReferenceForward =FVector::ForwardVector; // �Ǵ�FVector(1.f, 0.f, 0.f)

	//// Forward�� Up�� ���� �����ϸ� �ٸ� ���� ���
	//if (FMath::Abs(FVector::DotProduct(ReferenceForward, FVector::UpVector)) > 0.99f)
	//{
	//	ReferenceForward = FVector::RightVector;
	//}

	//// ���� �� ����
	//FVector vecRight = FVector::CrossProduct(ReferenceForward, FVector::UpVector).GetSafeNormal(); //Y
	//FVector vecForward = FVector::CrossProduct(FVector::UpVector, vecRight).GetSafeNormal(); //X
	//FVector vecUp = FVector::UpVector; //Z

	//// ��� Ȯ��
	//UE_LOG(LogTemp, Warning, TEXT("Right:   %s"), *vecRight.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *vecForward.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Up:      %s"), *vecUp.ToString());

	//DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, m_vecBallLocation + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��



	FVector vecAxisX = m_vecBallLocation + FVector(1.f, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = m_vecBallLocation + FVector(0.f, 1.f, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = m_vecBallLocation + FVector(0.f, 0.f, 1.f) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);


	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Forward Vector (X):   (%f, %f, %f)"), vecAxisX.X, vecAxisX.Y, vecAxisX.Z);
	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Right Vector (Y): (%f, %f, %f)"), vecAxisY.X, vecAxisY.Y, vecAxisY.Z);
	UE_LOG(LogTemp, Log, TEXT("[World Gizmo] Up Vector (Z):      (%f, %f, %f)"), vecAxisZ.X, vecAxisZ.Y, vecAxisZ.Z);
}