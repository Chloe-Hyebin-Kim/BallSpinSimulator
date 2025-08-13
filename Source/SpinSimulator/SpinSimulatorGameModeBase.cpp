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



    TArray<FString> FileLines;

    // 파일 경로 지정
    FString FilePath = FPaths::ProjectSavedDir() + TEXT("/SpinDataCSV/range_for_synthetic_data.csv");
    if (FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
    {
        for (int32 i = 1; i < FileLines.Num(); ++i) // 0번째 줄은 헤더
        {
            TArray<FString> Columns;
            FileLines[i].ParseIntoArray(Columns, TEXT(","), true);

            FString rpm = Columns[0];
            float f32InputRPM = FCString::Atoi(*Columns[0]);

            FVector SpinAxisAsVec = FVector::UpVector;
            SpinAxisAsVec.X = FCString::Atoi(*Columns[1]);
            SpinAxisAsVec.Y = FCString::Atof(*Columns[2]);
            SpinAxisAsVec.Z = FCString::Atof(*Columns[3]);

            SpinController->m_arrRPM.Push(f32InputRPM);
            SpinController->m_arrSpinAxis.Push(SpinAxisAsVec);

            UE_LOG(LogTemp, Log, TEXT("RPM:%s , SpinAxis:%f,%f,%f"), *rpm, SpinAxisAsVec.X, SpinAxisAsVec.Y, SpinAxisAsVec.Z);
        }
    }

    //APostProcessVolume* PPVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    //if (PPVolume)
    //{
    //    PPVolume->bUnbound = true; // 월드 전역에 적용됨

    //    // 자동 노출 제거
    //    PPVolume->Settings.bOverride_AutoExposureMethod = true;
    //    PPVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;

    //    PPVolume->Settings.bOverride_AutoExposureBias = true;
    //    PPVolume->Settings.AutoExposureBias = 1.5f; // 1.0~2.0 사이에서 적절히 조정

    //    UE_LOG(LogTemp, Log, TEXT("Global PostProcessVolume created with Manual Exposure."));
    //}
}