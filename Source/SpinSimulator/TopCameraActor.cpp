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

    // ������ �Ʒ��� ���ϰ� �ʱ� ����
    FVector camLocation = BALL_LOCATION;
    camLocation.Z+=100;
    TopCameraComponent->SetRelativeLocation(camLocation); // ������ ��
    TopCameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // �Ʒ��� ����
}

ATopCameraActor::~ATopCameraActor()
{
}

void ATopCameraActor::BeginPlay()
{
    Super::BeginPlay();

    //SetAperture(4.0f); // F4
    //SetManualFocusDistance(1000.f);

    //�ǹ� ī�޶� F4 ����
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
