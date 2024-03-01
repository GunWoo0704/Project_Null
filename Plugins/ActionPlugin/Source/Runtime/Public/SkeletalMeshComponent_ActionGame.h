// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "SkeletalMeshComponent_ActionGame.generated.h"

/** A skeletal mesh component that has its tick rate governed by a global animation budget */
UCLASS(meta=(BlueprintSpawnableComponent))
class ACTIONPLUGINRUNTIME_API USkeletalMeshComponent_ActionGame : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	USkeletalMeshComponent_ActionGame(const FObjectInitializer& ObjectInitializer);

	// UActorComponent interface
	virtual void SetComponentTickEnabled(bool bEnabled) override;

	// USkeletalMeshComponent interface
	virtual bool IsPlayingRootMotion() const override;

protected:
	// UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	// USkeletalMeshComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void CompleteParallelAnimationEvaluation(bool bDoPostAnimEvaluation) override;
};