// Copyright Eugene Rho, Inc. All Rights Reserved.

#pragma once

#include "AnimNode_FootFixing.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "Components/CapsuleComponent.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Animation/BuiltInAttributeTypes.h"

DECLARE_CYCLE_STAT(TEXT("Foot Fixing Eval"), STAT_FootFixing_Eval, STATGROUP_Anim);

#if ENABLE_ANIM_DEBUG
static TAutoConsoleVariable<bool> CVarAnimNodeFootFixingEnable(TEXT("a.AnimNode.FootFixing.Enable"), true, TEXT("Enable/Disable Foot Fixing"));
static TAutoConsoleVariable<bool> CVarAnimNodeFootFixingEnableLock(TEXT("a.AnimNode.FootFixing.Enable.Lock"), true, TEXT("Enable/Disable Foot Locking"));
static TAutoConsoleVariable<bool> CVarAnimNodeFootFixingDebug(TEXT("a.AnimNode.FootFixing.Debug"), false, TEXT("Turn on visualization debugging for Foot Placement"));
static TAutoConsoleVariable<bool> CVarAnimNodeFootFixingDebugTraces(TEXT("a.AnimNode.FootFixing.Debug.Traces"), false, TEXT("Turn on visualization debugging for foot ground traces"));
static TAutoConsoleVariable<int> CVarAnimNodeFootFixingDebugDrawHistory(TEXT("a.AnimNode.FootFixing.Debug.DrawHistory"), 0,
	TEXT("Turn on history visualization debugging 0 = Disabled, -1 = Pelvis, >1 = Foot Index. Clear with FlushPersistentDebugLines"));
#endif

namespace UE::Anim::ActionMatching
{
	// Root motion animation attribute data will always be accessible on the root bone's attribute container
	static const FAttributeId RootMotionAttributeId = { IAnimRootMotionProvider::AttributeName , FCompactPoseBoneIndex(0) };
};

FAnimNode_FootFixing::FAnimNode_FootFixing()
{
}

// TODO: implement 
void FAnimNode_FootFixing::GatherDebugData(FNodeDebugData& NodeDebugData)
{
	ComponentPose.GatherDebugData(NodeDebugData);
}

void FAnimNode_FootFixing::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
	//ResetRuntimeData();
}

void FAnimNode_FootFixing::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	// If we just became relevant and haven't been initialized yet, then reinitialize foot placement.
	if (!bIsFirstUpdate && UpdateCounter.HasEverBeenUpdated() && !UpdateCounter.WasSynchronizedCounter(Context.AnimInstanceProxy->GetUpdateCounter()))
	{
		//ResetRuntimeData();
	}
	UpdateCounter.SynchronizeWith(Context.AnimInstanceProxy->GetUpdateCounter());

	if(IsInGameThread())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(Cast<UAnimInstance>(Context.AnimInstanceProxy->GetAnimInstanceObject())->GetOwningActor());
		if(OwnerCharacter)
		{
			FVector ActorLocation = OwnerCharacter->GetActorLocation();
			ActorLocation += RootMotionVelocity * Context.GetDeltaTime();
			OwnerCharacter->SetActorLocation(ActorLocation);
		}
	}

	CachedDeltaTime += Context.GetDeltaTime();
}

void FAnimNode_FootFixing::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output,
	TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_FootFixing_Eval);

	check(OutBoneTransforms.Num() == 0);

	{
		//Output.Pose.GetComponentSpaceTransform(PelvisData.Bones.IkBoneIndex);
		
		FTransform RootMotionTransformDelta = FTransform::Identity;
		RootMotionTransformDelta.SetLocation(RootMotionVelocity * CachedDeltaTime);
		Output.AnimInstanceProxy->GetExtractedRootMotion().Accumulate(RootMotionTransformDelta);
		//Cast<UAnimInstance>(Output.AnimInstanceProxy->GetAnimInstanceObject())->QueueRootMotionBlend(RootMotionTransformDelta, FAnimSlotGroup::DefaultSlotName, 1.f);
	}

	
	// Manually calculate distance instead of using the teleport flag to properly handle cases like crouch
	// and instantaneous root offsets i.e. when entering/leaving a vehicle.
	// See FAnimNode_Inertialization::Evaluate_AnyThread and/or UE-78594
	const float TeleportDistanceThreshold = Output.AnimInstanceProxy->GetSkelMeshComponent()->GetTeleportDistanceThreshold();
	if (!bIsFirstUpdate && (TeleportDistanceThreshold > 0.0f))
	{
		const FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
		const FVector RootWorldSpaceLocation = ComponentTransform.TransformPosition(
			Output.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(0)).GetTranslation());
/*		
		const FVector PrevRootWorldSpaceLocation = CharacterData.ComponentTransformWS.TransformPosition(PelvisData.InputPose.RootTransformCS.GetTranslation());
		if (FVector::DistSquared(RootWorldSpaceLocation, PrevRootWorldSpaceLocation) > FMath::Square(TeleportDistanceThreshold))
		{
			ResetRuntimeData();
		}
*/		
	}

	if (const UE::Anim::IAnimRootMotionProvider* RootMotionProvider = UE::Anim::IAnimRootMotionProvider::Get())
	{
		FTransform RootMotionTransformDelta = FTransform::Identity;
		RootMotionTransformDelta.SetLocation(RootMotionVelocity * CachedDeltaTime);
		//const bool bRootMotionOverridden = RootMotionProvider->OverrideRootMotion(RootMotionTransformDelta, Output.CustomAttributes);
		//ensure(bRootMotionOverridden);
		FTransformAnimationAttribute* RootMotionAttribute = Output.CustomAttributes.FindOrAdd<FTransformAnimationAttribute>(UE::Anim::ActionMatching::RootMotionAttributeId);
		RootMotionAttribute->Value = RootMotionTransformDelta;
	}
	
	CachedDeltaTime = 0.f;
	bIsFirstUpdate = false;
}

bool FAnimNode_FootFixing::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
#if ENABLE_ANIM_DEBUG
	if (!CVarAnimNodeFootFixingEnable.GetValueOnAnyThread())
	{
		return false;
	}
#endif

	return true;
}

void FAnimNode_FootFixing::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	PelvisBone.Initialize(RequiredBones);
	IKFootRootBone.Initialize(RequiredBones);
}
