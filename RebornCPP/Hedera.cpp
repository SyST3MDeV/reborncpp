#include "pch.h"
#include "proto_out/definitions.pb.h"

#include "SdkHeaders.hpp"
#include "Hedera.hpp"

using namespace Hedera;

HederaDefinitions::NonReplicatedObjectReference* Dumper::ObjectToObjectReference(UObject* obj) {
	HederaDefinitions::NonReplicatedObjectReference* ret = new HederaDefinitions::NonReplicatedObjectReference();

	if (obj) {
		//std::cout << "Found OBJ NAME " << obj->GetFullName() << std::endl;
		ret->set_fullnamelookup(obj->GetFullName());
	}
	else {
		ret->set_fullnamelookup("null");
	}
	

	return ret;
}

HederaDefinitions::ReplicatedActorReference* Dumper::ReplicatedActorToReplicatedActorReference(AActor* actor, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::ReplicatedActorReference* ret = new HederaDefinitions::ReplicatedActorReference();

	bool bFound = false;
	uint32_t id = 0;

	for (UActorChannel* channel : channels) {
		if (channel->Actor == actor) {
			id = channel->ChIndex;
			bFound = true;

			//std::cout << "Found CHIDX " << id << std::endl;
		}
	}

	ret->set_actorid(id);

	return ret;
}

HederaDefinitions::FRotator* Dumper::DumpFRotator(FRotator a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FRotator* ret = new HederaDefinitions::FRotator();
	ret->set_pitch(a.Pitch);
	ret->set_yaw(a.Yaw);
	ret->set_roll(a.Roll);
	return ret;
}

HederaDefinitions::FVector* Dumper::DumpFVector(FVector a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FVector* ret = new HederaDefinitions::FVector();
	ret->set_x(a.X);
	ret->set_y(a.Y);
	ret->set_z(a.Z);
	return ret;
}

HederaDefinitions::FName* Dumper::DumpFName(FName a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FName* ret = new HederaDefinitions::FName();
	ret->set_fnameentryid(a.FNameEntryId);
	ret->set_instancenumber(a.InstanceNumber);
	return ret;
}

HederaDefinitions::FEnvironmentalEffectInfo* Dumper::DumpFEnvironmentalEffectInfo(FEnvironmentalEffectInfo a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FEnvironmentalEffectInfo* ret = new HederaDefinitions::FEnvironmentalEffectInfo();
	ret->set_auxbusvolume(a.AuxBusVolume);
	ret->set_effectid(a.EffectID);
	ret->set_outputbusvolume(a.OutputBusVolume);
	return ret;
}

HederaDefinitions::FAttributeDefinitionDefault* Dumper::DumpFAttributeDefinitionDefault(FAttributeDefinitionDefault a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FAttributeDefinitionDefault* ret = new HederaDefinitions::FAttributeDefinitionDefault();

	ret->set_allocated_attribute(ObjectToObjectReference(a.Attribute));
	ret->set_defaultvalue(a.DefaultValue);

	return ret;
}

void Dumper::DumpFAttributeScalarParam(FAttributeScalarParam a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAttributeScalarParam* scalarParam) {
	scalarParam->set_allocated_paramname(DumpFName(a.ParamName, channels));
	scalarParam->set_bvectorparam(a.bVectorParam);
	scalarParam->set_bclampinput(a.bClampInput);
	scalarParam->set_allocated_input(DumpFAttributeDefinitionDefault(a.Input, channels));
	scalarParam->set_allocated_mininput(DumpFAttributeDefinitionDefault(a.MinInput, channels));
	scalarParam->set_allocated_maxinput(DumpFAttributeDefinitionDefault(a.MaxInput, channels));
	scalarParam->set_allocated_minoutput(DumpFAttributeDefinitionDefault(a.MinOutput, channels));
	scalarParam->set_allocated_maxoutput(DumpFAttributeDefinitionDefault(a.MaxOutput, channels));
	scalarParam->set_scalarparamoperand(a.ScalarParamOperand);
	scalarParam->set_currentvalue(a.CurrentValue);
}

void Dumper::DumpFScreenParticleModifier(FScreenParticleModifier a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleModifier* ret) {
	for (FAttributeScalarParam scalarParam : a.ScalarParams) {
		HederaDefinitions::FAttributeScalarParam* scalar = ret->add_scalarparams();
		DumpFAttributeScalarParam(scalarParam, channels, scalar);
	}

	ret->set_cachedvalue(a.CachedValue);
}

void Dumper::DumpFInteractionIconWithOverrides(FInteractionIconWithOverrides a, std::vector<UActorChannel*>& channels, HederaDefinitions::FInteractionIconWithOverrides* ret) {
	ret->set_allocated_icondef(ObjectToObjectReference(a.IconDef));
	ret->set_allocated_overrideicondef(ObjectToObjectReference(a.OverrideIconDef));
	ret->set_boverrideicon(a.bOverrideIcon);
	ret->set_boverrideaction(a.bOverrideAction);
	ret->set_boverridetext(a.bOverrideText);
	ret->set_bcoststouse(a.bCostsToUse);
	ret->set_costscurrencytype(a.CostsCurrencyType);
	ret->set_costsamount(a.CostsAmount);
}

HederaDefinitions::FVector2D* Dumper::DumpFVector2D(FVector2D a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FVector2D* ret = new HederaDefinitions::FVector2D();

	ret->set_x(a.X);
	ret->set_y(a.Y);

	return ret;
}

void Dumper::DumpFScreenParticleScalarParamOverTime(FScreenParticleScalarParamOverTime a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleScalarParamOverTime* ret) {
	ret->set_allocated_parametername(DumpFName(a.ParameterName, channels));
	ret->set_endvalue(a.EndValue);
	ret->set_lastvalue(a.LastValue);
	ret->set_startvalue(a.StartValue);
	ret->set_totaltime(a.TotalTime);
}

void Dumper::DumpFScreenParticleVectorParamOverTime(FScreenParticleVectorParamOverTime a, std::vector<UActorChannel*>& channels, HederaDefinitions::FScreenParticleVectorParamOverTime* ret) {
	ret->set_allocated_parametername(DumpFName(a.ParameterName, channels));
	ret->set_totaltime(a.TotalTime);
	ret->set_allocated_vectorstartvalue(DumpFVector(a.VectorStartValue, channels));
	ret->set_allocated_vectorendvalue(DumpFVector(a.VectorEndValue, channels));
	ret->set_allocated_lastvalue(DumpFVector(a.LastValue, channels));
}

HederaDefinitions::FScreenParticleParamOverTime* Dumper::DumpFScreenParticleParamOverTime(FScreenParticleParamOverTime a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FScreenParticleParamOverTime* ret = new HederaDefinitions::FScreenParticleParamOverTime();

	for (FScreenParticleScalarParamOverTime ScalarParam : a.ScalarParams) {
		HederaDefinitions::FScreenParticleScalarParamOverTime* ScalarParams = ret->add_scalarparams();
		DumpFScreenParticleScalarParamOverTime(ScalarParam, channels, ScalarParams);
	}

	for (FScreenParticleVectorParamOverTime VectorParam : a.VectorParams) {
		HederaDefinitions::FScreenParticleVectorParamOverTime* VectorParams = ret->add_vectorparams();
		DumpFScreenParticleVectorParamOverTime(VectorParam, channels, VectorParams);
	}

	return ret;
}

HederaDefinitions::FScreenParticleInitParams* Dumper::DumpFScreenParticleInitParams(FScreenParticleInitParams a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FScreenParticleInitParams* ret = new HederaDefinitions::FScreenParticleInitParams();

	ret->set_allocated_template_(ObjectToObjectReference(a.Template));
	
	for (FScreenParticleModifier mod : a.ScreenParticleModifiers) {
		HederaDefinitions::FScreenParticleModifier* allocMod = ret->add_screenparticlemodifiers();
		DumpFScreenParticleModifier(mod, channels, allocMod);
	}

	ret->set_allocated_templatescreenparticlematerial(ObjectToObjectReference(a.TemplateScreenParticleMaterial));
	ret->set_allocated_matparamname(DumpFName(a.MatParamName, channels));
	ret->set_bhidewhenfinished(a.bHideWhenFinished);
	ret->set_allocated_particletag(DumpFName(a.ParticleTag, channels));
	ret->set_allocated_contentdims(DumpFVector2D(a.ContentDims, channels));
	ret->set_particledepth(a.ParticleDepth);
	ret->set_scalingmode(a.ScalingMode);
	ret->set_allocated_stopparamsot(DumpFScreenParticleParamOverTime(a.StopParamsOT, channels));
	ret->set_bonlyownersee(a.bOnlyOwnerSee);
	ret->set_effectdepthprioritygroup(a.EffectDepthPriorityGroup);
	ret->set_balwaysvisible(a.bAlwaysVisible);
	ret->set_busedesignerattributeparameters(a.bUseDesignerAttributeParameters);

	return ret;
}

HederaDefinitions::FReplicatedDamageScreenParticle* Dumper::DumpFReplicatedDamageScreenParticle(FReplicatedDamageScreenParticle a, std::vector<UActorChannel*>& channels){
	HederaDefinitions::FReplicatedDamageScreenParticle* ret = new HederaDefinitions::FReplicatedDamageScreenParticle();

	ret->set_allocated_params(DumpFScreenParticleInitParams(a.Params, channels));
	ret->set_allocated_referenceloaction(DumpFVector(a.ReferenceLocation, channels));
	ret->set_genericscale(a.GenericScale);
	ret->set_counter(a.Counter);

	return ret;
}

HederaDefinitions::FReplicatedProgressBarData* Dumper::DumpFReplicatedProgressBarData(FReplicatedProgressBarData a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FReplicatedProgressBarData* ret = new HederaDefinitions::FReplicatedProgressBarData();

	ret->set_duration(a.Duration);
	ret->set_rate(a.Rate);
	ret->set_startoffset(a.StartOffset);

	return ret;
}

HederaDefinitions::FCameraViewData* Dumper::DumpFCameraViewData(FCameraViewData a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FCameraViewData* ret = new HederaDefinitions::FCameraViewData();

	ret->set_allocated_viewtarget(ReplicatedActorToReplicatedActorReference(a.ViewTarget, channels));
	ret->set_allocated_modename(DumpFName(a.ModeName, channels));
	ret->set_blendtimeoverride(a.BlendTimeOverride);
	ret->set_bisteleport(a.bIsTeleport);
	ret->set_desireddeathspectatorstate(a.DesiredDeathSpectatorState);

	return ret;
}

void Dumper::DumpFActorSelectionDataContainer(FActorSelectionDataContainer a, std::vector<UActorChannel*>& channels, HederaDefinitions::FActorSelectionDataContainer* ret) {
	ret->set_health(a.Health);
	ret->set_bdisplayhealth(a.bDisplayHealth);
	ret->set_shield(a.Shield);
	ret->set_bdisplayshield(a.bDisplayShield);
	ret->set_overshield(a.Overshield);
	ret->set_bdisplayovershield(a.bDisplayOvershield);
	ret->set_level(a.Level);
	ret->set_bdisplaylevel(a.bDisplayLevel);
	ret->set_respawntime(a.RespawnTime);
	ret->set_bdisplayrespawntime(a.bDisplayRespawnTime);
	ret->set_bdisplayname(a.bDisplayName);
	ret->set_allocated_location(DumpFVector(a.Location, channels));
	ret->set_bactive(a.bActive);
	ret->set_allocated_unittype(ObjectToObjectReference(a.UnitType));
	ret->set_allocated_pri(ReplicatedActorToReplicatedActorReference(a.PRI, channels));
	ret->set_binjured(a.bInjured);
	ret->set_allocated_nameid(ObjectToObjectReference(a.NameID));
	ret->set_name(a.Name.ToString());
}

HederaDefinitions::APoplarPlayerController* Dumper::DumpAPoplarPlayerController(APoplarPlayerController* a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::APoplarPlayerController* ret = new HederaDefinitions::APoplarPlayerController();

	ret->set_allocated_location(DumpFVector(a->Location, channels));
	ret->set_allocated_rotation(DumpFRotator(a->Rotation, channels));
	ret->set_drawscale(a->DrawScale);
	ret->set_customtimedilation(a->CustomTimeDilation);
	ret->set_physics((uint32_t)a->Physics);
	ret->set_remoterole((uint32_t)a->RemoteRole);
	ret->set_role((uint32_t)a->Role);
	ret->set_replicatedcollisiontype((uint32_t)a->ReplicatedCollisionType);
	ret->set_allocated_owner(ReplicatedActorToReplicatedActorReference(a->Owner, channels));
	ret->set_allocated_base(ReplicatedActorToReplicatedActorReference(a->Base, channels));
	ret->set_bhidden(a->bHidden);
	ret->set_btearoff(a->bTearOff);
	ret->set_bhardattach(a->bHardAttach);
	ret->set_bcollideactors(a->bCollideActors);
	ret->set_bcollideworld(a->bCollideWorld);
	ret->set_bblockactors(a->bBlockActors);
	ret->set_bprojtarget(a->bProjTarget);
	ret->set_bnetowner(a->bNetOwner);
	ret->set_allocated_instigator(ReplicatedActorToReplicatedActorReference(a->Instigator, channels));
	ret->set_allocated_velocity(DumpFVector(a->Velocity, channels));
	ret->set_allocated_basebonename(DumpFName(a->BaseBoneName, channels));
	ret->set_allocated_relativelocation(DumpFVector(a->RelativeLocation, channels));
	ret->set_allocated_relativerotation(DumpFRotator(a->RelativeRotation, channels));
	ret->set_allocated_pawn(ReplicatedActorToReplicatedActorReference(a->Pawn, channels));
	ret->set_allocated_clientpendingpsi(ReplicatedActorToReplicatedActorReference(a->ClientPendingPSI, channels));
	ret->set_allocated_clientpendingpri(ReplicatedActorToReplicatedActorReference(a->ClientPendingPRI, channels));
	ret->set_allocated_targetviewrotation(DumpFRotator(a->TargetViewRotation, channels));
	ret->set_targeteyeheight(a->TargetEyeHeight);
	ret->set_allocated_activeaudioeffect(DumpFEnvironmentalEffectInfo(a->ActiveAudioEffect, channels));
	ret->set_bhideammoonhud(a->bHideAmmoOnHUD);
	ret->set_bhidecompassonhud(a->bHideCompassOnHUD);
	ret->set_bhideskillsonhud(a->bHideSkillsOnHUD);
	ret->set_bhidexponhud(a->bHideXPOnHUD);
	ret->set_bconsolecontroller(a->bConsoleController);
	ret->set_bdisplaytradeicon(a->bDisplayTradeIcon);
	ret->set_bdisplaytradedisabledicon(a->bDisplayTradeDisabledIcon);
	ret->set_bresurrectall(a->bResurrectAll);
	ret->set_bplayingteleportersound(a->bPlayingTeleporterSound);
	ret->set_allocated_mywillowpawn(ReplicatedActorToReplicatedActorReference(a->MyWillowPawn, channels));
	ret->set_allocated_currentusableobject_object(ObjectToObjectReference(a->CurrentUsableObject_Object));
	ret->set_allocated_currentusableobject_interface(ObjectToObjectReference(a->CurrentUsableObject_Interface));
	
	for (FInteractionIconWithOverrides icon : a->CurrentInteractionIcon) {
		HederaDefinitions::FInteractionIconWithOverrides* allocIcon = ret->add_currentinteractionicon();
		DumpFInteractionIconWithOverrides(icon, channels, allocIcon);
	}

	ret->set_allocated_recentdamagescreenparticle(DumpFReplicatedDamageScreenParticle(a->RecentDamageScreenParticle, channels));
	ret->set_fovmodifierspeed(a->FOVModifierSpeed);
	ret->set_fovmodifierspeedbasevalue(a->FOVModifierSpeedBaseValue);
	ret->set_postprocessdurationmultiplier(a->PostProcessDurationMultiplier);
	ret->set_allocated_useprogressbar(DumpFReplicatedProgressBarData(a->UseProgressBar, channels));
	ret->set_targetedwarningseverity(a->TargetedWarningSeverity);
	ret->set_allocated_replicatedcameraviewdata(DumpFCameraViewData(a->ReplicatedCameraViewData, channels));
	ret->set_allocated_pipelinesummaryhistorymanager(ReplicatedActorToReplicatedActorReference(a->PipelineSummaryHistoryManager, channels));
	
	for (FActorSelectionDataContainer container : a->MinimapActorSelectData) {
		HederaDefinitions::FActorSelectionDataContainer* allocContainer = ret->add_minimapactorselectdata();
		DumpFActorSelectionDataContainer(container, channels, allocContainer);
	}

	ret->set_currentminimapactorselectionindex(a->CurrentMinimapActorSelectionIndex);

	return ret;
}

std::map<std::string, UObject*> Parser::cacheMap;

UObject* Parser::ObjectReferenceToObject(const HederaDefinitions::NonReplicatedObjectReference& obj, TArray<UObject*>* GObjects, UClass* className) {
	if (obj.fullnamelookup() == "null" || obj.fullnamelookup() == "") {
		//std::cout << "[HEDERA] Null object ref, null value " << obj.fullnamelookup() << std::endl;
		return nullptr;
	}

	auto cachedValue = cacheMap.find(obj.fullnamelookup());

	if (cachedValue != cacheMap.end()) {
		return cacheMap[obj.fullnamelookup()];
	}

	for (UObject* cmpObj : *GObjects) {
		if (cmpObj) {
			if (cmpObj->IsA(className)) {
				if (cmpObj->GetFullName() == obj.fullnamelookup()) {
					//std::cout << "[HEDERA] found object value " << obj.fullnamelookup() << std::endl;
					cacheMap.insert(std::pair<std::string, UObject*>(obj.fullnamelookup(), cmpObj));
					return cmpObj;
				}
			}
		}
	}

	//std::cout << "[HEDERA] Null object ref, couldn't find obj " << obj.fullnamelookup() << std::endl;
	return nullptr;
}

AActor* Parser::ReplicatedActorReferenceToReplicatedActor(const HederaDefinitions::ReplicatedActorReference& actor, std::vector<UActorChannel*>& channels) {
	if (actor.actorid() == 0) {
		//std::cout << "[HEDERA] Null actor ref, null passed in" << std::endl;
		return nullptr;
	}
	
	for (UActorChannel* channel : channels) {
		if (channel->ChIndex == actor.actorid()) {
			
			return channel->Actor;
		}
	}

	//std::cout << "[HEDERA] Null actor ref, couldn't find actor" << std::endl;
	return nullptr;
}

FRotator Parser::ParseFRotator(HederaDefinitions::FRotator a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FRotator out = FRotator();

	out.Pitch = a.pitch();
	out.Yaw = a.yaw();
	out.Roll = a.roll();

	return out;
}

FVector Parser::ParseFVector(HederaDefinitions::FVector a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FVector out = FVector();

	out.X = a.x();
	out.Y = a.y();
	out.Z = a.z();

	return out;
}

FName Parser::ParseFName(HederaDefinitions::FName a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FName out = FName();

	out.FNameEntryId = a.fnameentryid();
	out.InstanceNumber = a.instancenumber();

	return out;
}

FEnvironmentalEffectInfo Parser::ParseFEnvironmentalEffectInfo(HederaDefinitions::FEnvironmentalEffectInfo a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FEnvironmentalEffectInfo out = FEnvironmentalEffectInfo();

	out.AuxBusVolume = a.auxbusvolume();
	out.EffectID = a.effectid();
	out.OutputBusVolume = a.outputbusvolume();

	return out;
}

FAttributeDefinitionDefault Parser::ParseFAttributeDefinitionDefault(HederaDefinitions::FAttributeDefinitionDefault a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAttributeDefinitionDefault out = FAttributeDefinitionDefault();

	out.Attribute = (UAttributeDefinition*)ObjectReferenceToObject(a.attribute(), objects, UAttributeDefinition::StaticClass());
	out.DefaultValue = a.defaultvalue();

	return out;
}

FAttributeScalarParam Parser::ParseFAttributeScalarParam(HederaDefinitions::FAttributeScalarParam a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAttributeScalarParam out = FAttributeScalarParam();

	out.bClampInput = a.bclampinput();
	out.bVectorParam = a.bvectorparam();
	out.CurrentValue = a.currentvalue();
	out.Input = ParseFAttributeDefinitionDefault(a.input(), channels, objects);
	out.MaxInput = ParseFAttributeDefinitionDefault(a.maxinput(), channels, objects);
	out.MaxOutput = ParseFAttributeDefinitionDefault(a.maxoutput(), channels, objects);
	out.MinInput = ParseFAttributeDefinitionDefault(a.mininput(), channels, objects);
	out.MinOutput = ParseFAttributeDefinitionDefault(a.minoutput(), channels, objects);
	out.ParamName = ParseFName(a.paramname(), channels, objects);
	out.ScalarParamOperand = a.scalarparamoperand();

	return out;
}

FScreenParticleModifier Parser::ParseFScreenParticleModifier(HederaDefinitions::FScreenParticleModifier a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FScreenParticleModifier out = FScreenParticleModifier();

	out.CachedValue = a.cachedvalue();

	TArray<FAttributeScalarParam> scalarParams = TArray<FAttributeScalarParam>();

	for (int i = 0; i < a.scalarparams_size(); i++) {
		scalarParams.push_back(ParseFAttributeScalarParam(a.scalarparams()[i], channels, objects));
	}

	out.ScalarParams = scalarParams;

	return out;
}

FInteractionIconWithOverrides Parser::ParseFInteractionIconWithOverrides(HederaDefinitions::FInteractionIconWithOverrides a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FInteractionIconWithOverrides out = FInteractionIconWithOverrides();

	out.bCostsToUse = a.bcoststouse();
	out.bOverrideAction = a.boverrideaction();
	out.bOverrideIcon = a.boverrideicon();
	out.bOverrideText = a.boverridetext();
	out.CostsAmount = a.costsamount();
	out.CostsCurrencyType = a.costscurrencytype();
	out.IconDef = (UInteractionIconDefinition*)ObjectReferenceToObject(a.icondef(), objects, UInteractionIconDefinition::StaticClass());
	out.OverrideIconDef = (UInteractionIconDefinition*)ObjectReferenceToObject(a.overrideicondef(), objects, UInteractionIconDefinition::StaticClass());

	return out;
}

FVector2D Parser::ParseFVector2D(HederaDefinitions::FVector2D a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FVector2D out = FVector2D();

	out.X = a.x();
	out.Y = a.y();

	return out;
}

FScreenParticleScalarParamOverTime Parser::ParseFScreenParticleScalarParamOverTime(HederaDefinitions::FScreenParticleScalarParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FScreenParticleScalarParamOverTime out = FScreenParticleScalarParamOverTime();

	out.EndValue = a.endvalue();
	out.LastValue = a.lastvalue();
	out.ParameterName = ParseFName(a.parametername(), channels, objects);
	out.StartValue = a.startvalue();
	out.TotalTime = a.totaltime();

	return out;
}

FScreenParticleVectorParamOverTime Parser::ParseFScreenParticleVectorParamOverTime(HederaDefinitions::FScreenParticleVectorParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FScreenParticleVectorParamOverTime out = FScreenParticleVectorParamOverTime();

	out.LastValue = ParseFVector(a.lastvalue(), channels, objects);
	out.ParameterName = ParseFName(a.parametername(), channels, objects);
	out.TotalTime = a.totaltime();
	out.VectorEndValue = ParseFVector(a.vectorendvalue(), channels, objects);
	out.VectorStartValue = ParseFVector(a.vectorstartvalue(), channels, objects);

	return out;
}

FScreenParticleParamOverTime Parser::ParseFScreenParticleParamOverTime(HederaDefinitions::FScreenParticleParamOverTime a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FScreenParticleParamOverTime out = FScreenParticleParamOverTime();

	TArray<FScreenParticleScalarParamOverTime> scalarParams = TArray<FScreenParticleScalarParamOverTime>();

	for (int i = 0; i < a.scalarparams_size(); i++) {
		scalarParams.push_back(ParseFScreenParticleScalarParamOverTime(a.scalarparams()[i], channels, objects));
	}

	out.ScalarParams = scalarParams;

	TArray<FScreenParticleVectorParamOverTime> vectorParams = TArray<FScreenParticleVectorParamOverTime>();

	for (int i = 0; i < a.vectorparams_size(); i++) {
		vectorParams.push_back(ParseFScreenParticleVectorParamOverTime(a.vectorparams()[i], channels, objects));
	}

	out.VectorParams = vectorParams;

	return out;
}

FScreenParticleInitParams Parser::ParseFScreenParticleInitParams(HederaDefinitions::FScreenParticleInitParams a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FScreenParticleInitParams out = FScreenParticleInitParams();

	out.bAlwaysVisible = a.balwaysvisible();
	out.bHideWhenFinished = a.bhidewhenfinished();
	out.bOnlyOwnerSee = a.bonlyownersee();
	out.bUseDesignerAttributeParameters = a.busedesignerattributeparameters();
	out.ContentDims = ParseFVector2D(a.contentdims(), channels, objects);
	out.EffectDepthPriorityGroup = a.effectdepthprioritygroup();
	out.MatParamName = ParseFName(a.matparamname(), channels, objects);
	out.ParticleDepth = a.particledepth();
	out.ParticleTag = ParseFName(a.particletag(), channels, objects);
	out.ScalingMode = a.scalingmode();

	TArray<FScreenParticleModifier> screen = TArray<FScreenParticleModifier>();

	for (int i = 0; i < a.screenparticlemodifiers_size(); i++) {
		screen.push_back(ParseFScreenParticleModifier(a.screenparticlemodifiers()[i], channels, objects));
	}

	out.ScreenParticleModifiers = screen;

	out.StopParamsOT = ParseFScreenParticleParamOverTime(a.stopparamsot(), channels, objects);
	out.Template = (UParticleSystem*)ObjectReferenceToObject(a.template_(), objects, UParticleSystem::StaticClass());
	out.TemplateScreenParticleMaterial = (UMaterialInstance*)ObjectReferenceToObject(a.templatescreenparticlematerial(), objects, UMaterialInstance::StaticClass());

	return out;
}

FReplicatedDamageScreenParticle Parser::ParseFReplicatedDamageScreenParticle(HederaDefinitions::FReplicatedDamageScreenParticle a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedDamageScreenParticle out = FReplicatedDamageScreenParticle();

	out.Counter = a.counter();
	out.GenericScale = a.genericscale();
	out.Params = ParseFScreenParticleInitParams(a.params(), channels, objects);
	out.ReferenceLocation = ParseFVector(a.referenceloaction(), channels, objects);

	return out;
}

FReplicatedProgressBarData Parser::ParseFReplicatedProgressBarData(HederaDefinitions::FReplicatedProgressBarData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedProgressBarData out = FReplicatedProgressBarData();

	out.Duration = a.duration();
	out.Rate = a.rate();
	out.StartOffset = a.rate();

	return out;
}

FCameraViewData Parser::ParseFCameraViewData(HederaDefinitions::FCameraViewData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FCameraViewData out = FCameraViewData();

	out.bIsTeleport = a.bisteleport();
	out.BlendTimeOverride = a.blendtimeoverride();
	out.DesiredDeathSpectatorState = a.desireddeathspectatorstate();
	out.ModeName = ParseFName(a.modename(), channels, objects);
	out.ViewTarget = ReplicatedActorReferenceToReplicatedActor(a.viewtarget(), channels);

	return out;
}

FActorSelectionDataContainer Parser::ParseFActorSelectionDataContainer(HederaDefinitions::FActorSelectionDataContainer a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FActorSelectionDataContainer out = FActorSelectionDataContainer();

	out.bActive = a.bactive();
	out.bDisplayHealth = a.bdisplayhealth();
	out.bDisplayLevel = a.bdisplaylevel();
	out.bDisplayName = a.bdisplayname();
	out.bDisplayOvershield = a.bdisplayovershield();
	out.bDisplayRespawnTime = a.bdisplayrespawntime();
	out.bDisplayShield = a.bdisplayshield();
	out.bInjured = a.binjured();
	out.Health = a.health();
	out.Level = a.level();
	out.Location = ParseFVector(a.location(), channels, objects);
	out.Name = FString(a.name().c_str());
	out.NameID = (UPlayerNameIdentifierDefinition*)ObjectReferenceToObject(a.nameid(), objects, UPlayerNameIdentifierDefinition::StaticClass());
	out.Overshield = a.overshield();
	out.PRI = (APlayerReplicationInfo*)ReplicatedActorReferenceToReplicatedActor(a.pri(), channels);
	out.RespawnTime = a.respawntime();
	out.Shield = a.shield();
	out.UnitType = (UPoplarUnitTypeDefinition*)ObjectReferenceToObject(a.unittype(), objects, UPoplarUnitTypeDefinition::StaticClass());

	return out;
}

void Parser::ParseAPoplarPlayerController(HederaDefinitions::APoplarPlayerController a, APoplarPlayerController* b, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	b->Location = ParseFVector(a.location(), channels, objects);
	b->Rotation = ParseFRotator(a.rotation(), channels, objects);
	b->DrawScale = a.drawscale();
	b->CustomTimeDilation = a.customtimedilation();
	b->Physics = (EPhysics)a.physics();
	b->RemoteRole = (ENetRole)a.remoterole();
	b->Role = (ENetRole)a.role();
	b->ReplicatedCollisionType = (ECollisionType)a.replicatedcollisiontype();
	b->Owner = ReplicatedActorReferenceToReplicatedActor(a.owner(), channels);
	b->Base = ReplicatedActorReferenceToReplicatedActor(a.base(), channels);
	b->bHidden = a.bhidden();
	b->bTearOff = a.btearoff();
	b->bHardAttach = a.bhardattach();
	b->bCollideActors = a.bcollideactors();
	b->bCollideWorld = a.bcollideworld();
	b->bBlockActors = a.bblockactors();
	b->bProjTarget = a.bprojtarget();
	b->bNetOwner = a.bnetowner();
	b->Instigator = (APawn*)ReplicatedActorReferenceToReplicatedActor(a.instigator(), channels);
	b->Velocity = ParseFVector(a.velocity(), channels, objects);
	b->BaseBoneName = ParseFName(a.basebonename(), channels, objects);
	b->RelativeLocation = ParseFVector(a.relativelocation(), channels, objects);
	b->RelativeRotation = ParseFRotator(a.relativerotation(), channels, objects);
	b->Pawn = (APawn*)ReplicatedActorReferenceToReplicatedActor(a.pawn(), channels);
	b->ClientPendingPSI = (APlayerStateInfo*)ReplicatedActorReferenceToReplicatedActor(a.clientpendingpsi(), channels);
	b->ClientPendingPRI = (APlayerReplicationInfo*)ReplicatedActorReferenceToReplicatedActor(a.clientpendingpri(), channels);
	b->TargetViewRotation = ParseFRotator(a.targetviewrotation(), channels, objects);
	b->TargetEyeHeight = a.targeteyeheight();
	b->ActiveAudioEffect = ParseFEnvironmentalEffectInfo(a.activeaudioeffect(), channels, objects);
	b->bHideAmmoOnHUD = a.bhideammoonhud();
	b->bHideCompassOnHUD = a.bhidecompassonhud();
	b->bHideSkillsOnHUD = a.bhideskillsonhud();
	b->bHideXPOnHUD = a.bhidexponhud();
	b->bConsoleController = a.bconsolecontroller();
	b->bDisplayTradeIcon = a.bdisplaytradeicon();
	b->bDisplayTradeDisabledIcon = a.bdisplaytradedisabledicon();
	b->bResurrectAll = a.bresurrectall();
	b->bPlayingTeleporterSound = a.bplayingteleportersound();
	b->MyWillowPawn = (AWillowPawn*)ReplicatedActorReferenceToReplicatedActor(a.mywillowpawn(), channels);
	b->CurrentUsableObject_Object = (UIUsable*)ObjectReferenceToObject(a.currentusableobject_object(), objects, UIUsable::StaticClass());
	b->CurrentUsableObject_Interface = (UIUsable*)ObjectReferenceToObject(a.currentusableobject_interface(), objects, UIUsable::StaticClass());

	if(a.currentinteractionicon_size() > 0)
		b->CurrentInteractionIcon[0] = ParseFInteractionIconWithOverrides(a.currentinteractionicon()[0], channels, objects);

	if (a.currentinteractionicon_size() > 1)
		b->CurrentInteractionIcon[1] = ParseFInteractionIconWithOverrides(a.currentinteractionicon()[1], channels, objects);
	
	b->RecentDamageScreenParticle = ParseFReplicatedDamageScreenParticle(a.recentdamagescreenparticle(), channels, objects);
	b->FOVModifierSpeed = a.fovmodifierspeed();
	b->FOVModifierSpeedBaseValue = a.fovmodifierspeedbasevalue();
	b->PostProcessDurationMultiplier = a.postprocessdurationmultiplier();
	b->UseProgressBar = ParseFReplicatedProgressBarData(a.useprogressbar(), channels, objects);
	b->TargetedWarningSeverity = a.targetedwarningseverity();
	b->ReplicatedCameraViewData = ParseFCameraViewData(a.replicatedcameraviewdata(), channels, objects);
	b->PipelineSummaryHistoryManager = (APipelineHistoryManager*)ReplicatedActorReferenceToReplicatedActor(a.pipelinesummaryhistorymanager(), channels);

	if (a.minimapactorselectdata_size() > 0)
		b->MinimapActorSelectData[0] = ParseFActorSelectionDataContainer(a.minimapactorselectdata()[0], channels, objects);

	if (a.minimapactorselectdata_size() > 1)
		b->MinimapActorSelectData[1] = ParseFActorSelectionDataContainer(a.minimapactorselectdata()[1], channels, objects);

	if (a.minimapactorselectdata_size() > 2)
		b->MinimapActorSelectData[2] = ParseFActorSelectionDataContainer(a.minimapactorselectdata()[2], channels, objects);

	if (a.minimapactorselectdata_size() > 3)
		b->MinimapActorSelectData[3] = ParseFActorSelectionDataContainer(a.minimapactorselectdata()[3], channels, objects);

	b->CurrentMinimapActorSelectionIndex = a.currentminimapactorselectionindex();
}

HederaDefinitions::FUniqueID* Dumper::DumpFUniqueID(FUniqueNetId a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FUniqueID* ret = new HederaDefinitions::FUniqueID();

	for (int i = 0; i < 0x10; i++) {
		ret->add_rawid(a.RawId[i]);
	}

	ret->set_bhasvalue(a.bHasValue);

	return ret;
}

void Dumper::DumpFCurrencyState(FCurrencyState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FCurrencyState* ret) {
	ret->set_currentamount(a.CurrentAmount);
	ret->set_lastknownamount(a.LastKnownAmount);
	ret->set_type(a.FormOfCurrency);
}

HederaDefinitions::FResourcePoolReference* Dumper::DumpFResourcePoolReference(FResourcePoolReference a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FResourcePoolReference* ret = new HederaDefinitions::FResourcePoolReference();

	ret->set_allocated_data(ObjectToObjectReference(a.Data));
	ret->set_allocated_poolmanager(ReplicatedActorToReplicatedActorReference(a.PoolManager, channels));
	ret->set_poolguid(a.PoolGUID);
	ret->set_poolindexinmanager(a.PoolIndexInManager);

	return ret;
}

void Dumper::DumpFReplicatedNounAttributeState(FReplicatedNounAttributeState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FReplicatedNounAttributeState* ret) {
	ret->set_allocated_nounname(DumpFName(a.NounName, channels));
	ret->set_bonlyrelevanttoowner(a.bOnlyRelevantToOwner);
	ret->set_value(a.Value);
}

HederaDefinitions::FWeaponDefinitionData* Dumper::DumpFWeaponDefinitionData(FWeaponDefinitionData a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FWeaponDefinitionData* ret = new HederaDefinitions::FWeaponDefinitionData();

	ret->set_allocated_weapontypedefinition(ObjectToObjectReference(a.WeaponTypeDefinition));

	return ret;
}

HederaDefinitions::FReplicatedStandInGear* Dumper::DumpFReplicatedStandInGear(FReplicatedStandInGear a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FReplicatedStandInGear* ret = new HederaDefinitions::FReplicatedStandInGear();

	ret->set_allocated_weapon1data(DumpFWeaponDefinitionData(a.Weapon1Data, channels));
	ret->set_allocated_weapon2data(DumpFWeaponDefinitionData(a.Weapon2Data, channels));

	return ret;
}

void Dumper::DumpFTrackedSkillState(FTrackedSkillState a, std::vector<UActorChannel*>& channels, HederaDefinitions::FTrackedSkillState* ret) {
	ret->set_allocated_skilldef(ObjectToObjectReference(a.SkillDef));
	ret->set_packedstackcountandpercentcomplete(a.PackedStackCountAndPercentComplete);
}

void Dumper::DumpFManagedActorCountData(FManagedActorCountData a, std::vector<UActorChannel*>& channels, HederaDefinitions::FManagedActorCountData* ret) {
	ret->set_allocated_type(ObjectToObjectReference(a.Type));
	ret->set_count(a.Count);
}

void Dumper::DumpFReplicatedPerkItem(FReplicatedPerkItem a, std::vector<UActorChannel*>& channels, HederaDefinitions::FReplicatedPerkItem* ret) {
	ret->set_metaid(a.MetaID.ToString());
	ret->set_assetkey(a.AssetKey.ToString());
	ret->set_bcanuse(a.bCanUse);
	ret->set_itemlevel(a.ItemLevel);
	ret->set_rarity(a.Rarity);
	ret->set_allocated_playerid(DumpFUniqueID(a.PlayerID, channels));
	ret->set_apollojson(a.ApolloJSON.ToString());
	ret->set_entitlementindex(a.EntitlementIndex);
	ret->set_bactive(a.bActive);
	ret->set_allocated_perkfunction(ObjectToObjectReference(a.PerkFunction));
	ret->set_allocated_wearable(ObjectToObjectReference(a.Wearable));
}

HederaDefinitions::FAugInstance* Dumper::DumpFAugInstance(FAugInstance a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FAugInstance* ret = new HederaDefinitions::FAugInstance();

	ret->set_allocated_augdef(ObjectToObjectReference(a.AugDef));
	ret->set_currenttier(a.CurrentTier);
	ret->set_clientpurchasestatus(a.ClientPurchaseStatus);
	ret->set_preselected(a.Preselected);

	return ret;
}

void Dumper::DumpFAugInstancePassedIn(FAugInstance a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAugInstance* ret) {
	ret->set_allocated_augdef(ObjectToObjectReference(a.AugDef));
	ret->set_currenttier(a.CurrentTier);
	ret->set_clientpurchasestatus(a.ClientPurchaseStatus);
	ret->set_preselected(a.Preselected);
}

void Dumper::DumpFAugCategoryInstance(FAugCategoryInstance a, std::vector<UActorChannel*>& channels, HederaDefinitions::FAugCategoryInstance* ret) {
	ret->set_allocated_categorydef(ObjectToObjectReference(a.CategoryDef));
	HederaDefinitions::FAugInstance* aug1 = ret->add_augs();
	DumpFAugInstancePassedIn(a.Augs[0], channels, aug1);
	HederaDefinitions::FAugInstance* aug2 = ret->add_augs();
	DumpFAugInstancePassedIn(a.Augs[1], channels, aug2);
	ret->set_allocated_mutation(DumpFAugInstance(a.Mutation, channels));
	ret->set_categoryisunlocked(a.CategoryIsUnlocked);
}

HederaDefinitions::FAugSetInstance* Dumper::DumpFAugSetInstance(FAugSetInstance a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FAugSetInstance* ret = new HederaDefinitions::FAugSetInstance();

	ret->set_allocated_augsetdef(ObjectToObjectReference(a.AugSetDef));
	for (int i = 0; i < 10; i++) {
		HederaDefinitions::FAugCategoryInstance* aug = ret->add_allcategories();
		DumpFAugCategoryInstance(a.AllCategories[i], channels, aug);
	}
	
	return ret;
}

HederaDefinitions::FReplicatedBehaviorConsumerState* Dumper::DumpFReplicatedBehaviorConsumerState(FReplicatedBehaviorConsumerState a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FReplicatedBehaviorConsumerState* ret = new HederaDefinitions::FReplicatedBehaviorConsumerState();

	ret->set_behaviorsequenceenabledflags(a.BehaviorSequenceEnabledFlags);
	ret->set_bisprocesssuspended(a.bIsProcessSuspended);
	ret->set_bneedtoapplythisstate(a.bNeedToApplyThisState);

	return ret;
}

HederaDefinitions::FReplicatedBehaviorEvent* Dumper::DumpFReplicatedBehaviorEvent(FReplicatedBehaviorEvent a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FReplicatedBehaviorEvent* ret = new HederaDefinitions::FReplicatedBehaviorEvent();

	ret->set_sequenceindex(a.SequenceIndex);
	ret->set_eventcounter(a.EventCounter);
	ret->set_eventindex(a.EventIndex);
	ret->set_eventoutput(a.EventOutput);
	ret->set_allocated_instigator(ObjectToObjectReference(a.Instigator));

	return ret;
}

HederaDefinitions::FCinematicModeOptions* Dumper::DumpFCinematicModeOptions(FCinematicModeOptions a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::FCinematicModeOptions* ret = new HederaDefinitions::FCinematicModeOptions();

	ret->set_applicationmode(a.ApplicationMode);
	ret->set_baffectsaudio(a.bAffectsAudio);
	ret->set_baffectsbuttons(a.bAffectsButtons);
	ret->set_baffectsgodmode(a.bAffectsGodMode);
	ret->set_baffectshud(a.bAffectsHUD);
	ret->set_baffectsmovement(a.bAffectsMovement);
	ret->set_baffectsnotarget(a.bAffectsNoTarget);
	ret->set_baffectsskills(a.bAffectsSkills);
	ret->set_baffectsturning(a.bAffectsTurning);
	ret->set_ballowcinematicsplitscreen(a.bAllowCinematicSplitScreen);
	ret->set_bcinematicmode(a.bCinematicMode);
	ret->set_bhideoutlines(a.bHideOutlines);
	ret->set_bhideplayer(a.bHidePlayer);
	ret->set_bhidescreenparticles(a.bHideScreenParticles);
	ret->set_bkismetsetcinematicmode(a.bKismetSetCinematicMode);
	ret->set_modetoken(a.ModeToken);

	return ret;
}

HederaDefinitions::APoplarPlayerReplicationInfo* Dumper::DumpAPoplarPlayerReplicationInfo(APoplarPlayerReplicationInfo* a, std::vector<UActorChannel*>& channels) {
	HederaDefinitions::APoplarPlayerReplicationInfo* ret = new HederaDefinitions::APoplarPlayerReplicationInfo();

	ret->set_allocated_location(DumpFVector(a->Location, channels));
	ret->set_allocated_rotation(DumpFRotator(a->Rotation, channels));
	ret->set_drawscale(a->DrawScale);
	ret->set_customtimedilation(a->CustomTimeDilation);
	ret->set_physics((uint32_t)a->Physics);
	ret->set_remoterole((uint32_t)a->RemoteRole);
	ret->set_role((uint32_t)a->Role);
	ret->set_replicatedcollisiontype((uint32_t)a->ReplicatedCollisionType);
	ret->set_allocated_owner(ReplicatedActorToReplicatedActorReference(a->Owner, channels));
	ret->set_allocated_base(ReplicatedActorToReplicatedActorReference(a->Base, channels));
	ret->set_bhidden(a->bHidden);
	ret->set_btearoff(a->bTearOff);
	ret->set_bhardattach(a->bHardAttach);
	ret->set_bcollideactors(a->bCollideActors);
	ret->set_bcollideworld(a->bCollideWorld);
	ret->set_bblockactors(a->bBlockActors);
	ret->set_bprojtarget(a->bProjTarget);
	ret->set_bnetowner(a->bNetOwner);
	ret->set_allocated_instigator(ReplicatedActorToReplicatedActorReference(a->Instigator, channels));
	ret->set_allocated_velocity(DumpFVector(a->Velocity, channels));
	ret->set_deaths(a->Deaths);
	ret->set_ping(a->Ping);
	ret->set_playername(a->PlayerName.ToString());
	ret->set_badmin(a->bAdmin);
	ret->set_bisspectator(a->bIsSpectator);
	ret->set_bonlyspectator(a->bOnlySpectator);
	ret->set_boutoflives(a->bOutOfLives);
	ret->set_bbot(a->bBot);
	ret->set_bfrompreviouslevel(a->bFromPreviousLevel);
	ret->set_bplayerafk(a->bPlayerAFK);
	ret->set_playerid(a->PlayerID);
	ret->set_allocated_team(ReplicatedActorToReplicatedActorReference(a->Team, channels));
	ret->set_starttime(a->StartTime);
	ret->set_kills(a->Kills);
	ret->set_allocated_uniqueid(DumpFUniqueID(a->UniqueId, channels));
	ret->set_explevel(a->ExpLevel);
	ret->set_exppointsnextlevelat(a->ExpPointsNextLevelAt);
	ret->set_exppointsnextlevelatbasevalue(a->ExpPointsNextLevelAtBaseValue);
	ret->set_bisdead(a->bIsDead);
	ret->set_bhideonhudifenemy(a->bHideOnHUDIfEnemy);
	ret->set_generalskillpoints(a->GeneralSkillPoints);
	ret->set_specialistskillpoints(a->SpecialistSkillPoints);
	ret->set_currentinstanthitcriticalhitbonus(a->CurrentInstantHitCriticalHitBonus);
	ret->set_currentinstanthitcriticalhitbonusbasevalue(a->CurrentInstantHitCriticalHitBonusBaseValue);
	
	for (int i = 0; i < 13; i++) {
		HederaDefinitions::FCurrencyState* currency = ret->add_currency();
		DumpFCurrencyState(a->Currency[i], channels, currency);
	}

	ret->set_allocated_healthpool(DumpFResourcePoolReference(a->HealthPool, channels));
	ret->set_allocated_shieldarmor(DumpFResourcePoolReference(a->ShieldArmor, channels));
	ret->set_allocated_pawnallegiance(ObjectToObjectReference(a->PawnAllegiance));
	ret->set_allocated_characternameiddef(ObjectToObjectReference(a->CharacterNameIdDef));
	ret->set_bgfxmenuopen(a->bGFxMenuOpen);
	ret->set_numtrackedskillslotsinuse(a->NumTrackedSkillSlotsInUse);
	ret->set_highestcompletedplaythrough(a->HighestCompletedPlayThrough);
	ret->set_numreplicatednouns(a->NumReplicatedNouns);
	
	for (int i = 0; i < 20; i++) {
		HederaDefinitions::FReplicatedNounAttributeState* noun = ret->add_replicatednouns();
		DumpFReplicatedNounAttributeState(a->ReplicatedNouns[i], channels, noun);
	}

	ret->set_allocated_standingear(DumpFReplicatedStandInGear(a->StandInGear, channels));
	
	for (int i = 0; i < 17; i++) {
		HederaDefinitions::FTrackedSkillState* skill = ret->add_trackedskills();
		DumpFTrackedSkillState(a->TrackedSkills[i], channels, skill);
	}

	ret->set_allocated_poplarpsi(ReplicatedActorToReplicatedActorReference(a->PoplarPSI, channels));
	ret->set_bnpc(a->bNPC);
	ret->set_bdigitaldelux(a->bDigitalDelux);

	ret->set_bplaybrowsingtaunt(a->bPlayBrowsingTaunt);
	ret->set_bispvematch(a->bIsPvEMatch);
	ret->set_bneedstocastsurrendervote(a->bNeedsToCastSurrenderVote);
	ret->set_allocated_botownerpri(ReplicatedActorToReplicatedActorReference(a->BotOwnerPRI, channels));
	ret->set_botdifficulty(a->BotDifficulty);
	ret->set_allocated_myalertpoint(ReplicatedActorToReplicatedActorReference(a->MyAlertPoint, channels));
	ret->set_allocated_energypool(DumpFResourcePoolReference(a->EnergyPool, channels));
	ret->set_allocated_unittypedefinition(ObjectToObjectReference(a->UnitTypeDefinition));
	ret->set_allocated_statreplicationinfo(ReplicatedActorToReplicatedActorReference(a->StatReplicationInfo, channels));
	
	for (int i = 0; i < 5; i++) {
		HederaDefinitions::ReplicatedActorReference* ref = ReplicatedActorToReplicatedActorReference(a->NemesisPRIArray[i], channels);
		HederaDefinitions::ReplicatedActorReference* addRef = ret->add_nemesispriarray();
		addRef->set_actorid(ref->actorid());
	}

	for (int i = 0; i < 3; i++) {
		HederaDefinitions::FManagedActorCountData* actor = ret->add_managedactorcounts();
		DumpFManagedActorCountData(a->ManagedActorCounts[i], channels, actor);
	}

	ret->set_replicatedrespawntimeremaining(a->ReplicatedRespawnTimeRemaining);
	ret->set_characterselectionstate((uint32_t)a->CharacterSelectionState);
	
	for (int i = 0; i < 3; i++) {
		ret->add_activatedperkflags(a->ActivatedPerkFlags[i]);
	}

	for (int i = 0; i < 3; i++) {
		HederaDefinitions::FReplicatedPerkItem* perk = ret->add_perks();
		DumpFReplicatedPerkItem(a->Perks[i], channels, perk);
	}

	ret->set_overshieldstrength(a->OvershieldStrength);
	ret->set_maxovershieldvalue(a->MaxOvershieldValue);
	ret->set_allocated_browsecharacternameiddef(ObjectToObjectReference(a->BrowseCharacterNameIdDef));
	ret->set_allocated_browseskiniddef(ObjectToObjectReference(a->BrowseSkinIdDef));
	ret->set_allocated_skiniddef(ObjectToObjectReference(a->SkinIdDef));
	ret->set_allocated_augs(DumpFAugSetInstance(a->Augs, channels));
	ret->set_allocated_replicatedbehaviorconsumerstate(DumpFReplicatedBehaviorConsumerState(a->ReplicatedBehaviorConsumerState, channels));
	ret->set_allocated_replicatedbehaviorevent(DumpFReplicatedBehaviorEvent(a->ReplicatedBehaviorEvent, channels));
	ret->set_allocated_replicatedcinematicmodeoptions(DumpFCinematicModeOptions(a->ReplicatedCinematicModeOptions, channels));
	
	for (int i = 0; i < 3; i++) {
		HederaDefinitions::NonReplicatedObjectReference* ref = ObjectToObjectReference(a->CurrentWearableDefinitions[i]);
		HederaDefinitions::NonReplicatedObjectReference* addRef = ret->add_currentwearabledefinitions();
		addRef->set_fullnamelookup(addRef->fullnamelookup());
	}

	return ret;
}

FUniqueNetId Parser::ParseFUniqueID(HederaDefinitions::FUniqueID a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FUniqueNetId ret = FUniqueNetId();

	ret.bHasValue = a.bhasvalue();

	for (int i = 0; i < a.rawid_size(); i++) {
		ret.RawId[i] = a.rawid()[i];
	}

	return ret;
}

FCurrencyState Parser::ParseFCurrencyState(HederaDefinitions::FCurrencyState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects){
	FCurrencyState ret = FCurrencyState();

	ret.CurrentAmount = a.currentamount();
	ret.FormOfCurrency = a.type();
	ret.LastKnownAmount = a.lastknownamount();

	return ret;
}

FResourcePoolReference Parser::ParseFResourcePoolReference(HederaDefinitions::FResourcePoolReference a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FResourcePoolReference ret = FResourcePoolReference();
	ret.Data = (UResourcePool*)ObjectReferenceToObject(a.data(), objects, UResourcePool::StaticClass());
	ret.PoolGUID = a.poolguid();
	ret.PoolIndexInManager = a.poolindexinmanager();
	ret.PoolManager = (AResourcePoolManager*)ReplicatedActorReferenceToReplicatedActor(a.poolmanager(), channels);
	return ret;
}

FReplicatedNounAttributeState Parser::ParseFReplicatedNounAttributeState(HederaDefinitions::FReplicatedNounAttributeState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedNounAttributeState ret = FReplicatedNounAttributeState();

	ret.bOnlyRelevantToOwner = a.bonlyrelevanttoowner();
	ret.NounName = ParseFName(a.nounname(), channels, objects);
	ret.Value = a.value();

	return ret;
}

FWeaponDefinitionData Parser::ParseFWeaponDefinitionData(HederaDefinitions::FWeaponDefinitionData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FWeaponDefinitionData ret = FWeaponDefinitionData();

	ret.WeaponTypeDefinition = (UWeaponTypeDefinition*)ObjectReferenceToObject(a.weapontypedefinition(), objects, UWeaponTypeDefinition::StaticClass());
	
	return ret;
}

FReplicatedStandInGear Parser::ParseFReplicatedStandInGear(HederaDefinitions::FReplicatedStandInGear a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedStandInGear ret = FReplicatedStandInGear();
	ret.Weapon1Data = ParseFWeaponDefinitionData(a.weapon1data(), channels, objects);
	ret.Weapon2Data = ParseFWeaponDefinitionData(a.weapon2data(), channels, objects);
	return ret;
}

FTrackedSkillState Parser::ParseFTrackedSkillState(HederaDefinitions::FTrackedSkillState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FTrackedSkillState ret = FTrackedSkillState();

	ret.PackedStackCountAndPercentComplete = a.packedstackcountandpercentcomplete();
	ret.SkillDef = (USkillDefinition*)ObjectReferenceToObject(a.skilldef(), objects, USkillDefinition::StaticClass());
	
	return ret;
}

FManagedActorCountData Parser::ParseFManagedActorCountData(HederaDefinitions::FManagedActorCountData a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FManagedActorCountData ret = FManagedActorCountData();

	ret.Count = a.count();
	ret.Type = (UPoplarManagedActorDefinition*)ObjectReferenceToObject(a.type(), objects, UPoplarManagedActorDefinition::StaticClass());

	return ret;
}

FReplicatedPerkItem Parser::ParseFReplicatedPerkItem(HederaDefinitions::FReplicatedPerkItem a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedPerkItem ret = FReplicatedPerkItem();

	ret.ApolloJSON = FString(a.apollojson().c_str());
	ret.AssetKey = FString(a.assetkey().c_str());
	ret.bActive = a.bactive();
	ret.bCanUse = a.bcanuse();
	ret.EntitlementIndex = a.entitlementindex();
	ret.ItemLevel = a.itemlevel();
	ret.MetaID = FString(a.metaid().c_str());
	ret.PerkFunction = (UPoplarPerkFunction*)ObjectReferenceToObject(a.perkfunction(), objects, UPoplarPerkFunction::StaticClass());
	ret.PlayerID = ParseFUniqueID(a.playerid(), channels, objects);
	ret.Rarity = a.rarity();
	ret.Wearable = (UPoplarPlayerWearableDefinition*)ObjectReferenceToObject(a.wearable(), objects, UPoplarPlayerWearableDefinition::StaticClass());

	return ret;
}

FAugInstance Parser::ParseFAugInstance(HederaDefinitions::FAugInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAugInstance ret = FAugInstance();

	ret.AugDef = (UPoplarAugDefinition*)ObjectReferenceToObject(a.augdef(), objects, UPoplarAugDefinition::StaticClass());
	ret.ClientPurchaseStatus = a.clientpurchasestatus();
	ret.CurrentTier = a.currenttier();
	ret.Preselected = a.preselected();

	return ret;
}

FAugCategoryInstance Parser::ParseFAugCategoryInstance(HederaDefinitions::FAugCategoryInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAugCategoryInstance ret = FAugCategoryInstance();

	for (int i = 0; i < a.augs_size(); i++) {
		ret.Augs[i] = ParseFAugInstance(a.augs()[i], channels, objects);
	}

	ret.CategoryDef = (UPoplarAugCategory*)ObjectReferenceToObject(a.categorydef(), objects, UPoplarAugCategory::StaticClass());
	ret.CategoryIsUnlocked = a.categoryisunlocked();
	ret.Mutation = ParseFAugInstance(a.mutation(), channels, objects);

	return ret;
}

FAugSetInstance Parser::ParseFAugSetInstance(HederaDefinitions::FAugSetInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAugSetInstance ret = FAugSetInstance();

	for (int i = 0; i < a.allcategories_size(); i++) {
		ret.AllCategories[i] = ParseFAugCategoryInstance(a.allcategories()[i], channels, objects);
	}

	ret.AugSetDef = (UPoplarAugSet*)ObjectReferenceToObject(a.augsetdef(), objects, UPoplarAugSet::StaticClass());

	return ret;
}

FAugInstance Parser::ParseFAugInstancePassedIn(HederaDefinitions::FAugInstance a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FAugInstance ret = FAugInstance();

	ret.AugDef = (UPoplarAugDefinition*)ObjectReferenceToObject(a.augdef(), objects, UPoplarAugDefinition::StaticClass());
	ret.ClientPurchaseStatus = a.clientpurchasestatus();
	ret.CurrentTier = a.currenttier();
	ret.Preselected = a.preselected();

	return ret;
}

FReplicatedBehaviorConsumerState Parser::ParseFReplicatedBehaviorConsumerState(HederaDefinitions::FReplicatedBehaviorConsumerState a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedBehaviorConsumerState ret = FReplicatedBehaviorConsumerState();

	ret.BehaviorSequenceEnabledFlags = a.behaviorsequenceenabledflags();
	ret.bIsProcessSuspended = a.bisprocesssuspended();
	ret.bNeedToApplyThisState = a.bneedtoapplythisstate();

	return ret;
}

FReplicatedBehaviorEvent Parser::ParseFReplicatedBehaviorEvent(HederaDefinitions::FReplicatedBehaviorEvent a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FReplicatedBehaviorEvent ret = FReplicatedBehaviorEvent();

	ret.EventCounter = a.eventcounter();
	ret.EventIndex = a.eventindex();
	ret.EventOutput = a.eventoutput();
	ret.Instigator = ObjectReferenceToObject(a.instigator(), objects, UObject::StaticClass());
	ret.SequenceIndex = a.sequenceindex();

	return ret;
}

FCinematicModeOptions Parser::ParseFCinematicModeOptions(HederaDefinitions::FCinematicModeOptions a, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {
	FCinematicModeOptions ret = FCinematicModeOptions();

	ret.ApplicationMode = a.applicationmode();
	ret.bAffectsAudio = a.baffectsaudio();
	ret.bAffectsButtons = a.baffectsbuttons();
	ret.bAffectsGodMode = a.baffectsgodmode();
	ret.bAffectsHUD = a.baffectshud();
	ret.bAffectsMovement = a.baffectsmovement();
	ret.bAffectsNoTarget = a.baffectsnotarget();
	ret.bAffectsSkills = a.baffectsskills();
	ret.bAffectsTurning = a.baffectsturning();
	ret.bAllowCinematicSplitScreen = a.ballowcinematicsplitscreen();
	ret.bCinematicMode = a.bcinematicmode();
	ret.bHideOutlines = a.bhideoutlines();
	ret.bHidePlayer = a.bhideplayer();
	ret.bHideScreenParticles = a.bhidescreenparticles();
	ret.bKismetSetCinematicMode = a.bkismetsetcinematicmode();
	ret.ModeToken = a.modetoken();

	return ret;
}

void Parser::ParseAPoplarPlayerReplicationInfo(HederaDefinitions::APoplarPlayerReplicationInfo a, APoplarPlayerReplicationInfo* b, std::vector<UActorChannel*>& channels, TArray<UObject*>* objects) {	
	b->Location = ParseFVector(a.location(), channels, objects);
	b->Rotation = ParseFRotator(a.rotation(), channels, objects);
	b->DrawScale = a.drawscale();
	b->CustomTimeDilation = a.customtimedilation();
	b->Physics = (EPhysics)a.physics();
	b->RemoteRole = (ENetRole)a.role();
	b->Role = (ENetRole)a.remoterole(); //intentional, needs to be swapped from Host's POV
	b->ReplicatedCollisionType = (ECollisionType)a.replicatedcollisiontype();
	b->Owner = ReplicatedActorReferenceToReplicatedActor(a.owner(), channels);
	b->Base = ReplicatedActorReferenceToReplicatedActor(a.base(), channels);
	b->bHidden = a.bhidden();
	b->bTearOff = a.btearoff();
	b->bHardAttach = a.bhardattach();
	b->bCollideActors = a.bcollideactors();
	b->bCollideWorld = a.bcollideworld();
	b->bBlockActors = a.bblockactors();
	b->bProjTarget = a.bprojtarget();
	b->bNetOwner = a.bnetowner();
	b->Instigator = (APawn*)ReplicatedActorReferenceToReplicatedActor(a.instigator(), channels);
	b->Velocity = ParseFVector(a.velocity(), channels, objects);
	b->Deaths = a.deaths();
	b->Ping = a.ping();
	b->PlayerName = FString(a.playername().c_str());
	b->bAdmin = a.badmin();
	b->bIsSpectator = a.bisspectator();
	b->bOutOfLives = a.boutoflives();
	b->bBot = a.bbot();
	b->bFromPreviousLevel = a.bfrompreviouslevel();
	b->bPlayerAFK = a.bplayerafk();
	b->PlayerID = a.playerid();
	if (!b->Team) {
		ATeamInfo* teamInfo = b->Team;
		b->Team = (ATeamInfo*)ReplicatedActorReferenceToReplicatedActor(a.team(), channels);
		b->OnRep_Team(teamInfo);
	}
	b->StartTime = a.starttime();
	b->Kills = a.kills();
	b->UniqueId = ParseFUniqueID(a.uniqueid(), channels, objects);
	b->ExpLevel = a.explevel();
	b->ExpPointsNextLevelAt = a.exppointsnextlevelat();
	b->ExpPointsNextLevelAtBaseValue = a.exppointsnextlevelatbasevalue();
	b->bIsDead = a.bisdead();
	b->bHideOnHUDIfEnemy = a.bhideonhudifenemy();
	b->GeneralSkillPoints = a.generalskillpoints();
	b->SpecialistSkillPoints = a.specialistskillpoints();
	b->CurrentInstantHitCriticalHitBonus = a.currentinstanthitcriticalhitbonus();
	b->CurrentInstantHitCriticalHitBonusBaseValue = a.currentinstanthitcriticalhitbonusbasevalue();
	
	for (int i = 0; i < a.currency_size(); i++) {
		b->Currency[i] = ParseFCurrencyState(a.currency()[i], channels, objects);
	}

	b->HealthPool = ParseFResourcePoolReference(a.healthpool(), channels, objects);
	b->ShieldArmor = ParseFResourcePoolReference(a.healthpool(), channels, objects);
	b->PawnAllegiance = (UPawnAllegiance*)ObjectReferenceToObject(a.pawnallegiance(), objects, UPawnAllegiance::StaticClass());
	b->CharacterNameIdDef = (UPlayerNameIdentifierDefinition*)ObjectReferenceToObject(a.characternameiddef(), objects, UPlayerNameIdentifierDefinition::StaticClass());
	b->bGFxMenuOpen = a.bgfxmenuopen();
	b->NumTrackedSkillSlotsInUse = a.numtrackedskillslotsinuse();
	b->HighestCompletedPlayThrough = a.highestcompletedplaythrough();
	b->NumReplicatedNouns = a.numreplicatednouns();
	
	for (int i = 0; i < a.replicatednouns_size(); i++) {
		b->ReplicatedNouns[i] = ParseFReplicatedNounAttributeState(a.replicatednouns()[i], channels, objects);
	}

	b->StandInGear = ParseFReplicatedStandInGear(a.standingear(), channels, objects);

	for (int i = 0; i < a.trackedskills_size(); i++) {
		b->TrackedSkills[i] = ParseFTrackedSkillState(a.trackedskills()[i], channels, objects);
	}

	if(!b->PoplarPSI)
		b->PoplarPSI = (APoplarPlayerStateInfo*)ReplicatedActorReferenceToReplicatedActor(a.poplarpsi(), channels);
	b->bNPC = a.bnpc();
	b->bDigitalDelux = a.bdigitaldelux();

	bool bPrevPlayBrowsingTaunt = 0;
	b->bPlayBrowsingTaunt = a.bplaybrowsingtaunt();
	b->OnRep_bPlayBrowsingTaunt(bPrevPlayBrowsingTaunt);

	b->bIsPvEMatch = a.bispvematch();
	b->bNeedsToCastSurrenderVote = a.bneedstocastsurrendervote();
	b->BotOwnerPRI = (APoplarPlayerReplicationInfo*)ReplicatedActorReferenceToReplicatedActor(a.botownerpri(), channels);
	b->BotDifficulty = a.botdifficulty();
	b->MyAlertPoint = (APoplarAlertPoint*)ReplicatedActorReferenceToReplicatedActor(a.myalertpoint(), channels);
	b->EnergyPool = ParseFResourcePoolReference(a.energypool(), channels, objects);
	b->UnitTypeDefinition = (UPoplarUnitTypeDefinition*)ObjectReferenceToObject(a.unittypedefinition(), objects, UPoplarUnitTypeDefinition::StaticClass());
	b->StatReplicationInfo = (APoplarStatReplicationInfo*)ReplicatedActorReferenceToReplicatedActor(a.statreplicationinfo(), channels);
	
	for (int i = 0; i < a.nemesispriarray_size(); i++) {
		b->NemesisPRIArray[i] = (APoplarPlayerReplicationInfo*)ReplicatedActorReferenceToReplicatedActor(a.nemesispriarray()[i], channels);
	}

	for (int i = 0; i < a.managedactorcounts_size(); i++) {
		b->ManagedActorCounts[i] = ParseFManagedActorCountData(a.managedactorcounts()[i], channels, objects);
	}

	uint8_t oldRespTime = b->ReplicatedRespawnTimeRemaining;
	b->ReplicatedRespawnTimeRemaining = a.replicatedrespawntimeremaining();
	//b->OnRep_ReplicatedRespawnTimeRemaining(oldRespTime);

	b->CharacterSelectionState = (ECharacterMenuSelectionState)a.characterselectionstate();
	
	for (int i = 0; i < a.activatedperkflags_size(); i++) {
		b->ActivatedPerkFlags[i] = a.activatedperkflags()[i];
	}

	for (int i = 0; i < a.perks_size(); i++) {
		FReplicatedPerkItem prevPerk = b->Perks[i];
		if (!prevPerk.PerkFunction) {
			b->Perks[i] = ParseFReplicatedPerkItem(a.perks()[i], channels, objects);
			//b->OnRep_Perks(i, prevPerk);
		}
	}

	b->OvershieldStrength = a.overshieldstrength();
	b->MaxOvershieldValue = a.maxovershieldvalue();
	b->BrowseCharacterNameIdDef = (UPlayerNameIdentifierDefinition*)ObjectReferenceToObject(a.browsecharacternameiddef(), objects, UPlayerNameIdentifierDefinition::StaticClass());
	b->BrowseSkinIdDef = (UPoplarMetaSkinDefinition*)ObjectReferenceToObject(a.browseskiniddef(), objects, UPoplarMetaSkinDefinition::StaticClass());
	b->SkinIdDef = (UPoplarMetaSkinDefinition*)ObjectReferenceToObject(a.skiniddef(), objects, UPoplarMetaSkinDefinition::StaticClass());
	//if(!b->Augs.AllCategories[0].Augs[0].AugDef)
	b->Augs = ParseFAugSetInstance(a.augs(), channels, objects);
	b->ReplicatedBehaviorConsumerState = ParseFReplicatedBehaviorConsumerState(a.replicatedbehaviorconsumerstate(), channels, objects);
	b->ReplicatedBehaviorEvent = ParseFReplicatedBehaviorEvent(a.replicatedbehaviorevent(), channels, objects);
	
	FCinematicModeOptions oldCinematicModeOptions = b->ReplicatedCinematicModeOptions;
	b->ReplicatedCinematicModeOptions = ParseFCinematicModeOptions(a.replicatedcinematicmodeoptions(), channels, objects);
	//b->OnRep_ReplicatedCinematicModeOptions(oldCinematicModeOptions);
	
	for (int i = 0; i < a.currentwearabledefinitions_size(); i++) {
		b->CurrentWearableDefinitions[i] = (UPoplarPlayerWearableDefinition*)ObjectReferenceToObject(a.currentwearabledefinitions()[i], objects, UPoplarPlayerWearableDefinition::StaticClass());
	}
}