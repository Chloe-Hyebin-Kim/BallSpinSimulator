#include "SpinController.h"

#include "SpinSimulator.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TopCameraActor.h"



ASpinController::ASpinController()
{
    PrimaryActorTick.bCanEverTick = false;
   // AutoPossessPlayer = EAutoReceiveInput::Player0; // 키 입력 수신


}

void ASpinController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT(">> ASpinController BeginPlay() "));


    // 골프공 자동 검색     
     if (!ControlledBallActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("There is no BallMesh Actor. Try to find BallMesh Actor."));

         ControlledBallActor = Cast<AGolfBall>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolfBall::StaticClass()));
		if (!ControlledBallActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Can not find BallMesh Actor."));
            return;
        }
    }
    //
    ////카메라 탐색 후 뷰 설정
    // ATopCameraActor* TopCam =  Cast<ATopCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopCameraActor::StaticClass()));
    // if (TopCam)
    // {
    //    SetViewTarget(TopCam);
    // }

    // 콘솔 명령 등록
    
    // 공 회전 on/off
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SwitchSpin"),
        TEXT("Switch Ball Spin"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSwitchBallSpinCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SwitchSpin"));


    //축 변환
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SetAxis"),
        TEXT("Sets the spin axis of the golf ball. [Usag: SetAxis X Y Z]"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSetAxisCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SetAxis"));

    //축 변환
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SetSpinSpeed"),
        TEXT("Sets the spin speed of the golf ball. unit is RPM. [Usag: SpinSpeed N(RPM)]"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSetSpinSpeedCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SetSpinSpeed"));

    // 월드 절대좌표 축
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("ShowOriginAxis"),
        TEXT("Show Original Absolute Ball Axis"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnShowOriginAxisCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>ShowOriginAxis"));

    // 변경된 공 회전 축
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("ShowBallAxis"),
        TEXT("Show Ball Spin Axis"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnShowBallAxisCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>ShowBallAxis"));
}

void ASpinController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SwitchSpin"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SetAxis"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SetSpinSpeed"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ShowOriginAxis"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ShowBallAxis"));

    Super::EndPlay(EndPlayReason);

}


void ASpinController::OnSwitchBallSpinCommand(const TArray<FString>& Args)
{
    if (!ControlledBallActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
    }
    ControlledBallActor->SetIsSpin();
}

void ASpinController::OnSetSpinSpeedCommand(const TArray<FString>& Args)
{
    if (Args.Num() != 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid number of arguments. ex)SetSpinSpeed 4000"));
        return;
    }

    if (!ControlledBallActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
        return;
    }

    float f32InputRPM = FCString::Atof(*Args[0]);
    ControlledBallActor->SetSpinSpeed(f32InputRPM);
    AGolfBall* GolfBallActor = Cast<AGolfBall>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolfBall::StaticClass()));
	
    /*float f32InputRPM = FCString::Atof(*Args[0]);
    ControlledBallActor->SetIsSpin(true);
	 FTimerHandle timerHandle;
	 FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([&]()
		 {
			 ControlledBallActor->SetSpinSpeed(f32InputRPM);
             ControlledBallActor->SetIsSpin();
			 GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		 });
	 GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 4.f, false);*/
     
}

void ASpinController::OnSetAxisCommand(const TArray<FString>& Args)
{
    if (Args.Num() != 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid number of arguments."));
        return;
    }

    if (!ControlledBallActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
        return;
    }

    FVector InputAxis(FCString::Atof(*Args[0]), FCString::Atof(*Args[1]), FCString::Atof(*Args[2]));
    ControlledBallActor->SetSpinAxis(InputAxis);

	/*
    FVector InputAxis(FCString::Atof(*Args[0]), FCString::Atof(*Args[1]), FCString::Atof(*Args[2]));
    ControlledBallActor->SetIsSpin(true);
    FTimerHandle timerHandle;
	FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([&]()
		{
			ControlledBallActor->SetSpinAxis(InputAxis);
            ControlledBallActor->SetIsSpin();
			GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		});
	GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 4.f, false);
    */

}

void ASpinController::OnShowOriginAxisCommand(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnShowOriginAxisCommand."));

    ControlledBallActor->DrawWorldGizmoAxis();
    /*FVector vecAxisX = BALL_LOCATION + 	FVector::XAxisVector * ARROWLENGTH;
    FVector vecAxisY = BALL_LOCATION + FVector::YAxisVector * ARROWLENGTH;
    FVector vecAxisZ = BALL_LOCATION + FVector::ZAxisVector* ARROWLENGTH;

	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisX, ARROWSIZE, XCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisY, ARROWSIZE, YCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);
	DrawDebugDirectionalArrow(GetWorld(), BALL_LOCATION, vecAxisZ, ARROWSIZE, ZCOLOR, false, ARROWLIFE, ARROWDEPTH, ARROWTHICK);*/
}

void ASpinController::OnShowBallAxisCommand(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnShowBallAxisCommand."));

    ControlledBallActor->DrawBallSpinAxis();
    //FVector vecBallSpinAxis = ControlledBallActor->GetBallSpinAxis();
    //FRotator rotBallSpinAxis = ControlledBallActor->GetBallRotator();
    
}
