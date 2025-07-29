#include "TopCameraActor.h"


#include "SpinSimulator.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include <CinematicCamera/Public/CineCameraActor.h>


ATopCameraActor::ATopCameraActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TopCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("TopCamera"));
    RootComponent = TopCameraComponent;

    // ������ �Ʒ��� ���ϰ� �ʱ� ����
    FVector camLocation = BALL_LOCATION;
    camLocation.Z=300.f;
    TopCameraComponent->SetRelativeLocation(camLocation); // ������ ��
    TopCameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // �Ʒ��� ����
    TopCameraComponent->SetFieldOfView(23);




    
    // �ǹ� ī�޶� F4 ����
	TopCameraComponent->LensSettings.MinFStop = 1.2f;
	TopCameraComponent->SetFieldOfView(27.f);
    // ���� ũ�� ���� (mm)
	TopCameraComponent->Filmback.SensorWidth = 14.56f;
	TopCameraComponent->Filmback.SensorHeight = 12.63f;

    // ���� ���� �Ÿ� ����
   TopCameraComponent->CurrentFocalLength = 39.0f;

   // ������ �� �߰� ����
   TopCameraComponent->CurrentAperture = 4.0f;
     

 // (Optional) ���� ���� �Ÿ�
	TopCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	TopCameraComponent->FocusSettings.ManualFocusDistance = 1000.0f;
}


ATopCameraActor::~ATopCameraActor()
{
}

void ATopCameraActor::BeginPlay()
{
    Super::BeginPlay();
}

void ATopCameraActor::SetTarget(AActor* Target)
{
    TargetActor = Target;
    if (TargetActor)
    {
        FVector BallLocation = TargetActor->GetActorLocation();
        SetActorLocation(FVector(BallLocation.X, BallLocation.Y, BallLocation.Z+50.f));//300.f));

        TopCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;
        TopCameraComponent->FocusSettings.TrackingFocusSettings.ActorToTrack= Target;
        TopCameraComponent->FocusSettings.TrackingFocusSettings.bDrawDebugTrackingFocusPoint=false;
    }
}

void ATopCameraActor::CaptureFrame()
{
    // ���� ��ǥ�迡�� ȸ���� ��� (���� �� ����)
    FQuat rotationQuat = FQuat(m_SpinAxisAsVec, FMath::DegreesToRadians(m_DegreesPerSecond * DeltaTime));
    AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


    // ����� ���� (ȸ���� �ð�ȭ)
    DrawDebugLine(GetWorld(), m_vecBallLocation - m_SpinAxisAsVec * 1000.f, m_vecBallLocation + m_SpinAxisAsVec * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);


}
