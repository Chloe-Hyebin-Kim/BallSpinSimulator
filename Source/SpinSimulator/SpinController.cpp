#include "SpinController.h"

#include "SpinSimulator.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TopCameraActor.h"
#include "FrameCapture.h"



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
    
    ////카메라 탐색 후 뷰 설정
    // ATopCameraActor* TopCam =  Cast<ATopCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopCameraActor::StaticClass()));
    // if (TopCam)
    // {
    //    SetViewTarget(TopCam);
    // }

    /* 콘솔 명령 등록 */ 
    
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


    //CaptureView
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("CaptureView"),
        TEXT("Capture View"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnCaptureCameraView)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>CaptureView"));

    //CaptureCombinations
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("CaptureCombinations"),
        TEXT("Simulated and captured all possible cases."),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnCaptureAllCombinations)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>CaptureCombinations"));

    //CaptureCombinations
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("CaptureCSV"),
        TEXT("Simulated and captured all possible cases."),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnCaptureCSV)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>CaptureCSV"));
}

void ASpinController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SwitchSpin"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SetAxis"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("SetSpinSpeed"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ShowOriginAxis"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ShowBallAxis"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("CaptureView"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("CaptureCombinations"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("CaptureCSV"));

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
}

void ASpinController::OnShowOriginAxisCommand(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnShowOriginAxisCommand."));

    ControlledBallActor->DrawWorldGizmoAxis();
}

void ASpinController::OnShowBallAxisCommand(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnShowBallAxisCommand."));

    ControlledBallActor->DrawBallSpinAxis();

    FVector spinAxis=  ControlledBallActor->GetBallSpinAxis();
    float xRad = FMath::Acos(FVector::DotProduct(FVector::ForwardVector, spinAxis));
	float yRad = FMath::Acos(FVector::DotProduct(FVector::RightVector, spinAxis));
	float zRad = FMath::Acos(FVector::DotProduct(FVector::UpVector, spinAxis));
	
    float xDeg = FMath::RadiansToDegrees(xRad);
	float yDeg = FMath::RadiansToDegrees(yRad);
	float zDeg = FMath::RadiansToDegrees(zRad);



}

void ASpinController::OnCaptureCameraView(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnCaptureCameraView."));
    //ControlledBallActor->CaptureFrame();

   
     // AFrameCapture 자동 검색     
     AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

    ControlledBallActor->SetIsSpin(true);//무조건 스핀 끄기
    FVector vecInputSpinAxis = ControlledBallActor->GetInputSpinAxis();
    float inputRPM = ControlledBallActor->GetInputRPM();

    for (int i = 1; i <= FRAMECOUNT; ++i)
    {
        int idx = i;
        FTimerHandle timerHandle;
        FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
            {
                ControlledBallActor->RotateBallForFrameCapture(idx);
                CaptureActor->CaptureAndSave(idx, vecInputSpinAxis, inputRPM);

                // 출력 또는 저장
                UE_LOG(LogTemp, Log, TEXT("[Frame No. %d] SpinAxis = (%.6f, %.6f, %.6f)"), idx, vecInputSpinAxis.X, vecInputSpinAxis.Y, vecInputSpinAxis.Z);
            });
        GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 3.5f * i, false);
    }

}

void ASpinController::OnCaptureAllCombinations(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnCaptureAllCombinations."));

    AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

    ControlledBallActor->SetIsSpin(true);//무조건 스핀 끄기

    bool bDegree1 = true;

    int i = 1;

     if (bDegree1)//1도씩 증가
     {
      for (int pitchDeg = 1; pitchDeg <= 360; ++pitchDeg)
         {
             for (int rollDeg = 1; rollDeg <= 360; ++rollDeg)
             {
                 FTimerHandle timerHandle;
                 FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                     {
                     //Pitch 먼저 적용 -> Pitch로 회전된 Local Y축 기준 Roll 적용 -> 최종 쿼터니언 생성 -> Z축 변환
                         FQuat PitchQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(pitchDeg));//기준 축 ForwardVector ( X축 )
                         FVector LocalY = PitchQuat.RotateVector(FVector::RightVector);// Roll은 Pitch 이후의 새로운 Y축(Local Y) 기준으로 회전.
                         FQuat RollQuat = FQuat(LocalY, FMath::DegreesToRadians(rollDeg));//Roll 쿼터니언 생성
                         FQuat TotalQuat = RollQuat * PitchQuat;//Pitch 먼저 적용, 그 다음 Roll
                         ControlledBallActor->SetActorRotation(TotalQuat);//초기 스핀축 UpVector (Z축)
                         CaptureActor->CaptureCombinations(pitchDeg, rollDeg);
     
                         FVector SpinAxis = TotalQuat.RotateVector(FVector::UpVector); 
                         UE_LOG(LogTemp, Log, TEXT("[ Pitch: %d, Roll: %d ] New Spin Axis:(%.6f, %.6f, %.6f)"),
                             pitchDeg, rollDeg, SpinAxis.X, SpinAxis.Y, SpinAxis.Z);
                     });
                 GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.2f * i, false);

                 i++;

             }
         }
     } 
     else //5도씩 증가
     {
         for (int pitch = 1; pitch <= 72; ++pitch)
         {
             for (int roll = 1; roll <= 72; ++roll)
             {
                 FTimerHandle timerHandle;
                 FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                     {
                         float pitchDeg = (pitch * 5.0);
                         float rollDeg = (roll * 5.0);
                         // X축 기준 Pitch 회전
                         FQuat PitchQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(pitchDeg));
                         // Pitch 이후 회전된 공간에서의 Y축을 기준으로 Roll 회전
                         FVector LocalY = PitchQuat.RotateVector(FVector::RightVector);// Pitch 후 회전된 공간 기준 Y축을 얻음
                         FQuat RollQuat = FQuat(LocalY, FMath::DegreesToRadians(rollDeg)); // 그 Y축을 기준으로 Roll 회전

                         // 최종 쿼터니언 (누적 회전)
                         FQuat TotalQuat = RollQuat * PitchQuat;
                         ControlledBallActor->SetActorRotation(TotalQuat);
                         CaptureActor->CaptureCombinations(pitchDeg, rollDeg);
                         // 결과 회전축(Z축 → Spin Axis)
                         FVector SpinAxis = TotalQuat.RotateVector(FVector::UpVector); // Z축 방향 회전 벡터
                         UE_LOG(LogTemp, Log, TEXT("[ Pitch: %d, Roll: %d ] New Spin Axis:(%.6f, %.6f, %.6f)"),
                             (int)pitchDeg, (int)rollDeg, SpinAxis.X, SpinAxis.Y, SpinAxis.Z);
                     });
                 GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.2f * i, false);

                 i++;

             }
         }
     }


}

void ASpinController::OnCaptureCSV(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("OnCaptureCSV."));

    AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

    ControlledBallActor->SetIsSpin(true);//무조건 스핀 끄기

    int t =1;
    for (int i = 0; i < m_arrRPM.Num(); ++i)
    {
        for (int j = 1; j <= 1000; ++j)
        {
            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {
                    FVector SpinAxis = m_arrSpinAxis[i];
                    ControlledBallActor->SetSpinAxis(SpinAxis);

                    float AngleDeg = m_arrRPM[i] * RPM2DPS * DPS2FPS * j; //RPM -> DegreesPerSecond -> Degrees Per Frame
                    float AngleRad = FMath::DegreesToRadians(AngleDeg);
					FQuat rotationQuat = FQuat(SpinAxis, AngleRad);
					ControlledBallActor->AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

                    CaptureActor->CaptureAndSave_AI(j, SpinAxis, m_arrRPM[i]);
                });
            GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.1f * t, false);

            ++t;
        }
    }
   
}