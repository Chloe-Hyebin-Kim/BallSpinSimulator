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



    TArray<FString> FileLines;

    // ���� ��� ����
    FString FilePath = FPaths::ProjectSavedDir() + TEXT("/SpinDataCSV/range_for_synthetic_data.csv");
    if (FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
    {
        for (int32 i = 1; i < FileLines.Num(); ++i) // 0��° ���� ���
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
    //    PPVolume->bUnbound = true; // ���� ������ �����

    //    // �ڵ� ���� ����
    //    PPVolume->Settings.bOverride_AutoExposureMethod = true;
    //    PPVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;

    //    PPVolume->Settings.bOverride_AutoExposureBias = true;
    //    PPVolume->Settings.AutoExposureBias = 1.5f; // 1.0~2.0 ���̿��� ������ ����

    //    UE_LOG(LogTemp, Log, TEXT("Global PostProcessVolume created with Manual Exposure."));
    //}
}