#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpinSimulatorData.generated.h"

USTRUCT(BlueprintType)
struct SPINSIMULATOR_API FSpinData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PRM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpinAxisX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpinAxisY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpinAxisZ;
};
