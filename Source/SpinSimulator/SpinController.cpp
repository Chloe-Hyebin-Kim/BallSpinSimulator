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
   // AutoPossessPlayer = EAutoReceiveInput::Player0; // Ű �Է� ����


}

void ASpinController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT(">> ASpinController BeginPlay() "));


    // ������ �ڵ� �˻�     
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
    
    ////ī�޶� Ž�� �� �� ����
    // ATopCameraActor* TopCam =  Cast<ATopCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopCameraActor::StaticClass()));
    // if (TopCam)
    // {
    //    SetViewTarget(TopCam);
    // }

    /* �ܼ� ��� ��� */ 
    
    // �� ȸ�� on/off
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SwitchSpin"),
        TEXT("Switch Ball Spin"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSwitchBallSpinCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SwitchSpin"));


    //�� ��ȯ
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SetAxis"),
        TEXT("Sets the spin axis of the golf ball. [Usag: SetAxis X Y Z]"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSetAxisCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SetAxis"));

    //�� ��ȯ
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SetSpinSpeed"),
        TEXT("Sets the spin speed of the golf ball. unit is RPM. [Usag: SpinSpeed N(RPM)]"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnSetSpinSpeedCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>SetSpinSpeed"));

    // ���� ������ǥ ��
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("ShowOriginAxis"),
        TEXT("Show Original Absolute Ball Axis"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnShowOriginAxisCommand)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>ShowOriginAxis"));

    // ����� �� ȸ�� ��
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

   
     // AFrameCapture �ڵ� �˻�     
     AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

    ControlledBallActor->SetIsSpin(true);//������ ���� ����
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

                // ��� �Ǵ� ����
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

    ControlledBallActor->SetIsSpin(true);//������ ���� ����

    bool bDegree1 = true;

    int i = 1;

     if (bDegree1)//1���� ����
     {
      for (int pitchDeg = 1; pitchDeg <= 360; ++pitchDeg)
         {
             for (int rollDeg = 1; rollDeg <= 360; ++rollDeg)
             {
                 FTimerHandle timerHandle;
                 FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                     {
                     //Pitch ���� ���� -> Pitch�� ȸ���� Local Y�� ���� Roll ���� -> ���� ���ʹϾ� ���� -> Z�� ��ȯ
                         FQuat PitchQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(pitchDeg));//���� �� ForwardVector ( X�� )
                         FVector LocalY = PitchQuat.RotateVector(FVector::RightVector);// Roll�� Pitch ������ ���ο� Y��(Local Y) �������� ȸ��.
                         FQuat RollQuat = FQuat(LocalY, FMath::DegreesToRadians(rollDeg));//Roll ���ʹϾ� ����
                         FQuat TotalQuat = RollQuat * PitchQuat;//Pitch ���� ����, �� ���� Roll
                         ControlledBallActor->SetActorRotation(TotalQuat);//�ʱ� ������ UpVector (Z��)
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
     else //5���� ����
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
                         // X�� ���� Pitch ȸ��
                         FQuat PitchQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(pitchDeg));
                         // Pitch ���� ȸ���� ���������� Y���� �������� Roll ȸ��
                         FVector LocalY = PitchQuat.RotateVector(FVector::RightVector);// Pitch �� ȸ���� ���� ���� Y���� ����
                         FQuat RollQuat = FQuat(LocalY, FMath::DegreesToRadians(rollDeg)); // �� Y���� �������� Roll ȸ��

                         // ���� ���ʹϾ� (���� ȸ��)
                         FQuat TotalQuat = RollQuat * PitchQuat;
                         ControlledBallActor->SetActorRotation(TotalQuat);
                         CaptureActor->CaptureCombinations(pitchDeg, rollDeg);
                         // ��� ȸ����(Z�� �� Spin Axis)
                         FVector SpinAxis = TotalQuat.RotateVector(FVector::UpVector); // Z�� ���� ȸ�� ����
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

    ControlledBallActor->SetIsSpin(true);//������ ���� ����

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