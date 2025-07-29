// Copyright Epic Games, Inc. All Rights Reserved.
#include "SpinSimulatorGameModeBase.h"

#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"

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
   // ATopCameraActor* TopCam = Cast<ATopCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopCameraActor::StaticClass()));
     ATopCameraActor* TopCam = GetWorld()->SpawnActor<ATopCameraActor>(ATopCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    // ī�޶� ��ġ ����
	if (TopCam)
	{
		TopCam->SetTarget(BallActor);
		/*SpinController->SetViewTarget(TopCam);
		SpinController->SetViewTargetWithBlend`(TopCam, 0.0f);*/
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
		//SpotLight->GetLightComponent()->SetOuterConeAngle(45.0f);
		//SpotLight->GetLightComponent()->SetInnerConeAngle(25.0f);
        //SpotLight->GetLightComponent()->SetAttenuationRadius(2000.f);  // ��� �л�

    }


    // ����Ʈ ���μ���: NIR �䳻
    //APostProcessVolume* PPVolume = GetWorld()->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator);
    //if (PPVolume)
    //{
    //    PPVolume->bUnbound = true;

    //    //���(IR �ùķ��̼�. �����ܼ� ����)
    //    PPVolume->Settings.bOverride_ColorSaturation = true;
    //    PPVolume->Settings.ColorSaturation = FVector4(0.f, 0.f, 0.f, 0.f); // RGB = 0 �� ���

    //    //�� ���� ����
    //    PPVolume->Settings.bOverride_ToneCurveAmount = true;
    //    PPVolume->Settings.ToneCurveAmount = 0.f;


    //    // ���� ���� ����
    //    PPVolume->Settings.bOverride_AutoExposureMethod = true;
    //    PPVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
    //    PPVolume->Settings.bOverride_AutoExposureBias = true;
    //    PPVolume->Settings.AutoExposureBias = -1.0f;

    //    // Bloom ����
    //    PPVolume->Settings.bOverride_BloomIntensity = true;
    //    PPVolume->Settings.BloomIntensity = 0.f;
    //}

}