// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionPluginRuntimeModule.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimSequence.h"
#include "Animation/BuiltInAttributeTypes.h"

#define LOCTEXT_NAMESPACE "FActionPluginModule"

class FActionPluginRuntimeModule : public IActionPluginRuntimeModule
{
};


namespace UE { namespace ActionPlugin {

// Root motion animation attribute data will always be accessible on the root bone's attribute container
static const Anim::FAttributeId RootMotionAttributeId = { Anim::IAnimRootMotionProvider::AttributeName , FCompactPoseBoneIndex(0) };

class FModule : public IModuleInterface, public Anim::IAnimRootMotionProvider
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void SampleRootMotion(const FDeltaTimeRecord& SampleRange, const UAnimSequence& Sequence, bool bLoopingSequence, Anim::FStackAttributeContainer& OutAttributes) const override;
	virtual bool OverrideRootMotion(const FTransform& RootMotionDelta, Anim::FStackAttributeContainer& OutAttributes) const override;
	virtual bool ExtractRootMotion(const Anim::FStackAttributeContainer& Attributes, FTransform& OutRootMotionDelta) const override;
	virtual bool HasRootMotion(const Anim::FStackAttributeContainer& Attributes) const override;
};

void FModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(Anim::IAnimRootMotionProvider::ModularFeatureName, this);
}

void FModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(Anim::IAnimRootMotionProvider::ModularFeatureName, this);
}

void FModule::SampleRootMotion(const FDeltaTimeRecord& SampleRange, const UAnimSequence& Sequence, bool bLoopingSequence, Anim::FStackAttributeContainer& OutAttributes) const
{
	const FTransform RootMotionTransform = Sequence.ExtractRootMotion(SampleRange.GetPrevious(), SampleRange.Delta, bLoopingSequence);
	FTransformAnimationAttribute* RootMotionAttribute = OutAttributes.FindOrAdd<FTransformAnimationAttribute>(RootMotionAttributeId);
	RootMotionAttribute->Value = RootMotionTransform;
}

bool FModule::OverrideRootMotion(const FTransform& RootMotionDelta, Anim::FStackAttributeContainer& OutAttributes) const
{
	if(FTransformAnimationAttribute* RootMotionAttribute = OutAttributes.Find<FTransformAnimationAttribute>(RootMotionAttributeId))
	{
		RootMotionAttribute->Value = RootMotionDelta;
		return true;
	}
	return false;
}
	
bool FModule::ExtractRootMotion(const Anim::FStackAttributeContainer& Attributes, FTransform& OutRootMotionDelta) const
{
	const FTransformAnimationAttribute* RootMotionAttribute = Attributes.Find<FTransformAnimationAttribute>(RootMotionAttributeId);
	OutRootMotionDelta = RootMotionAttribute ? RootMotionAttribute->Value : FTransform::Identity;
	return !!RootMotionAttribute;
}
	
bool FModule::HasRootMotion(const Anim::FStackAttributeContainer& Attributes) const
{
	return !!Attributes.Find<FTransformAnimationAttribute>(RootMotionAttributeId);
}
	
}}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(UE::ActionPlugin::FModule, ActionPluginRuntime)