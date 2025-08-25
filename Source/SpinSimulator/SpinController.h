#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GolfBall.h"

#include "SpinController.generated.h"

class AGolfBall;

UCLASS()
class SPINSIMULATOR_API ASpinController : public APlayerController
{
    GENERATED_BODY()

public:
    ASpinController();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


    template <typename FmtType, typename... Types>
    void Log(ELogVerbosity::Type InVerbosity, const FmtType& Fmt, Types... Args)
    {
        if (!ArchiveFile) return;

        AddTimeStamp();

        FString LogMsg = FString::Printf(Fmt, Args...) + TEXT("\n");
        FTCHARToUTF8 UTF8String(*LogMsg, LogMsg.Len());
        File->Serialize((UTF8CHAR*)UTF8String.Get(), UTF8String.Length() * sizeof(UTF8CHAR));
        File->Flush();
    }


public:
    AGolfBall* GetBallActor() {return ControlledBallActor;}
    bool CheckReadCSV() {return bReadRangeforSyntheticData;}
    void ReadRangeforSyntheticData();
    void ScanBallMeshVertexData();

    const TArray<FVector> GetSpinAxisArray() {return m_arrSpinAxis;}
    const TArray<float> GetRPMArray() { return m_arrRPM; }

	void AddSpinAxis(FVector newSpinAxis) { m_arrSpinAxis.Add(newSpinAxis); }
	void AddRPM(float newRPM) { m_arrRPM.Add(newRPM); }

protected:
    void RegisterCMD();
    void UnregisterCMD();

    /** 콘솔에서 호출될 SetAxis 명령 함수 */
    void OnSwitchBallSpinCommand(const TArray<FString>& Args);
    void OnSetSpinSpeedCommand(const TArray<FString>& Args);
    void OnSetAxisCommand(const TArray<FString>& Args);
    void OnShowOriginAxisCommand(const TArray<FString>& Args);
    void OnShowBallAxisCommand(const TArray<FString>& Args);
    void OnCaptureCameraView(const TArray<FString>& Args);
    void OnCaptureAllCombinations(const TArray<FString>& Args);
    void OnCaptureCSV(const TArray<FString>& Args);
    void OnCheckVertexPosition(const TArray<FString>& Args);

    void VirtualSpinCapture();

    void DoTaskSequentially(int Index, int Max);
    void DoSomeAsyncTask(int Index, TFunction<void()> OnComplete);

    bool StartLogFile(const FString& Prefix);
    void PrepareCSV();
    void CSVAppendLine(const FString& Line, const FString& csvAbsPath);
    void CloseLogFile();
    bool LoadCSV(const TCHAR* FilePathName, TArray<FString>& OutPutStrings);

private:
    TArray<FVector> m_arrSpinAxis;
    TArray<float>  m_arrRPM;
    bool bReadRangeforSyntheticData;

protected:
    TUniquePtr<FArchive> ArchiveFile;
    
    /** 골프공 액터 참조 (에디터 또는 런타임에서 설정) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spin")
        AGolfBall* ControlledBallActor;

};