#include "GolfBall.h"
#include "DrawDebugHelpers.h"
#include "Math/RotationTranslationMatrix.h"

#include "SpinSimulator.h"
#include "Kismet/KismetMathLibrary.h"

inline bool IsValidVector(const FVector& V)
{
	return FMath::IsFinite(V.X) && FMath::IsFinite(V.Y) && FMath::IsFinite(V.Z);
}

AGolfBall::AGolfBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default spin values
	m_SpinAxisAsVec = FVector::UpVector;//(���� Z��==0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator(); //m_SpinAxisAsRot = FRotator::ZeroRotator;
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector::UpVector, m_SpinAxisAsVec);//deltaRotation 
	m_DegreesPerSecond = 360.f; //�ʴ� 1ȸ�� = 60 RPM
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
			//static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/SM_MERGED_SM_TitleistBall_2"));
			
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
			static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/GolfzonParkBall/SM_GolfzonPark_Ball"));
			//static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/SM_MERGED_SM_TitleistBall_2"));
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
	m_SpinAxisAsVec = FVector::UpVector;//(���� Z��==0,0,1)
	m_DegreesPerSecond = 360.f; //�ʴ� 1ȸ�� = 60 RPM
	m_DegreesPerFrame = m_DegreesPerSecond* DPS2FPS;
	bSpin = false;
}

void AGolfBall::BeginPlay()
{
	Super::BeginPlay();

	SpinAxes.Empty();

	SetActorLocation(BALL_LOCATION);

	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);
}

void AGolfBall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGolfBall::ScanBonesOnce()
{
	//Dots.Reset();
	//if (!BallMeshComp || !BallMeshComp->SkeletalMesh) return;

	//const USkeleton* Skel = BallMeshComp->SkeletalMesh->GetSkeleton();
	//const FReferenceSkeleton& RefSkel = Skel->GetReferenceSkeleton();
	//const int32 NumBones = RefSkel.GetNum();

	//for (int32 BoneIdx = 0; BoneIdx < NumBones; ++BoneIdx)
	//{
	//	const FName BoneName = RefSkel.GetBoneName(BoneIdx);
	//	if (!BoneName.ToString().StartsWith(CircleBonePrefix)) continue;

	//	// ���� ������ ������Ʈ-���� ��ȯ
	//	const FTransform BoneRefTM = BallMeshComp->GetBoneTransform(BoneIdx);
	//	FSpinDOE Info;
	//	Info.CircleId = BoneName;
	//	// �� ��ġ�� �� ���÷� ��ȯ: ������Ʈ ���� ���� �� �޽� ��Ʈ ���� ����(���� �ǹ̷� ���)
	//	Info.LocalPos = BoneRefTM.GetLocation();
	//	Dots.Add(Info);
	//}
}

FString AGolfBall::FormatCsvRow(const FString& ImageName, const FRotator& Rotator,const FSpinDOE& DotInfo, const FVector& WorldPos) const
{

	/*Info.LocalPos�� �� ���� ��ǥ���� �� �߽�.
���� CSV�� ���� ī�޶� / ���� ��ǥ�迡���� �� ��ġ�� �����ϹǷ� B���� ȸ�� ���� �� WorldPos�� ����Ѵ�.
���� �󺧷��� ��-���� ��ǥ(ȸ�� �� ����) �� �䱸�ϸ� Info.LocalPos�� �״�� �����ϸ� �ȴ�(�ٸ� ���� CSVó�� rx, ry, rz�� �Բ� ����)*/


	// CSV: image,rx,ry,rz,circle_id,point_x,point_y,point_z  
	// // UE: Roll=X, Pitch=Y, Yaw=Z (����)
	return FString::Printf(TEXT("%s,%d,%d,%d,%s,%.4f,%.4f,%.4f"),*ImageName,FMath::RoundToInt(Rotator.Roll),FMath::RoundToInt(Rotator.Pitch),FMath::RoundToInt(Rotator.Yaw),*DotInfo.CircleId.ToString(),	WorldPos.X, WorldPos.Y, WorldPos.Z);
}


void AGolfBall::CheckVertexPosition()
{
	if (GolfBallMesh && GolfBallMesh->GetStaticMesh())
	{
		FStaticMeshRenderData* RenderData = GolfBallMesh->GetStaticMesh()->GetRenderData();
		if (RenderData && RenderData->LODResources.Num() > 0)
		{
			//GolfBallMesh (StaticMesh)�� ���� ��ǥ�κ��� ������(Vertex) ��ǥ ���
			const FStaticMeshLODResources& smLODResource = RenderData->LODResources[0]; // LOD 0 ���
			const FPositionVertexBuffer& vertexBuffer = smLODResource.VertexBuffers.PositionVertexBuffer;

			for (uint32 i = 0; i < vertexBuffer.GetNumVertices(); ++i)//��ü ���� ����ŭ ��ȸ
			{
				FVector vertexLocalPosition = vertexBuffer.VertexPosition(i); //i��° ���� ��ǥ (���� ��ġx,y,z ��ǥ�� ����)
				FVector vertexWorldosition = GolfBallMesh->GetComponentTransform().TransformPosition(vertexLocalPosition);//���� ��ǥ�� ���� ��ǥ�� ��ȯ
				UE_LOG(LogTemp, Log, TEXT("Vertex [%d] LocalPos = %s, WorldPos[%d] = %s"), i, *vertexLocalPosition.ToString(), *vertexWorldosition.ToString());
			}

			//�ﰢ��(Triangle) �ε��� ���� ���
			const FRawStaticIndexBuffer& indexBuffer = smLODResource.IndexBuffer;//�ﰢ�� ������ ���� �ε��� �迭

			for (auto j = 0; j < indexBuffer.GetNumIndices(); j += 3)
			{
				uint32 index0 = indexBuffer.GetIndex(j);
				uint32 index1 = indexBuffer.GetIndex(j + 1);
				uint32 index2 = indexBuffer.GetIndex(j + 2);

				if (index0 >= vertexBuffer.GetNumVertices() || index1 >= vertexBuffer.GetNumVertices() || index2 >= vertexBuffer.GetNumVertices())
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid vertex indices at triangle %d"), j / 3);
					continue;
				}

				//���� ��ǥ(Local Space)
				FVector v0 = vertexBuffer.VertexPosition(index0);
				FVector v1 = vertexBuffer.VertexPosition(index1);
				FVector v2 = vertexBuffer.VertexPosition(index2);
				
				//���� ��ǥ(Local Space)->���� ��ǥ ��ȯ(World Space)
				FTransform meshTransform = GolfBallMesh->GetComponentTransform();
				FVector w0 = meshTransform.TransformPosition(v0);
				FVector w1 = meshTransform.TransformPosition(v1);
				FVector w2 = meshTransform.TransformPosition(v2);

				if (!IsValidVector(w0) || !IsValidVector(w1) || !IsValidVector(w2))
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid WorldPos Triangle[%d] (NaN or Inf)"), j / 3);
					continue;
				}
				
				UE_LOG(LogTemp, Log, TEXT("Triangle[%d] LocalPos v0: %s ,WorldPos w0: %s"), j / 3, *v0.ToString(), *w0.ToString());
				UE_LOG(LogTemp, Log, TEXT("Triangle[%d] LocalPos v1: %s ,WorldPos w0: %s"), j / 3, *v1.ToString(), *w1.ToString());
				UE_LOG(LogTemp, Log, TEXT("Triangle[%d] LocalPos v2: %s ,WorldPos w0: %s"), j / 3, *v2.ToString(), *w2.ToString());
			}
		}
	}

}


void AGolfBall::LogUsedVerticesOnly()
{
	if (GolfBallMesh && GolfBallMesh->GetStaticMesh())
	if (!GolfBallMesh || !GolfBallMesh->GetStaticMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid MeshComponent or StaticMesh"));
		return;
	}

	const FStaticMeshRenderData* RenderData = GolfBallMesh->GetStaticMesh()->GetRenderData();
	if (!RenderData || RenderData->LODResources.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid RenderData"));
		return;
	}

	int32 LODIndex = 0;
	const FStaticMeshLODResources& LODResource = RenderData->LODResources[LODIndex];
	const FPositionVertexBuffer& PosBuffer = LODResource.VertexBuffers.PositionVertexBuffer;
	const FRawStaticIndexBuffer& IndexBuffer = LODResource.IndexBuffer;
	TSet<int32> UsedVertexIndices;

	int32 VertexCount = PosBuffer.GetNumVertices();
	int32 IndexCount = IndexBuffer.GetNumIndices();

	for (int32 i = 0; i < IndexCount; ++i)
	{
		int32 Index = IndexBuffer.GetIndex(i);
		if (Index >= 0 && Index < VertexCount)
		{
			UsedVertexIndices.Add(Index);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid index %d"), Index);
		}
	}

	const FTransform MeshTransform = GolfBallMesh->GetComponentTransform();

	for (int32 i = 0; i < UsedVertexIndices.Num(); ++i)
	{
		int32 Index = UsedVertexIndices.Array()[i];
		FVector LocalPos = PosBuffer.VertexPosition(Index);
		FVector WorldPos = MeshTransform.TransformPosition(LocalPos);

		UE_LOG(LogTemp, Log, TEXT("[Vertex %d] Local: %s, World: %s"), Index, *LocalPos.ToString(), *WorldPos.ToString());
		
		if (i % 1800 == 0)
		{
			VertexLocalPos.Add(LocalPos);
			VertexWorldPos.Add(WorldPos);
		}

	}

	/*
	const FTransform MeshTransform = GolfBallMesh->GetComponentTransform();
	for (int32 Index : UsedVertexIndices)
	{
		FVector LocalPos = PosBuffer.VertexPosition(Index);
		FVector WorldPos = MeshTransform.TransformPosition(LocalPos);

		UE_LOG(LogTemp, Log, TEXT("[Vertex %d] Local: %s, World: %s"), Index, *LocalPos.ToString(), *WorldPos.ToString());
	}*/
}



void AGolfBall::LogAndDrawUsedVertices()
{
	UE_LOG(LogTemp, Log, TEXT("DrawUsedVertices."));

	const UWorld* World = GolfBallMesh->GetWorld();
	for (int32 i = 0; i < VertexWorldPos.Num(); ++i)
	{
		FVector LocalPos = VertexLocalPos[i];
		FVector WorldPos = VertexWorldPos[i];

		UE_LOG(LogTemp, Log, TEXT("[Draw Vertex %d] Local: %s, World: %s"), i, *LocalPos.ToString(), *WorldPos.ToString());

		// 2mm ������ �� �׸���
		//DrawDebugCircle(World,WorldPos,0.1f,32,FColor::Red,false, 2.0f,0 );
		DrawDebugSphere(World, WorldPos, 0.01f, 32, FColor::Red, false, 10.0f, 0);
		//DrawDebugPoint(World,WorldPos, 0.4,FColor::Red,false,10.0f,0);
	}
}





void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Spin Axis�� Pitch + Yaw�� �����ϰ�,
	//�� ���� ���� ȸ����Ű�� �� Roll(�Ǵ� FQuat)�� ó���Ѵ�.
	
	if (bSpin)
	{
		// ���� ��ǥ�迡�� ȸ���� ��� (���� �� ����)
		FQuat rotationQuat = FQuat(m_SpinAxisAsVec, FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime));
		AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


		// ����� ���� (ȸ���� �ð�ȭ)
		DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

		//if (!m_SpinAxisAsVec.IsNearlyZero() && m_DegreesPerSecond != 0.0f)
		//{
			//float deltaAngleRad = FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime);
			//FQuat deltaRotation = FQuat(m_SpinAxisAsVec.GetSafeNormal(), deltaAngleRad);

			//// �޽� ȸ�� ������Ʈ (���� ȸ��)
			//FQuat curQuat = GolfBallMesh->GetComponentQuat();
			//m_SpinAxisAsRot = (deltaRotation * curQuat).Rotator();
			//GolfBallMesh->SetWorldRotation((deltaRotation * curQuat).Rotator());
		//}
	}
	
}

void AGolfBall::SetIsSpin(bool onoff, bool bDefault/* = false*/)
{
	if (onoff)//���� �ѱ�
	{
		bSpin = true;
	} 
	else//���� ����
	{
		bSpin = false;

		if (bDefault)
		{
			bSpin = false;

			m_SpinAxisAsVec = FVector::UpVector;//(���� Z�� == 0,0,1)
			m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
			SetActorRotation(m_SpinAxisAsRot);
		}
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
	m_SpinAxisAsVec = NewSpinAxis.GetSafeNormal();// �ݵ�� ����ȭ
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z���� �־��� ���⿡ ����
	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector(0.f, 0.f, 1.f), m_SpinAxisAsVec);//FQuat ���� ȸ�� (���� ȸ������ �����Ϸ���)

	UE_LOG(LogTemp, Log, TEXT("Normalized SpinAxis (%f, %f, %f)"), m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);

	AlignToSpinAxis();
}

void AGolfBall::AlignToSpinAxis()
{	
	//FRotator zRotation = FRotationMatrix::MakeFromZ(FVector(0.f, 0.f, 1.f)).Rotator();//(Z�� == 0,0,1)
	//SetActorRotation(zRotation);//�ʱ�ȭ
	//SetActorRotation(m_SpinAxisAsRot);
	
	//SetActorRotation(m_SpinAxisAsQuat * GetActorQuat());//FQuat ���� ȸ�� (���� ȸ������ �����Ϸ���)


	///////////////////////
	m_BallForward = GetActorForwardVector();
	m_BallRight = GetActorRightVector();
	m_BallUp = GetActorUpVector();

	///////////////////////
	/* OLD */
	//FVector(0.f, 0.f, 1.f)���� ����
	m_SpinAxisAsVec = FVector::UpVector;//(���� Z�� == 0,0,1)
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
	SetActorRotation(m_SpinAxisAsRot);
	//���Ŀ� �ٽ� �� ����
	m_SpinAxisAsVec = m_InputSpinAxis.GetSafeNormal();// �ݵ�� ����ȭ
	m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z���� �־��� ���⿡ ����
	SetActorRotation(m_SpinAxisAsRot);

	m_SpinAxisAsQuat = FQuat::FindBetweenNormals(FVector(0.f, 0.f, 1.f), m_SpinAxisAsVec);

}

void AGolfBall::VirtualSpinCapture(AFrameCapture* CaptureActor, const FVector spinAxis, const float rpm, int idx)
{
	//FVector(0.f, 0.f, 1.f)���� ����
	SetActorRotation(FRotationMatrix::MakeFromZ(FVector::UpVector).Rotator());//FVector(0.f, 0.f, 1.f)���� ����

	//���Ŀ� �ٽ� �� ����
	const FVector axisNorm = spinAxis.GetSafeNormal();// �ݵ�� ����ȭ
	FRotator axisAsRot = FRotationMatrix::MakeFromZ(axisNorm).Rotator();// Z���� �־��� ���⿡ ����
	SetActorRotation(axisAsRot);

	float angleDeg = rpm * RPM2DPS * DPS2FPS * idx; //RPM -> DegreesPerSecond -> Degrees Per Frame
	float angleRad = FMath::DegreesToRadians(angleDeg);

	FQuat rotationQuat = FQuat(axisNorm, angleRad);
	AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

	CaptureActor->CaptureAndSave_CSV(idx, axisNorm, rpm);
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

		AddActorWorldRotation(rollQuat, false, nullptr, ETeleportType::None);//World Space ���� ȸ�� ����
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

		AddActorWorldRotation(tmpQuat, false, nullptr, ETeleportType::None);//World Space ���� ȸ�� ����
		AddActorWorldRotation(rollQuat, false, nullptr, ETeleportType::None);//World Space ���� ȸ�� ����
		m_SpinAxisAsQuat = GetActorQuat();

		
	}


}


void AGolfBall::AllCombinationsOfRotateAxis(FVector newSpinAxis)
{
	// �ʱ�ȭ
	FRotator zRotation = FRotationMatrix::MakeFromZ(FVector(0.f, 0.f, 1.f)).Rotator();//(Z�� == 0,0,1)
	SetActorRotation(zRotation);//�ʱ�ȭ


	// ȸ���� ����
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
					// �ʱ�ȭ
					SetActorRotation(FRotator::ZeroRotator);

					// ȸ���� ����
					FVector normSpinAxis = newSpinAxis.GetSafeNormal();
					FRotator newRot = FRotationMatrix::MakeFromZ(normSpinAxis).Rotator();
					SetActorRotation(newRot);

					// ���
					UE_LOG(LogTemp, Log, TEXT("[ �� = %3d, �� = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"),
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
	// ���� ��ǥ�迡�� ȸ���� ��� (���� �� ����)
	float AngleDeg = m_DegreesPerFrame * idx;
	float AngleRad = FMath::DegreesToRadians(AngleDeg);
	FQuat rotationQuat = FQuat(m_SpinAxisAsVec, AngleRad);// ȸ�� ���ʹϾ�  (���� ���� ȸ�� ���� �������� AngleRad��ŭ ȸ���� ���Ϳ� ���� ����)
	AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);//World Space ���� ���� ȸ�� ����
	
	// ����� ���� (ȸ���� �ð�ȭ)
	DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.5f, 0, 0.1f);

	UE_LOG(LogTemp, Log, TEXT("### %d Frame ###"), idx);
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Spin Axis (Local Vec) : (%f, %f, %f)"), idx, m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);//���� ���� ȸ�� ��
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotation Quaternion : X = %f, Y = %f, Z = %f, W = %f"), idx, rotationQuat.X, rotationQuat.Y, rotationQuat.Z, rotationQuat.W);//(X, Y, Z) = sin(��/2) * ȸ���� ,  W = cos(��/2)
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotated Direction (World Vec): Forward (X) = %f, Right (Y) = %f, Up (Z) = %f"), idx, rotationQuat.RotateVector(FVector(1, 0, 0)).X, rotationQuat.RotateVector(FVector(0, 1, 0)).Y, rotationQuat.RotateVector(FVector(0, 0, 1)).Z);//ȸ���� �������� ���� ���ο� ���� ����
	UE_LOG(LogTemp, Log, TEXT("[ %d ] Euler (deg): Pitch = %f, Yaw = %f, Roll = %f"), idx, rotationQuat.Rotator().Pitch, rotationQuat.Rotator().Yaw, rotationQuat.Rotator().Roll);//���ʹϾ��� ���Ϸ� ��(Euler angles) ���� ��ȯ�� ��
}

void AGolfBall::RotateSpinAxis(FVector newSpinAxis)
{

	for (int32 PitchDeg = 0; PitchDeg < 360; ++PitchDeg)
	{
		// Pitch�� ������ ȸ�� (X�� ����)
		FRotator PitchRotator(PitchDeg, 0, 0);
		FQuat PitchQuat = PitchRotator.Quaternion();

		// Pitch�� ���� ���� ���ǵ� X�� ����
		FVector NewX = PitchQuat.RotateVector(FVector(1, 0, 0));

		for (int32 RollDeg = 0; RollDeg < 360; ++RollDeg)
		{
			// �� X��(NewX)�� �߽����� RollDeg�� ȸ��
			FQuat RollQuat = FQuat(NewX.GetSafeNormal(), FMath::DegreesToRadians(RollDeg));

			// ��ü ȸ�� ���ʹϾ� = Pitch ����, Roll ���� (RollQuat * PitchQuat)
			FQuat TotalQuat = RollQuat * PitchQuat;

			// �� ȸ���� ȸ���� ����
			FVector SpinAxis = TotalQuat.GetRotationAxis(); // Normalize �Ǿ� ����
			SpinAxes.Add(SpinAxis);

			// �α� ���
			UE_LOG(LogTemp, Log, TEXT("Pitch: %d, Roll: %d �� SpinAxis: %s"), PitchDeg, RollDeg, *SpinAxis.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("�� Spin Axis ��: %d"), SpinAxes.Num());

}

void AGolfBall::CaptureFrame()
{
	if (bSpin)
	{
		bSpin = false;//������ ���� ����

		m_SpinAxisAsVec = FVector::UpVector;//(���� Z�� == 0,0,1)
		m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();
		SetActorRotation(m_SpinAxisAsRot);

		// FRotationMatrix
		m_SpinAxisAsVec = m_InputSpinAxis.GetSafeNormal();// �ݵ�� ����ȭ
		m_SpinAxisAsRot = FRotationMatrix::MakeFromZ(m_SpinAxisAsVec).Rotator();// Z���� �־��� ���⿡ ����
		SetActorRotation(m_SpinAxisAsRot);
	}


	for (int i = 1; i <= FRAMECOUNT; ++i)
	{	
		FTimerHandle timerHandle;
		FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
			{
				float AngleDeg = m_DegreesPerFrame * i;
				float AngleRad = FMath::DegreesToRadians(AngleDeg);
				FQuat rotationQuat = FQuat(m_SpinAxisAsVec, AngleRad);// ȸ�� ���ʹϾ� (���� ���� ȸ�� ���� �������� AngleRad��ŭ ȸ���ϴ� ���ʹϾ� ȸ�� ��ü)
				AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

				UE_LOG(LogTemp, Log, TEXT("### %d Frame ###"), i);
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Spin Axis (Local Vec) : (%f, %f, %f)"),i, m_SpinAxisAsVec.X, m_SpinAxisAsVec.Y, m_SpinAxisAsVec.Z);//���� ���� ȸ�� ��
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotation Quaternion : X = %f, Y = %f, Z = %f, W = %f"),i, rotationQuat.X, rotationQuat.Y, rotationQuat.Z, rotationQuat.W);//(X, Y, Z) = sin(��/2) * ȸ���� ,  W = cos(��/2)
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Rotated Direction (World Vec): Forward (X) = %f, Right (Y) = %f, Up (Z) = %f"), i, rotationQuat.RotateVector(FVector(1, 0, 0)).X, rotationQuat.RotateVector(FVector(0, 1, 0)).Y, rotationQuat.RotateVector(FVector(0, 0, 1)).Z);//ȸ���� �������� ���� ���ο� ���� ����
				UE_LOG(LogTemp, Log, TEXT("[ %d ] Euler (deg): Pitch = %f, Yaw = %f, Roll = %f"),i, rotationQuat.Rotator().Pitch, rotationQuat.Rotator().Yaw, rotationQuat.Rotator().Roll);//���ʹϾ��� ���Ϸ� ��(Euler angles) ���� ��ȯ�� ��

				// ����� ���� (ȸ���� �ð�ȭ)
				DrawDebugLine(GetWorld(), BALL_LOCATION - m_SpinAxisAsVec * 1000.f, BALL_LOCATION + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);

			});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 2.5f * i, false);
	}

}

void AGolfBall::SetVisible(bool isVisible)
{
	/*if (!GolfBallMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
		return;
	}

	GolfBallMesh->SetVisibility(isVisible);*/

	AxisBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TitleistBall"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshTitleistBall(TEXT("/Game/StarterContent/SM_MERGED_SM_TitleistBall_2"));
	if (meshTitleistBall.Object != nullptr)
	{

		AxisBall->SetupAttachment(RootComponent);

		AxisBall->SetStaticMesh(meshTitleistBall.Object);
		AxisBall->SetWorldLocation(BALL_LOCATION);
		AxisBall->SetWorldRotation(FRotator::ZeroRotator);
		AxisBall->SetRelativeScale3D(FVector::OneVector);
		AxisBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AxisBall->SetVisibility(true);

		//SetVisible(true);
	}
}

void AGolfBall::DrawBallSpinAxis()
{
	FVector OriginalAxis =FVector(0.f, 0.f, 1.f); //FVector::UpVector
	FQuat RotationQuat = FQuat::FindBetweenNormals(OriginalAxis, m_SpinAxisAsVec);// OriginalAxis�� m_SpinAxisAsVec �������� ȸ����Ű�� ���ʹϾ� 
	FVector RotatedAxis = RotationQuat.RotateVector(OriginalAxis); //���忡���� ���ο� ����(���� ���� ���� ����)
	FVector RotatedLocal = GetActorRotation().Quaternion().RotateVector(RotatedAxis);//���� ȸ�� ����

	FVector vecAxisX = BALL_LOCATION + FVector(RotatedLocal.X, 0.f, 0.f) * ARROWLENGTH;
	FVector vecAxisY = BALL_LOCATION + FVector(0.f, RotatedLocal.Y, 0.f) * ARROWLENGTH;
	FVector vecAxisZ = BALL_LOCATION + FVector(0.f, 0.f, RotatedLocal.Z) * ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);


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

	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

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

	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

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

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, YCOLOR,false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��

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

	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecForward * ARROWLENGTH, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// X��
	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecRight * ARROWLENGTH, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);// Y��
	//DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, BALL_LOCATION + vecUp * ARROWLENGTH, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK); // Z��



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