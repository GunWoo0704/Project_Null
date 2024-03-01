// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkeletalMeshComponent_ActionGame.h"
#include "SkeletalRenderPublic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "GameFramework/Controller.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(USkeletalMeshComponent_ActionGame)

CSV_DECLARE_CATEGORY_EXTERN(AnimationBudget);

USkeletalMeshComponent_ActionGame::USkeletalMeshComponent_ActionGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USkeletalMeshComponent_ActionGame::BeginPlay()
{
	Super::BeginPlay();
}

void USkeletalMeshComponent_ActionGame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USkeletalMeshComponent_ActionGame::SetComponentTickEnabled(bool bEnabled)
{
	Super::SetComponentTickEnabled(bEnabled);
}

void USkeletalMeshComponent_ActionGame::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkeletalMeshComponent_ActionGame::CompleteParallelAnimationEvaluation(bool bDoPostAnimEvaluation)
{
	Super::CompleteParallelAnimationEvaluation(bDoPostAnimEvaluation);
}

bool USkeletalMeshComponent_ActionGame::IsPlayingRootMotion() const
{
	//return Super::IsPlayingRootMotion();
	return true;
}
