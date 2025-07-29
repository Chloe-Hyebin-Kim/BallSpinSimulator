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

    // 위에서 아래를 향하게 초기 설정
    FVector camLocation = BALL_LOCATION;
    camLocation.Z=300.f;
    TopCameraComponent->SetRelativeLocation(camLocation); // 공보다 위
    TopCameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // 아래를 향함
    TopCameraComponent->SetFieldOfView(23);




    
    // 실물 카메라 F4 대응
	TopCameraComponent->LensSettings.MinFStop = 1.2f;
	TopCameraComponent->SetFieldOfView(27.f);
    // 센서 크기 설정 (mm)
	TopCameraComponent->Filmback.SensorWidth = 14.56f;
	TopCameraComponent->Filmback.SensorHeight = 12.63f;

    // 렌즈 초점 거리 설정
   TopCameraComponent->CurrentFocalLength = 39.0f;

   // 조리개 등 추가 설정
   TopCameraComponent->CurrentAperture = 4.0f;
     

 // (Optional) 수동 초점 거리
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
