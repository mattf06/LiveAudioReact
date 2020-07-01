// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindowsAudioCaptureActor.h"
#include <Engine/EngineTypes.h>

#include "AudioBar.generated.h"

UCLASS()
class LIVEAUDIOREACT_API AAudioBar : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAudioBar();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void GetAndBindAudioComponent();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void releasePreviousBoxes();
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AudioBar | Template")
    class UStaticMesh* Mesh = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AudioBar | Template")
    class UMaterialInstance* TemplateMaterialInstance = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AudioBar | Capture")
    class AWindowsAudioCaptureActor* AudioCaptureManager;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AudioBar | Settings")
    int32 NumBoxes = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AudioBar | Settings")
    FVector InitialScale = FVector::OneVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AudioBar | Settings")
    FVector OffsetStep = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AudioBar | Settings")
    FLinearColor ColorStart = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AudioBar | Settings")
    FLinearColor ColorEnd = FLinearColor::Green;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AudioBar | Settings")
    float OpcityFactor = 1.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AudioBar | Settings")
    TArray<UStaticMeshComponent*> boxes;

    UFUNCTION(BlueprintInternalUseOnly)
    void OnReceiveData(const TArray<float>& data);

    FDelegateHandle MyDelegateHandle;

protected:
    FTimerHandle CaptureDataTimerHandler;
};
