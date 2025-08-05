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

    for (int i = 1; i <= FRAMECOUNT; ++i)
    {
        int idx = i;
        FTimerHandle timerHandle;
        FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
            {
                ControlledBallActor->RotateBallForFrameCapture(idx);
                CaptureActor->CaptureAndSave(idx, vecInputSpinAxis);

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

    
   
    float rotateRad = FMath::DegreesToRadians(1.f);
    FQuat pitchQuat = FQuat(FVector(1.f, 0.f, 0.f), rotateRad);
    FQuat rollQuat = FQuat(FVector(0.f, 1.f, 0.f), rotateRad);
   
   /*
    TArray<FVector> SpinAxes;
    SpinAxes.Empty();
    for (int32 PitchDeg = 0; PitchDeg < 360; ++PitchDeg)
    {
        // Pitch를 적용한 회전 (X축 기준)
        FRotator PitchRotator(PitchDeg, 0, 0);
        FQuat PitchQuat = PitchRotator.Quaternion();

        // Pitch에 의해 새로 정의된 X축 방향
        FVector NewX = PitchQuat.RotateVector(FVector(1, 0, 0));

        for (int32 RollDeg = 0; RollDeg < 360; ++RollDeg)
        {
            // 새 X축(NewX)을 중심으로 RollDeg도 회전
            FQuat RollQuat = FQuat(NewX.GetSafeNormal(), FMath::DegreesToRadians(RollDeg));

            // 전체 회전 쿼터니언 = Pitch 먼저, Roll 나중 (RollQuat * PitchQuat)
            FQuat TotalQuat = RollQuat * PitchQuat;

            // 이 회전의 회전축 저장
            FVector SpinAxis = TotalQuat.GetRotationAxis(); // Normalize 되어 있음
            SpinAxes.Add(SpinAxis);

            // 로그 출력
            UE_LOG(LogTemp, Log, TEXT("Pitch: %d, Roll: %d → SpinAxis: %s"), PitchDeg, RollDeg, *SpinAxis.ToString());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("총 Spin Axis 수: %d"), SpinAxes.Num());


    for (int32 idx = 0; idx < SpinAxes.Num(); ++idx)
    {
        FTimerHandle timerHandle;
        FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
            {
                FVector newSpinAxis = SpinAxes[idx];
                ControlledBallActor->RotateSpinAxis(newSpinAxis);
                FVector vecInputSpinAxis = ControlledBallActor->GetInputSpinAxis();
                CaptureActor->CaptureAndSave(idx, newSpinAxis);

                // 출력 또는 저장
                UE_LOG(LogTemp, Log, TEXT("[Frame No. %d] SpinAxis = (%.6f, %.6f, %.6f)"), idx, newSpinAxis.X, newSpinAxis.Y, newSpinAxis.Z);
            });
        GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 3.5f * ids, false);

    }
    */
   
    
  




    //int i= 0;
    //for (int pitchDeg = 0; pitchDeg < 360; ++pitchDeg) // elevation (pitch)
    //{
    //    for (int rollDeg = 0; rollDeg < 360; ++rollDeg)    // azimuth (yaw)
    //    { 
    //        FTimerHandle timerHandle;
    //        FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
    //            {
    //            
    //                // 출력
    //                UE_LOG(LogTemp, Log, TEXT("[ θ = %3d, φ = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"), pitchDeg, rollDeg, normSpinAxis.X, normSpinAxis.Y, normSpinAxis.Z);
    //                //CaptureActor->CaptureCombinations(thetaDeg, phiDeg, newSpinAxis);

    //            });

    //        GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.01f * i, false);
    //        ++i;
    //    }
    //}

    /*
      int  i=0;
    for (int thetaDeg = 0; thetaDeg < 180; ++thetaDeg) // elevation (pitch)
    {
        for (int phiDeg = 0; phiDeg < 360; ++phiDeg)    // azimuth (yaw)
        {
            //////////////////////////////////////////////////
            //float angleXRad = FMath::DegreesToRadians(pitchDeg);
            //float angleYRad = FMath::DegreesToRadians(yawDeg);
            //float newX = FMath::Sin(angleYRad);
            //float newY = -FMath::Sin(angleXRad) * FMath::Cos(angleYRad);
            //float newZ = FMath::Cos(angleXRad) * FMath::Cos(angleYRad);
            //FVector newSpinAxis = FVector(newX, newY, newZ);
            //ControlledBallActor->AllCombinationsOfRotateAxis(newSpinAxis);
            //CaptureActor->CaptureCombinations(pitchDeg, yawDeg, newSpinAxis);
            //// 출력 또는 저장
            //UE_LOG(LogTemp, Log, TEXT("[ Pitch = %3d, Yaw = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"), pitchDeg, yawDeg, newSpinAxis.X, newSpinAxis.Y, newSpinAxis.Z);

            //////////////////////////////////////////////////


            float thetaRad = FMath::DegreesToRadians(thetaDeg);
            float phiRad = FMath::DegreesToRadians(phiDeg);

            FVector newSpinAxis;
            newSpinAxis.X = FMath::Sin(thetaRad) * FMath::Cos(phiRad);
            newSpinAxis.Y = FMath::Sin(thetaRad) * FMath::Sin(phiRad);
            newSpinAxis.Z = FMath::Cos(thetaRad);

            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {
                    // 초기화
                    ControlledBallActor->SetActorRotation(FRotator::ZeroRotator);

                    // 회전축 정렬
                    FVector normSpinAxis = newSpinAxis.GetSafeNormal();
                    FRotator newRotator = FRotationMatrix::MakeFromZ(normSpinAxis).Rotator();
                    ControlledBallActor->SetActorRotation(newRotator);


                    // 출력
                    CaptureActor->CaptureCombinations(thetaDeg, phiDeg, newSpinAxis);

                    UE_LOG(LogTemp, Log, TEXT("[ Pitch = %3d, Roll = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"),
                        thetaDeg, phiDeg, newSpinAxis.X, newSpinAxis.Y, newSpinAxis.Z);
                });

            GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.01f * i, false);
            ++i;
        }
    }
     */

   


     //이거 짐벌락생김  //OLD
   
     int i = 1;
    for (int pitchDeg = 0; pitchDeg < 360; ++pitchDeg)
    {
        for (int rollDeg = 0; rollDeg < 360; ++rollDeg)
        {
            //////////////////////////////////////////////////
            //float angleXRad = FMath::DegreesToRadians(pitchDeg);
            //float angleYRad = FMath::DegreesToRadians(yawDeg);
            //float newX = FMath::Sin(angleYRad);
            //float newY = -FMath::Sin(angleXRad) * FMath::Cos(angleYRad);
            //float newZ = FMath::Cos(angleXRad) * FMath::Cos(angleYRad);
            //FVector newSpinAxis = FVector(newX, newY, newZ);
            //ControlledBallActor->AllCombinationsOfRotateAxis(newSpinAxis);
            //CaptureActor->CaptureCombinations(pitchDeg, yawDeg, newSpinAxis);
            //// 출력 또는 저장
            //UE_LOG(LogTemp, Log, TEXT("[ Pitch = %3d, Yaw = %3d ]  SpinAxis = (%.6f, %.6f, %.6f)"), pitchDeg, yawDeg, newSpinAxis.X, newSpinAxis.Y, newSpinAxis.Z);

            //////////////////////////////////////////////////


            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {

                    // X축 기준 Pitch 회전
                    FQuat PitchQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians((float)pitchDeg));
                    // Pitch 이후 회전된 공간에서의 Y축을 기준으로 Roll 회전
                    FVector LocalY = PitchQuat.RotateVector(FVector::RightVector);// Pitch 후 회전된 공간 기준 Y축을 얻음
                    FQuat RollQuat = FQuat(LocalY, FMath::DegreesToRadians((float)rollDeg)); // 그 Y축을 기준으로 Roll 회전

                    // 최종 쿼터니언 (누적 회전)
                    FQuat TotalQuat = RollQuat * PitchQuat;
                    ControlledBallActor->SetActorRotation(TotalQuat);
                    CaptureActor->CaptureCombinations(pitchDeg, rollDeg);
                    // 결과 회전축(Z축 → Spin Axis)
                    FVector SpinAxis = TotalQuat.RotateVector(FVector::UpVector); // Z축 방향 회전 벡터
                    UE_LOG(LogTemp, Log, TEXT("[ Pitch: %d, Roll: %d ] New Spin Axis:(%.6f, %.6f, %.6f)"),
                        pitchDeg, rollDeg, SpinAxis.X, SpinAxis.Y, SpinAxis.Z);
                });
            GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.2f * i, false);

            i++;

        }
    }
    


}
