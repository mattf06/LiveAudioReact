// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioBar.h"

#include <Components/BillboardComponent.h>

// Sets default values
AAudioBar::AAudioBar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UBillboardComponent* billboardComp = UObject::CreateDefaultSubobject<UBillboardComponent>(TEXT("Root Comp"));
	check(billboardComp);

	SetRootComponent(billboardComp);
}

// Called when the game starts or when spawned
void AAudioBar::BeginPlay()
{
	Super::BeginPlay();

	GetAndBindAudioComponent();
}

void AAudioBar::GetAndBindAudioComponent()
{
	if (AudioCaptureManager != nullptr)
	{
		MyDelegateHandle = AudioCaptureManager->OnAudioCaptureNativeEvent.AddUObject(this, &AAudioBar::OnReceiveData);
	}
}

void AAudioBar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AudioCaptureManager != nullptr)
	{
		AudioCaptureManager->OnAudioCaptureNativeEvent.Remove(MyDelegateHandle);
	}
}

void AAudioBar::releasePreviousBoxes()
{
	for (auto It = boxes.CreateIterator(); It; It++)
	{
		if (*It != nullptr)
			(*It)->DestroyComponent();
	}
	boxes.Empty();
}

// Called every frame
void AAudioBar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAudioBar::OnReceiveData(const TArray<float>& data)
{
	// 	UE_LOG(LogTemp, Log, TEXT("AAudioBar::OnReceiveData"));
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AAudioBar::OnReceiveData"));

	// 	GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Red, FString::Printf(TEXT("OnReceiveData")));

	if (boxes.Num() == 0)
		return;

	for (int i = 0; i < boxes.Num(); i++)
	{
		UStaticMeshComponent* boxe = boxes[i];
		int index = FMath::GetMappedRangeValueUnclamped(FVector2D(0, boxes.Num() - 1), FVector2D(0, data.Num() - 1), i);
		FVector scale = boxe->GetRelativeScale3D();
		scale.Z = FMath::Clamp(data[index], 0.0001f, 9999.0f);
		boxe->SetRelativeScale3D(scale);

		UMaterialInstanceDynamic* mat = boxe->CreateAndSetMaterialInstanceDynamic(0);
		if (mat)
		{
			mat->SetScalarParameterValue(FName("Opacity"), FMath::Clamp(scale.Z * OpcityFactor, 0.0f, 1.0f));
		}
	}
}

void AAudioBar::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AAudioBar::OnConstruction"));

	// 	UE_LOG(LogTemp, Log, TEXT("AAudioBar::OnConstruction"));

	releasePreviousBoxes();

	for (int i = 0; i < NumBoxes; i++)
	{
		UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this);
		if (MeshComponent != nullptr)
		{
			MeshComponent->RegisterComponent();
			MeshComponent->SetStaticMesh(Mesh);

			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComponent->SetMobility(EComponentMobility::Movable);

			MeshComponent->SetWorldScale3D(InitialScale);
			MeshComponent->SetRelativeLocation(OffsetStep * i);

			MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			if (TemplateMaterialInstance != nullptr)
			{
				auto MatInstance = MeshComponent->CreateDynamicMaterialInstance(0, TemplateMaterialInstance, FName("BoxeMat"));
				if (MatInstance != nullptr)
				{
					MatInstance->SetVectorParameterValue(
						"BaseColor", FMath::Lerp(ColorStart, ColorEnd, float(i) / float(NumBoxes)));
				}
			}

			boxes.Add(MeshComponent);
		}
	}

	this->SetActorEnableCollision(false);

	RegisterAllComponents();
}
