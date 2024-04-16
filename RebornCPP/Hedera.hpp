#pragma once

#include "SdkHeaders.hpp"
#include "proto_out/definitions.pb.h"

namespace Hedera {
	class Dumper {
	public:
		static HederaDefinitions::FRotator* DumpFRotator(FRotator a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FVector* DumpFVector(FVector a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FName* DumpFName(FName a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FEnvironmentalEffectInfo* DumpFEnvironmentalEffectInfo(FEnvironmentalEffectInfo a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FAttributeDefinitionDefault* DumpFAttributeDefinitionDefault(FAttributeDefinitionDefault a, std::vector<UActorChannel*>& channels);
		static void DumpFAttributeScalarParam(FAttributeScalarParam a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAttributeScalarParam* scalarParam);
		static void DumpFScreenParticleModifier(FScreenParticleModifier a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleModifier* ret);
		static void DumpFInteractionIconWithOverrides(FInteractionIconWithOverrides a, std::vector<UActorChannel*>& channels, HederaDefinitions::FInteractionIconWithOverrides* ret);
		static HederaDefinitions::FVector2D* DumpFVector2D(FVector2D a, std::vector<UActorChannel*>& channels);
		static void DumpFScreenParticleScalarParamOverTime(FScreenParticleScalarParamOverTime a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleScalarParamOverTime* ret);
		static void DumpFScreenParticleVectorParamOverTime(FScreenParticleVectorParamOverTime a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleVectorParamOverTime* ret);
		static HederaDefinitions::FScreenParticleParamOverTime* DumpFScreenParticleParamOverTime(FScreenParticleParamOverTime a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FScreenParticleInitParams* DumpFScreenParticleInitParams(FScreenParticleInitParams a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FReplicatedDamageScreenParticle* DumpFReplicatedDamageScreenParticle(FReplicatedDamageScreenParticle a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FReplicatedProgressBarData* DumpFReplicatedProgressBarData(FReplicatedProgressBarData a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FCameraViewData* DumpFCameraViewData(FCameraViewData a, std::vector<UActorChannel*>& channels);
		static void DumpFActorSelectionDataContainer(FActorSelectionDataContainer a, std::vector<UActorChannel*>& channels, HederaDefinitions::FActorSelectionDataContainer* ret);
		static HederaDefinitions::APoplarPlayerController* DumpAPoplarPlayerController(APoplarPlayerController* a, std::vector<UActorChannel*>& channels);
	
		static HederaDefinitions::FUniqueID* DumpFUniqueID(FUniqueNetId a, std::vector<UActorChannel*>& channels);
		static void DumpFCurrencyState(FCurrencyState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FCurrencyState* ret);
		static HederaDefinitions::FResourcePoolReference* DumpFResourcePoolReference(FResourcePoolReference a, std::vector<UActorChannel*>& channels);
		static void DumpFReplicatedNounAttributeState(FReplicatedNounAttributeState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FReplicatedNounAttributeState* ret);
		static HederaDefinitions::FWeaponDefinitionData* DumpFWeaponDefinitionData(FWeaponDefinitionData a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FReplicatedStandInGear* DumpFReplicatedStandInGear(FReplicatedStandInGear a, std::vector<UActorChannel*>& channels);
		static void DumpFTrackedSkillState(FTrackedSkillState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FTrackedSkillState* ret);
		static void DumpFManagedActorCountData(FManagedActorCountData a, std::vector<UActorChannel*>& channels, HederaDefinitions::FManagedActorCountData* ret);
		static void DumpFReplicatedPerkItem(FReplicatedPerkItem a, std::vector<UActorChannel*>& channels, HederaDefinitions::FReplicatedPerkItem* ret);
		static HederaDefinitions::FAugInstance* DumpFAugInstance(FAugInstance a, std::vector<UActorChannel*>& channels);
		static void DumpFAugCategoryInstance(FAugCategoryInstance a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAugCategoryInstance* ret);
		static HederaDefinitions::FAugSetInstance* DumpFAugSetInstance(FAugSetInstance a, std::vector<UActorChannel*>& channels);
		static void DumpFAugInstancePassedIn(FAugInstance a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAugInstance* ret);
		static HederaDefinitions::FReplicatedBehaviorConsumerState* DumpFReplicatedBehaviorConsumerState(FReplicatedBehaviorConsumerState a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FReplicatedBehaviorEvent* DumpFReplicatedBehaviorEvent(FReplicatedBehaviorEvent a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::FCinematicModeOptions* DumpFCinematicModeOptions(FCinematicModeOptions a, std::vector<UActorChannel*>& channels);
		static HederaDefinitions::APoplarPlayerReplicationInfo* DumpAPoplarPlayerReplicationInfo(APoplarPlayerReplicationInfo* a, std::vector<UActorChannel*>& channels);
	private:
		static HederaDefinitions::NonReplicatedObjectReference* ObjectToObjectReference(UObject* obj);
		static HederaDefinitions::ReplicatedActorReference* ReplicatedActorToReplicatedActorReference(AActor* actor, std::vector<UActorChannel*>& channels);
	};


	class Parser {
	public:
		static FRotator ParseFRotator(HederaDefinitions::FRotator a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FVector ParseFVector(HederaDefinitions::FVector a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FName ParseFName(HederaDefinitions::FName a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FEnvironmentalEffectInfo ParseFEnvironmentalEffectInfo(HederaDefinitions::FEnvironmentalEffectInfo a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAttributeDefinitionDefault ParseFAttributeDefinitionDefault(HederaDefinitions::FAttributeDefinitionDefault a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAttributeScalarParam ParseFAttributeScalarParam(HederaDefinitions::FAttributeScalarParam a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FScreenParticleModifier ParseFScreenParticleModifier(HederaDefinitions::FScreenParticleModifier a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FInteractionIconWithOverrides ParseFInteractionIconWithOverrides(HederaDefinitions::FInteractionIconWithOverrides a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FVector2D ParseFVector2D(HederaDefinitions::FVector2D a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FScreenParticleScalarParamOverTime ParseFScreenParticleScalarParamOverTime(HederaDefinitions::FScreenParticleScalarParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FScreenParticleVectorParamOverTime ParseFScreenParticleVectorParamOverTime(HederaDefinitions::FScreenParticleVectorParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FScreenParticleParamOverTime ParseFScreenParticleParamOverTime(HederaDefinitions::FScreenParticleParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FScreenParticleInitParams ParseFScreenParticleInitParams(HederaDefinitions::FScreenParticleInitParams a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedDamageScreenParticle ParseFReplicatedDamageScreenParticle(HederaDefinitions::FReplicatedDamageScreenParticle a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedProgressBarData ParseFReplicatedProgressBarData(HederaDefinitions::FReplicatedProgressBarData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FCameraViewData ParseFCameraViewData(HederaDefinitions::FCameraViewData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FActorSelectionDataContainer ParseFActorSelectionDataContainer(HederaDefinitions::FActorSelectionDataContainer a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static void ParseAPoplarPlayerController(HederaDefinitions::APoplarPlayerController a, APoplarPlayerController* b, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
	
		static FUniqueNetId ParseFUniqueID(HederaDefinitions::FUniqueID a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FCurrencyState ParseFCurrencyState(HederaDefinitions::FCurrencyState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FResourcePoolReference ParseFResourcePoolReference(HederaDefinitions::FResourcePoolReference a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedNounAttributeState ParseFReplicatedNounAttributeState(HederaDefinitions::FReplicatedNounAttributeState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FWeaponDefinitionData ParseFWeaponDefinitionData(HederaDefinitions::FWeaponDefinitionData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedStandInGear ParseFReplicatedStandInGear(HederaDefinitions::FReplicatedStandInGear a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FTrackedSkillState ParseFTrackedSkillState(HederaDefinitions::FTrackedSkillState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FManagedActorCountData ParseFManagedActorCountData(HederaDefinitions::FManagedActorCountData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedPerkItem ParseFReplicatedPerkItem(HederaDefinitions::FReplicatedPerkItem a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAugInstance ParseFAugInstance(HederaDefinitions::FAugInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAugCategoryInstance ParseFAugCategoryInstance(HederaDefinitions::FAugCategoryInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAugSetInstance ParseFAugSetInstance(HederaDefinitions::FAugSetInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FAugInstance ParseFAugInstancePassedIn(HederaDefinitions::FAugInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedBehaviorConsumerState ParseFReplicatedBehaviorConsumerState(HederaDefinitions::FReplicatedBehaviorConsumerState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FReplicatedBehaviorEvent ParseFReplicatedBehaviorEvent(HederaDefinitions::FReplicatedBehaviorEvent a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static FCinematicModeOptions ParseFCinematicModeOptions(HederaDefinitions::FCinematicModeOptions a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
		static void ParseAPoplarPlayerReplicationInfo(HederaDefinitions::APoplarPlayerReplicationInfo a, APoplarPlayerReplicationInfo* b, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects);
	private:
		static UObject* ObjectReferenceToObject(const HederaDefinitions::NonReplicatedObjectReference& obj, TArray<UObject*>* GObjects, UClass* className);
		static AActor* ReplicatedActorReferenceToReplicatedActor(const HederaDefinitions::ReplicatedActorReference& actor, std::vector<UActorChannel*>& channels);
	private:
		static std::map<std::string, UObject*> cacheMap;
	};
}