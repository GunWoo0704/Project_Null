// Copyright Eugene Rho, Inc. All Rights Reserved.

#pragma once

#include "BoneControllers/BoneControllerTypes.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Animation/AnimNodeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

#include "AnimNode_FootFixing.generated.h"

namespace UE::Anim::FootFixing
{
}

USTRUCT(BlueprintInternalUseOnly, Experimental)
struct ACTIONPLUGINRUNTIME_API FAnimNode_FootFixing : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

	// Foot/Ball speed evaluation mode (Graph or Manual) used to decide when the feet are locked
	// Graph mode uses the root motion attribute from the animations to calculate the joint's speed
	// Manual mode uses a per-foot curve name representing the joint's speed
	UPROPERTY(EditAnywhere, Category = "Settings")
	EWarpingEvaluationMode PlantSpeedMode = EWarpingEvaluationMode::Manual;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference IKFootRootBone;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference PelvisBone;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector RootMotionVelocity;
	
	FAnimNode_FootFixing();
	
	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface
	
	// FAnimNode_SkeletalControlBase interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(
			FComponentSpacePoseContext& Output,
			TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	bool bIsFirstUpdate = false;
	FGraphTraversalCounter UpdateCounter;
	float CachedDeltaTime = 0.0f;
};