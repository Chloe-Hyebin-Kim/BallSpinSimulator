// Copyright Epic Games, Inc. All Rights Reserved.
#include "SpinSimulatorGameModeBase.h"

#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "SpinSimulator.h"
#include "TopCameraActor.h"
#include "SpinController.h"
#include "FrameCapture.h"



ASpinSimulatorGameModeBase::ASpinSimulatorGameModeBase()
{
    PlayerControllerClass = ASpinController::StaticClass(); 
    //DefaultPawnClass = AGolfBall::StaticClass();
   
   // ���� ��� ����  ( ���� ����)
    DefaultPawnClass = nullptr; // DefaultPawn ���� �� �ϵ��� ����
    //PlayerControllerClass = nullptr; //�����!
    //HUDClass = nullptr;
    //GameStateClass = nullptr;
    //SpectatorClass = nullptr;

}

void ASpinSimulatorGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    ASpinController* SpinController = Cast<ASpinController>(GetWorld()->GetFirstPlayerController());
    if (!SpinController) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // �� ����
    AGolfBall* BallActor = GetWorld()->SpawnActor<AGolfBall>(AGolfBall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	// ī�޶� ����
	ATopCameraActor* TopCam = GetWorld()->SpawnActor<ATopCameraActor>(ATopCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // ī�޶� ��ġ ����
	if (TopCam)
	{
		TopCam->SetTarget(BallActor);
        SpinController->SetViewTarget(TopCam);
	}

    //Spot Light ����
    ASpotLight* SpotLight = Cast<ASpotLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpotLight::StaticClass()));
    if (!SpotLight)
    {
        UE_LOG(LogTemp, Warning, TEXT("There is no SpotLight Actor."));
    }
    else
    {
        //SpotLight->GetLightComponent()->SetIntensity(290.0f); // ��� ����- ���� ���� 29V�� ��ప���� ����
		//SpotLight->GetLightComponent()->SetLightColor(FLinearColor(0.8f, 0.5f, 0.35f)); // �����ܼ� ���� ������
		//SpotLight->GetLightComponent()->SetOuterConeAngle(20.0f);
		//SpotLight->GetLightComponent()->SetInnerConeAngle(3.0f);
        //SpotLight->GetLightComponent()->SetAttenuationRadius(2000.f);  // ��� �л�

    }

    // ī�޶� ����
    AFrameCapture* FrameCapture = GetWorld()->SpawnActor<AFrameCapture>(AFrameCapture::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // ī�޶� ��ġ ����
    if (FrameCapture)
    {
        UE_LOG(LogTemp, Warning, TEXT("AFrameCapture!"));
    }

    SpinController->ReadRangeforSyntheticData();

    UStaticMeshComponent* MeshComp = BallActor->FindComponentByClass<UStaticMeshComponent>();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MeshSize] No UStaticMeshComponent on %s"), *BallActor->GetName());
        return;
    }

    UStaticMesh* Mesh = MeshComp->GetStaticMesh();
    if (!Mesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MeshSize] No UStaticMesh on %s"), *BallActor->GetName());
        return;
    }

    // ����Ʈ �� ���� ��ü�� ���� ũ��(����, ����Ʈ ����) - Unreal Unit(�⺻ 1uu=1cm)
    const FBox LocalBox = Mesh->GetBoundingBox();          // ���� ���� ������ AABB
    const FVector LocalSize = LocalBox.GetSize();          // X,Y,Z ��ü ����
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][Local/Asset]   Size = (X=%.3f, Y=%.3f, Z=%.3f) cm"),LocalSize.X, LocalSize.Y, LocalSize.Z);
    
    //���� �� ���� AABB ũ��(ȸ��/������ ��� �ݿ�, ȸ�� �� �� ���⿡ ���� ���� Ŀ�� �� ����)
    const FBoxSphereBounds WorldBounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
    const FVector WorldAABBSize = WorldBounds.GetBox().GetSize();
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][World/AABB]    Size = (X=%.3f, Y=%.3f, Z=%.3f) cm"),
        WorldAABBSize.X, WorldAABBSize.Y, WorldAABBSize.Z);

    //FBoxSphereBounds WorldBounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
    FVector Extent = WorldBounds.BoxExtent * 2.0f; // X, Y, Z ��ü ����
    UE_LOG(LogTemp, Log, TEXT("(WorldBounds*2)Mesh Size: X=%f, Y=%f, Z=%f"), Extent.X, Extent.Y, Extent.Z);

    //ȸ���� �����ϰ�, ������Ʈ(�Ǵ� �θ� ü��)�� ���� �����ϸ� ��(������ ���� OBB ũ��- ���� ��� ������ ���� ��� ũ�� �ľǿ�)
    const FVector AbsWorldScale = MeshComp->GetComponentTransform().GetScale3D().GetAbs();
    const FVector ScaledLocalOBB = LocalSize * AbsWorldScale; // ���� �� ������ ũ��(ȸ�� ����)
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][Local*Scale]   Size = (X=%.3f, Y=%.3f, Z=%.3f) cm (rotation ignored)"), ScaledLocalOBB.X, ScaledLocalOBB.Y, ScaledLocalOBB.Z);
}