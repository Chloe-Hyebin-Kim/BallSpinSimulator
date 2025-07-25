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
   
   // ���� ��� ����  ( ���� ����)
    DefaultPawnClass = nullptr; // DefaultPawn ���� �� �ϵ��� ����
    //PlayerControllerClass = nullptr; //�����!
    //HUDClass = nullptr;
    //GameStateClass = nullptr;
    //SpectatorClass = nullptr;

    m_vecSpinAxis = FVector::UpVector;//(���� Z��==0,0,1)
    m_rotSpinAxis = FRotator::ZeroRotator;
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
    ATopCameraActor* TopCamera = GetWorld()->SpawnActor<ATopCameraActor>(ATopCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    // ī�޶� ��ġ ����
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
    //    Spot->SetIntensity(20000.f);           // ���� ���� 29V�� ��ప���� ����
    //    Spot->SetLightColor(FLinearColor(0.8f, 0.2f, 0.2f)); // �����ܼ� ���� ������
    //    Spot->SetAttenuationRadius(1000.f);
    //    Spot->SetInnerConeAngle(20.f);
    //    Spot->SetOuterConeAngle(40.f);
    //}







}