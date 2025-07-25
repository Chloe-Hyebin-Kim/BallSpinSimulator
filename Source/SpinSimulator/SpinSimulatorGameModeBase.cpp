// Copyright Epic Games, Inc. All Rights Reserved.
#include "SpinSimulatorGameModeBase.h"

#include "Engine/World.h"
#include "Engine/SpotLight.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "SpinSimulator.h"
#include "TopCameraActor.h"
#include "SpinController.h"

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

    m_vecSpinAxis = FVector::UpVector;//(로컬 Z축==0,0,1)
    m_rotSpinAxis = FRotator::ZeroRotator;
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
    ATopCameraActor* TopCamera = GetWorld()->SpawnActor<ATopCameraActor>(ATopCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // 카메라 위치 지정
    if (TopCamera)
    {
        TopCamera->SetTarget(BallActor);

        FViewTargetTransitionParams ViewParams;
        SpinController->SetViewTarget(TopCamera, ViewParams);
    }

    //ASpotLight* SpotLight = Cast<ASpotLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpotLight::StaticClass()));
    //if (SpotLight)
    //{
    //    USpotLightComponent* Spot = CreateDefaultSubobject<USpotLightComponent>(TEXT("NIRLight"));
    //    Spot->SetIntensity(20000.f);           // 조명 전압 29V를 루멘값으로 추정
    //    Spot->SetLightColor(FLinearColor(0.8f, 0.2f, 0.2f)); // 근적외선 유사 붉은톤
    //    Spot->SetAttenuationRadius(1000.f);
    //    Spot->SetInnerConeAngle(20.f);
    //    Spot->SetOuterConeAngle(40.f);
    //}







}