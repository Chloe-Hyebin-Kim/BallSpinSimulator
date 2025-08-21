#include "TopCameraActor.h"


#include "SpinSimulator.h"
#include "GolfBall.h"
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
    camLocation.Z += 60.0f;
    TopCameraComponent->SetRelativeLocation(camLocation); // 공보다 위
    TopCameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // 아래를 향함
    //TopCameraComponent->SetFieldOfView(23);


    // Filmback 센서 크기 설정 (mm)
    TopCameraComponent->Filmback.SensorWidth = 16.f; //56f;
    TopCameraComponent->Filmback.SensorHeight = 16.f; //12.63f;
    
    // LensSettings 
	TopCameraComponent->LensSettings.MinFocalLength = 100.f;
	TopCameraComponent->LensSettings.MaxFocalLength = 100.f;
	TopCameraComponent->LensSettings.MinFStop = 2.8f;
	TopCameraComponent->LensSettings.MaxFStop = 10.f;



   TopCameraComponent->CurrentFocalLength = 100.f; //39.0f;
   TopCameraComponent->CurrentAperture =6.0f;
   TopCameraComponent->CurrentFocusDistance = 50.f;
   //TopCameraComponent->CurrentHorizontalFOV = 5.72481f;//27.f;
   //TopCameraComponent->SetFieldOfView(27.f);

 // (Optional) 수동 초점 거리
	/*TopCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;

    AActor* targetActor = Cast<AGolfBall>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolfBall::StaticClass()));
    if (!targetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can not find AGolfBall Actor."));
        return;
    }
	TopCameraComponent->FocusSettings.TrackingFocusSettings.ActorToTrack = targetActor;*/

}


ATopCameraActor::~ATopCameraActor()
{
}

void ATopCameraActor::BeginPlay()
{
    Super::BeginPlay();
}

void ATopCameraActor::SetTarget(AGolfBall* Target)
{
    TargetActor = Target;
    if (TargetActor)
    {
        //FVector BallLocation = TargetActor->GetActorLocation();
        SetActorLocation(FVector(BALL_LOCATION.X, BALL_LOCATION.Y, BALL_LOCATION.Z+60.f));//300.f));

        TopCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;
        TopCameraComponent->FocusSettings.TrackingFocusSettings.ActorToTrack = Target;
        TopCameraComponent->FocusSettings.TrackingFocusSettings.bDrawDebugTrackingFocusPoint=false;
    }
}

void ATopCameraActor::CaptureFrame()
{
    // 월드 좌표계에서 회전축 계산 (로컬 축 기준)
    float DegreesPerSecond = TargetActor->GetDegreesPerSecond();
    float DegreesPerFrame = TargetActor->GetDegreesPerFrame();
    FVector SpinAxis = TargetActor->GetBallSpinAxis();

    FQuat rotationQuat = FQuat(SpinAxis, FMath::DegreesToRadians(DegreesPerSecond * 0.002));
    AddActorWorldRotation(rotationQuat, false, nullptr, ETeleportType::None);


    // 디버그 라인 (회전축 시각화)
    //DrawDebugLine(GetWorld(), BALL_LOCATION - SpinAxis * 1000.f, BALL_LOCATION + SpinAxis * 1000.f, FColor::Orange, false, 1.f, 0, 0.1f);


}
