#include "GolfBall.h"
#include "DrawDebugHelpers.h"

#include "SpinSimulator.h"

AGolfBall::AGolfBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	

	// Default spin values
	m_vecBallLocation = BALL_LOCATION;
	m_vecSpinAxis = FVector::UpVector;//(���� Z��==0,0,1)
	m_rotSpinAxis = FRotationMatrix::MakeFromZ(m_vecSpinAxis).Rotator(); //m_rotSpinAxis = FRotator::ZeroRotator;
	m_quatSpinAxis = FQuat::FindBetweenNormals(FVector::UpVector, m_vecSpinAxis);//deltaRotation 
	m_f32SpinSpeedDegPerSec = 360.f; //�ʴ� 1ȸ��
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
			GolfBallMesh->SetWorldRotation(m_rotSpinAxis);
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
	m_rotSpinAxis = FRotator::ZeroRotator;
	m_vecSpinAxis = FVector::UpVector;//(���� Z��==0,0,1)
	m_f32SpinSpeedDegPerSec = 360.f; //�ʴ� 1ȸ��
	bSpin = false;
}

void AGolfBall::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(m_vecBallLocation);

	m_rotSpinAxis = FRotationMatrix::MakeFromZ(m_vecSpinAxis).Rotator();
	SetActorRotation(m_rotSpinAxis);
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
		FQuat rotationQuat = FQuat(m_vecSpinAxis, FMath::DegreesToRadians(m_f32SpinSpeedDegPerSec * DeltaTime));
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


		// ����� ���� (ȸ���� �ð�ȭ)
		DrawDebugLine(GetWorld(), m_vecBallLocation - m_vecSpinAxis * 1000.f, m_vecBallLocation + m_vecSpinAxis * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

		//if (!m_vecSpinAxis.IsNearlyZero() && m_f32SpinSpeedDegPerSec != 0.0f)
		//{
		//	float deltaAngleRad = FMath::DegreesToRadians(m_f32SpinSpeedDegPerSec * DeltaTime);
		//	FQuat deltaRotation = FQuat(m_vecSpinAxis.GetSafeNormal(), deltaAngleRad);

		//	// �޽� ȸ�� ������Ʈ (���� ȸ��)
		//	FQuat curQuat = GolfBallMesh->GetComponentQuat();
		//	m_rotSpinAxis = (deltaRotation * curQuat).Rotator();
		//	GolfBallMesh->SetWorldRotation((deltaRotation * curQuat).Rotator());
		//}
	}
	
}

void AGolfBall::SetIsSpin(bool bTmp/* = false*/)
{
	if (bTmp)
	{
		UE_LOG(LogTemp, Log, TEXT("Spin Off for New Command."));
		bSpin = false;
		m_vecSpinAxis = FVector::UpVector;//(���� Z�� == 0,0,1)
		m_rotSpinAxis = FRotationMatrix::MakeFromZ(m_vecSpinAxis).Rotator();
		SetActorRotation(m_rotSpinAxis);
		m_f32SpinSpeedDegPerSec = 360.f; //�ʴ� 1ȸ��

		DrawWorldGizmoAxis();
	}
	else
	{
		if (bSpin)//bSpin = !bSpin;
		{
			m_vecSpinAxis = FVector::UpVector;//(���� Z�� == 0,0,1)
			m_rotSpinAxis = FRotationMatrix::MakeFromZ(m_vecSpinAxis).Rotator();
			SetActorRotation(m_rotSpinAxis);
			m_f32SpinSpeedDegPerSec = 360.f; //�ʴ� 1ȸ��
			bSpin = false;
			UE_LOG(LogTemp, Log, TEXT("Spin Off"));

			DrawWorldGizmoAxis();
		}
		else
		{
			bSpin = true;
			UE_LOG(LogTemp, Log, TEXT("Spin On"));
		}
	}
	
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
	m_vecSpinAxis = NewSpinAxis.GetSafeNormal();// �ݵ�� ����ȭ

	UE_LOG(LogTemp, Log, TEXT("Normalized SpinAxis (%f, %f, %f)"), m_vecSpinAxis.X, m_vecSpinAxis.Y, m_vecSpinAxis.Z);

	AlignToSpinAxis();
}

void AGolfBall::AlignToSpinAxis()
{
	if (m_vecSpinAxis.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Axis is Nearly Zero."));
		return;
	}

	// FQuat ���� ȸ�� (���� ȸ������ �����Ϸ���)
	m_quatSpinAxis = FQuat::FindBetweenNormals(FVector::UpVector, m_vecSpinAxis);//deltaRotation 
	//SetActorRotation(m_quatSpinAxis * GetActorQuat());

	// FRotationMatrix
	m_rotSpinAxis = FRotationMatrix::MakeFromZ(m_vecSpinAxis).Rotator();
	SetActorRotation(m_rotSpinAxis);
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
	FVector vecAxisX = m_vecBallLocation + FVector(1.f, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = m_vecBallLocation + FVector(0.f, 1.f, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = m_vecBallLocation + FVector(0.f, 0.f, 1.f) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
}

void AGolfBall::DrawWorldGizmoAxis()
{
	FVector vecAxisX = m_vecBallLocation + FVector(1.f, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = m_vecBallLocation + FVector(0.f, 1.f, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = m_vecBallLocation + FVector(0.f, 0.f, 1.f) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), m_vecBallLocation, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
}