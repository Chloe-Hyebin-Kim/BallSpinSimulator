#include "TopCameraActor.h"


#include "SpinSimulator.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ATopCameraActor::ATopCameraActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TopCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopCamera"));
    RootComponent = TopCameraComponent;

    // 위에서 아래를 향하게 초기 설정
    FVector camLocation = BALL_LOCATION;
    camLocation.Z+=100;
    TopCameraComponent->SetRelativeLocation(camLocation); // 공보다 위
    TopCameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // 아래를 향함
}

ATopCameraActor::~ATopCameraActor()
{
}

void ATopCameraActor::BeginPlay()
{
    Super::BeginPlay();

    //SetAperture(4.0f); // F4
    //SetManualFocusDistance(1000.f);

    //실물 카메라 F4 대응
    //LensSettings.MinFStop = 1.2f;
    //CurrentAperture = 4.0f; // F4
    
    //SetFieldOfView(50.f);
}

void ATopCameraActor::SetTarget(AActor* Target)
{
    TargetActor = Target;
    if (TargetActor)
    {
        FVector BallLocation = TargetActor->GetActorLocation();
        SetActorLocation(FVector(BallLocation.X, BallLocation.Y, BallLocation.Z + 50.f));
    }
}
