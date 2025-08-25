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
   
   // 제거 대상 설정  ( 선택 사항)
    DefaultPawnClass = nullptr; // DefaultPawn 생성 안 하도록 설정
    //PlayerControllerClass = nullptr; //사용함!
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

    // 공 스폰
    AGolfBall* BallActor = GetWorld()->SpawnActor<AGolfBall>(AGolfBall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	// 카메라 스폰
	ATopCameraActor* TopCam = GetWorld()->SpawnActor<ATopCameraActor>(ATopCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // 카메라 위치 지정
	if (TopCam)
	{
		TopCam->SetTarget(BallActor);
        SpinController->SetViewTarget(TopCam);
	}

    //Spot Light 설정
    ASpotLight* SpotLight = Cast<ASpotLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpotLight::StaticClass()));
    if (!SpotLight)
    {
        UE_LOG(LogTemp, Warning, TEXT("There is no SpotLight Actor."));
    }
    else
    {
        //SpotLight->GetLightComponent()->SetIntensity(290.0f); // 밝기 설정- 조명 전압 29V를 루멘값으로 추정
		//SpotLight->GetLightComponent()->SetLightColor(FLinearColor(0.8f, 0.5f, 0.35f)); // 근적외선 유사 붉은톤
		//SpotLight->GetLightComponent()->SetOuterConeAngle(20.0f);
		//SpotLight->GetLightComponent()->SetInnerConeAngle(3.0f);
        //SpotLight->GetLightComponent()->SetAttenuationRadius(2000.f);  // 밝기 분산

    }

    // 카메라 스폰
    AFrameCapture* FrameCapture = GetWorld()->SpawnActor<AFrameCapture>(AFrameCapture::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // 카메라 위치 지정
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

    // 임포트 후 에셋 자체의 순수 크기(로컬, 임포트 기준) - Unreal Unit(기본 1uu=1cm)
    const FBox LocalBox = Mesh->GetBoundingBox();          // 에셋 로컬 공간의 AABB
    const FVector LocalSize = LocalBox.GetSize();          // X,Y,Z 전체 길이
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][Local/Asset]   Size = (X=%.3f, Y=%.3f, Z=%.3f) cm"),LocalSize.X, LocalSize.Y, LocalSize.Z);
    
    //월드 축 정렬 AABB 크기(회전/스케일 모두 반영, 회전 시 축 방향에 따라 값이 커질 수 있음)
    const FBoxSphereBounds WorldBounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
    const FVector WorldAABBSize = WorldBounds.GetBox().GetSize();
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][World/AABB]    Size = (X=%.3f, Y=%.3f, Z=%.3f) cm"),
        WorldAABBSize.X, WorldAABBSize.Y, WorldAABBSize.Z);

    //FBoxSphereBounds WorldBounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
    FVector Extent = WorldBounds.BoxExtent * 2.0f; // X, Y, Z 전체 길이
    UE_LOG(LogTemp, Log, TEXT("(WorldBounds*2)Mesh Size: X=%f, Y=%f, Z=%f"), Extent.X, Extent.Y, Extent.Z);

    //회전은 무시하고, 컴포넌트(또는 부모 체인)의 월드 스케일만 곱(로컬축 기준 OBB 크기- 제작 당시 기준의 원래 모양 크기 파악용)
    const FVector AbsWorldScale = MeshComp->GetComponentTransform().GetScale3D().GetAbs();
    const FVector ScaledLocalOBB = LocalSize * AbsWorldScale; // 로컬 축 기준의 크기(회전 무시)
    UE_LOG(LogTemp, Log, TEXT("[MeshSize][Local*Scale]   Size = (X=%.3f, Y=%.3f, Z=%.3f) cm (rotation ignored)"), ScaledLocalOBB.X, ScaledLocalOBB.Y, ScaledLocalOBB.Z);
}