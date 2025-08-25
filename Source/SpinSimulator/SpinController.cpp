#include "SpinController.h"

#include "SpinSimulator.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TopCameraActor.h"
#include "FrameCapture.h"

#include <iostream>
#include <fstream>


ASpinController::ASpinController()
{
    PrimaryActorTick.bCanEverTick = false;
   // AutoPossessPlayer = EAutoReceiveInput::Player0; // Ű �Է� ����

    bReadRangeforSyntheticData = false;
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
    RegisterCMD();
    
    /* �޽� ��ĵ */
    ScanBallMeshVertexData();

}

void ASpinController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterCMD();

    Super::EndPlay(EndPlayReason);
}

void ASpinController::RegisterCMD() 
{
    // �� ȸ�� on/off
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("SwitchSpin"),
        TEXT("Switch Ball Spin. 0 == false, 1 == true. [Usag: SwitchSpin 0/1]  "),
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

    //CheckVertexPosition
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("CheckVertexPosition"),
        TEXT("Count all number of vertices"),
        FConsoleCommandWithArgsDelegate::CreateUObject(this, &ASpinController::OnCheckVertexPosition)
    );
    UE_LOG(LogTemp, Log, TEXT("RegisterConsoleCommand.  >>>CheckVertexPosition"));
}

void ASpinController::UnregisterCMD()
{
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand SwitchSpin"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand SetAxis"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand SetSpinSpeed"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand ShowOriginAxis"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand ShowBallAxis"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand CaptureView"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand CaptureCombinations"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand CaptureCSV"));
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("UnregisterConsoleCommand CheckVertexPosition"));
}

void ASpinController::ScanBallMeshVertexData()
{
    UE_LOG(LogTemp, Log, TEXT("Scan Ball Mesh Vertex Data"));

    FString filePath = FPaths::ProjectSavedDir() + TEXT("/SpinDataCSV/VertexInfo.csv");
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*filePath))
    {
        UE_LOG(LogTemp, Log, TEXT("VertexInfo.csv already exists."));

        TArray<FString> fileLines;

        // ���� ��� ����
        if (FFileHelper::LoadFileToStringArray(fileLines, *filePath))
        {
            for (int32 i = 1; i < fileLines.Num(); ++i) // 0��° ���� ���
            {

                //const FString header = TEXT("CircleId,LocalPosX,LocalPosY,LocalPosZ,WorldPosX,WorldPosY,WorldPosZ");
                TArray<FString> Columns;
                fileLines[i].ParseIntoArray(Columns, TEXT(","), true);

                FSpinDOE tmpDot = FSpinDOE();
                tmpDot.CircleId = FName(*Columns[0]);
                tmpDot.LocalPos.X = FCString::Atof(*Columns[1]);
                tmpDot.LocalPos.Y = FCString::Atof(*Columns[2]);
                tmpDot.LocalPos.Z = FCString::Atof(*Columns[3]);
                tmpDot.WorldPos.X = FCString::Atof(*Columns[4]);
                tmpDot.WorldPos.Y = FCString::Atof(*Columns[5]);
                tmpDot.WorldPos.Z = FCString::Atof(*Columns[6]);

                ControlledBallActor->AddVertexInfo(tmpDot);

                UE_LOG(LogTemp, Log, TEXT("[Draw Vertex %d] Local: %s, World: %s"), i, *tmpDot.LocalPos.ToString(), *tmpDot.WorldPos.ToString());
            }
        }

    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("need to create /SpinDataCSV/VertexInfo.csv."));


        ControlledBallActor->CheckVertexPosition();

        TArray<FSpinDOE> dots = ControlledBallActor->GetArrayDots();
        FString CSVContent;
        const FString header = TEXT("CircleId,LocalPosX,LocalPosY,LocalPosZ,WorldPosX,WorldPosY,WorldPosZ"); // ��� 1ȸ ���
        //FFileHelper::SaveStringToFile(header + LINE_TERMINATOR, *filePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
        CSVContent += header + LINE_TERMINATOR;


        //TArray<TArray<FString>> fileLines;//    TArray<FString> columns �� ������ ����
        for (int32 i = 0; i < dots.Num(); ++i)
        {
            FSpinDOE tmpDot = dots[i];

            //TArray<FString> Columns;
            //Columns.Add(tmpDot.CircleId.ToString());//Columns.Add(FString::Printf(TEXT("%s"), tmpDot.CircleId));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.LocalPos.X));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.LocalPos.Y));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.LocalPos.Z));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.WorldPos.X));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.WorldPos.Y));
            //Columns.Add(FString::Printf(TEXT("%f"), tmpDot.WorldPos.Z));
            //fileLines.Add(Columns);

            FString line = "";
            line = FString::Printf(TEXT("%s,%f,%f,%f,%f,%f,%f"), *tmpDot.CircleId.ToString(), tmpDot.LocalPos.X, tmpDot.LocalPos.Y, tmpDot.LocalPos.Z, tmpDot.WorldPos.X, tmpDot.WorldPos.Y, tmpDot.WorldPos.Z);
            //FFileHelper::SaveStringToFile(line + LINE_TERMINATOR, *filePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
            CSVContent += line + LINE_TERMINATOR;
            UE_LOG(LogTemp, Log, TEXT("%s"), *line);
        }


        if (FFileHelper::SaveStringToFile(CSVContent, *filePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Save Success: %s"), *filePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Save Fail: %s"), *filePath);
        }

    }
}

void ASpinController::ReadRangeforSyntheticData()
{
    if (bReadRangeforSyntheticData)
    {
        UE_LOG(LogTemp, Log, TEXT("ReadRangeforSyntheticData."));

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
                float f32InputRPM = FCString::Atof(*Columns[0]);

                FVector SpinAxisAsVec = FVector::UpVector;
                SpinAxisAsVec.X = FCString::Atof(*Columns[1]);
                SpinAxisAsVec.Y = FCString::Atof(*Columns[2]);
                SpinAxisAsVec.Z = FCString::Atof(*Columns[3]);

                m_arrRPM.Add(f32InputRPM);
                m_arrSpinAxis.Add(SpinAxisAsVec);

                UE_LOG(LogTemp, Log, TEXT("RPM:%s , SpinAxis:%f,%f,%f"), *rpm, SpinAxisAsVec.X, SpinAxisAsVec.Y, SpinAxisAsVec.Z);
            }
        }

    }
    else
	{
		UE_LOG(LogTemp, Log, TEXT("Do not ReadRangeforSyntheticData."));
	}
}


void ASpinController::OnSwitchBallSpinCommand(const TArray<FString>& Args)
{
    UE_LOG(LogTemp, Log, TEXT("SwitchBallSpin."));
    
    if (Args.Num() != 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid number of arguments. ex)SwitchSpin 1"));
        return;
    }
    
    if (!ControlledBallActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BallMesh Actor is not assigned."));
    }

    int bSpin = FCString::Atoi(*Args[0]);
    ControlledBallActor->SetIsSpin((bool)bSpin,false);
    // ��� �Ǵ� ����
    FString rst = (bSpin==1)? "ON":"OFF";
    UE_LOG(LogTemp, Log, TEXT("Spin turn %s."), *rst);
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
    UE_LOG(LogTemp, Log, TEXT("xDeg: %.6f, yDeg: %.6f, zDeg: %.6f"), xDeg, yDeg, zDeg);
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

    ControlledBallActor->SetIsSpin(false,true);//������ ���� ����
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

    ControlledBallActor->SetIsSpin(false,true);//������ ���� ����

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

    if(bReadRangeforSyntheticData==false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please set bReadRangeforSyntheticData=true in SpinController.cpp and re-run."));
		return;
	}

	// AFrameCapture �ڵ� �˻�
    AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

     ControlledBallActor->SetIsSpin(false,true);//������ ���� ����

    int t = 1;
	for (int i = 0; i < m_arrRPM.Num(); ++i)
    {
        for (int j = 1; j <= 1000; ++j)
        {
            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {
                    FVector SpinAxis = m_arrSpinAxis[i].GetSafeNormal();
                    ControlledBallActor->SetSpinAxis(SpinAxis);
                    float rpm = m_arrRPM[i];
                    float AngleDeg = rpm * RPM2DPS * DPS2FPS * j; //RPM -> DegreesPerSecond -> Degrees Per Frame
                    float AngleRad = FMath::DegreesToRadians(AngleDeg);
                    FQuat rotationQuat = FQuat(SpinAxis, AngleRad);
                    ControlledBallActor->AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);

                    CaptureActor->CaptureAndSave_CSV(j, SpinAxis, rpm);
                });
            GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.2f * t, false);

            ++t;
        }
    }


   /*
    
      int t = 1;
    for (int i = 0; i <m_arrRPM.Num(); ++i)
    {
        for (int j = 1; j <= 1000; ++j)
        { 
            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {
					const FVector spinAxis = m_arrSpinAxis[i];
					const FVector AxisN = spinAxis.GetSafeNormal();
                    ControlledBallActor->SetSpinAxis(spinAxis);

                    const float rpm = m_arrRPM[i];
                    const float degPerFrame = (rpm * 0.003);// rpm* RPM2DPS* DPS2FPS* j;   //(RPM -> DegreesPerSecond -> Degrees Per Frame)
                    float radPerFrame = FMath::DegreesToRadians(degPerFrame);
                    const FQuat dQuat(AxisN, radPerFrame); // ���� ȸ��
                    ControlledBallActor->AddActorWorldRotation(dQuat, false, nullptr, ETeleportType::None);
                    CaptureActor->CaptureAndSave_CSV(j, spinAxis, rpm);
                    UE_LOG(LogTemp, Log, TEXT("[j=%d]  %.4f(deg)"), j, degPerFrame );
        });
        GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.1f * t, false);

        ++t;
        }
        
    }
    
    */
}


void ASpinController::OnCheckVertexPosition(const TArray<FString>& Args)
{
    ControlledBallActor->SetIsSpin(false,true);//������ ���� ����
    //ControlledBallActor->CheckVertexPosition();

    

    UE_LOG(LogTemp, Log, TEXT("DrawUsedVertices."));
    ControlledBallActor->DrawUsedVertices();
}

void ASpinController::VirtualSpinCapture()
{
    UE_LOG(LogTemp, Log, TEXT("VirtualSpinCapture."));

    AFrameCapture* CaptureActor = Cast<AFrameCapture>(UGameplayStatics::GetActorOfClass(GetWorld(), AFrameCapture::StaticClass()));
    if (!CaptureActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find CaptureActor."));
    }

    ControlledBallActor->SetIsSpin(false,true);//������ ���� ����

    int t = 1;
    for (int i = 0; i < 1; ++i)
    {
        for (int j = 1; j <= 1000; ++j)
        {
            FTimerHandle timerHandle;
            FTimerDelegate delayCommandDelegate = FTimerDelegate::CreateLambda([=]()
                {
                    const FVector spinAxis = m_arrSpinAxis[i];
                    ControlledBallActor->SetSpinAxis(spinAxis);
                    const FVector AxisN = spinAxis.GetSafeNormal();
                    const float rpm = m_arrRPM[i];
                    const float degPerFrame = (rpm * 0.003) * j;// rpm* RPM2DPS* DPS2FPS* j;   //(RPM -> DegreesPerSecond -> Degrees Per Frame)
                    float radPerFrame = FMath::DegreesToRadians(degPerFrame);
                    const FQuat dQuat(AxisN, radPerFrame); // ���� ȸ��
                    ControlledBallActor->AddActorWorldRotation(dQuat, false, nullptr, ETeleportType::None);
                    // ĸó (�ʿ�� ���� �÷��� ������ ����ȭ)
                    CaptureActor->CaptureAndSave_CSV(j, spinAxis, rpm);
                    UE_LOG(LogTemp, Log, TEXT("[j=%d]  %.4f(deg)"), j, degPerFrame);
                });
            GetWorld()->GetTimerManager().SetTimer(timerHandle, delayCommandDelegate, 0.1f * t, false);

            ++t;
        }

    }
   
}

bool ASpinController::LoadCSV(const TCHAR* FilePathName, TArray<FString>& OutPutStrings)
{
    bool bSuccess = FFileHelper::LoadANSITextFileToStrings(*(FPaths::ProjectDir() + "/Content/" + FilePathName + ".csv"), NULL, OutPutStrings);

    return bSuccess;
}


bool ASpinController::StartLogFile(const FString& Prefix)
{
    FDateTime CurrentDateTime = FDateTime::Now();

    int32 Year, Month, Day;
    CurrentDateTime.GetDate(Year, Month, Day);
    int32 Hour = CurrentDateTime.GetHour();
    int32 Minute = CurrentDateTime.GetMinute();

    FString GameSavedPath = FPaths::ProjectSavedDir();
    FString FileName = FString::Printf(TEXT("%s/Logs/%s_%02d-%02d-%02d_%02d-%02d.log"), *GameSavedPath, *Prefix, Year % 100, Month, Day, Hour, Minute);
    ArchiveFile = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*FileName, FILEWRITE_EvenIfReadOnly | FILEWRITE_AllowRead));
    if (ArchiveFile)
    {
        ArchiveFile->SetIsTextFormat(true);
        UTF8CHAR UTF8BOM[] = { 0xEF, 0xBB, 0xBF };
        ArchiveFile->Serialize(&UTF8BOM, UE_ARRAY_COUNT(UTF8BOM) * sizeof(UTF8CHAR));
    }

    return ArchiveFile != nullptr;
}

void ASpinController::CloseLogFile()
{
    if (ArchiveFile)
    {
        ArchiveFile->Close();
    }
    ArchiveFile = nullptr;
}

void ASpinController::PrepareCSV()
{ 
    // CSV ��� �غ�
    FString csvAbsPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SpinLabels/labels.csv"));
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(csvAbsPath), /*Tree*/true);

    // ��� 1ȸ ���
    const FString header = TEXT("image,rx,ry,rz,circle_id,point_x,point_y,point_z");
    FFileHelper::SaveStringToFile(header + LINE_TERMINATOR, *csvAbsPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    //CSVAppendLine(header,csvAbsPath);

    FString line ="";
    CSVAppendLine(line,csvAbsPath);
}

void ASpinController::CSVAppendLine(const FString& Line, const FString& csvAbsPath)
{
    FFileHelper::SaveStringToFile(Line + LINE_TERMINATOR, *csvAbsPath,FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(), FILEWRITE_Append);

    /*       std::string fileNameStd(TCHAR_TO_UTF8(*filePath));
        std::ofstream csvFile(fileNameStd.c_str(), std::ios_base::app);
        if (csvFile.is_open())
        {
            for (int32 i = 0; i < ArraySize; ++i)
            {
                std::string data(TCHAR_TO_UTF8(*DataArray[i]));
                csvFile << data;
                if (i < ArraySize - 1) 
                {
                    csvFile << ",";
                }
            }
            csvFile << "\n";
            csvFile.close();

        }*/
}


void ASpinController::DoTaskSequentially(int Index, int Max)
{
    if (Index >= Max)
    {
        UE_LOG(LogTemp, Log, TEXT("All tasks finished."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Processing Index %d"), Index);

    // ��: Ÿ�̸ӷ� ������ �� ���� ����
    GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([=]()
        {
            // ���� ó�� ����, �񵿱� ó�� �� ���� �ݺ�
            DoSomeAsyncTask(Index, [=]()
                {
                    // ���� �ε����� ��� ȣ��
                    DoTaskSequentially(Index + 1, Max);
                });
        }));
}

void ASpinController::DoSomeAsyncTask(int Index, TFunction<void()> OnComplete)
{
    // ����: 1�� ������ �� �ݹ� ȣ��
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([=]()
        {
            UE_LOG(LogTemp, Log, TEXT("Finished async task for index %d"), Index);
            OnComplete(); // �Ϸ� �� �ݹ�
        }), 1.0f, false);
}
