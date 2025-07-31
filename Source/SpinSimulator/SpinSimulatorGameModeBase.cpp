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





    // 포스트 프로세스: NIR 흉내
    //APostProcessVolume* PPVolume = GetWorld()->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator);
    //if (PPVolume)
    //{
    //    PPVolume->bUnbound = true;

    //    //흑백(IR 시뮬레이션. 근적외선 느낌)
    //    PPVolume->Settings.bOverride_ColorSaturation = true;
    //    PPVolume->Settings.ColorSaturation = FVector4(0.f, 0.f, 0.f, 0.f); // RGB = 0 → 흑백

    //    //톤 매핑 제거
    //    PPVolume->Settings.bOverride_ToneCurveAmount = true;
    //    PPVolume->Settings.ToneCurveAmount = 0.f;


    //    // 노출 수동 설정
    //    PPVolume->Settings.bOverride_AutoExposureMethod = true;
    //    PPVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
    //    PPVolume->Settings.bOverride_AutoExposureBias = true;
    //    PPVolume->Settings.AutoExposureBias = -1.0f;

    //    // Bloom 제거
    //    PPVolume->Settings.bOverride_BloomIntensity = true;
    //    PPVolume->Settings.BloomIntensity = 0.f;
    //}

}