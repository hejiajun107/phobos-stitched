#include "Body.h"

#include <SuperClass.h>
#include <SuperWeaponTypeClass.h>
#include <StringTable.h>
#include <Ext/SWType/NewSWType/NewSWType.h>

template<> const DWORD Extension<SuperWeaponTypeClass>::Canary = 0x11111111;
SWTypeExt::ExtContainer SWTypeExt::ExtMap;

bool SWTypeExt::Activate(SuperClass* pSuper, CellStruct cell, bool isPlayer)
{
	auto pSWTypeExt = SWTypeExt::ExtMap.Find(pSuper->Type);
	int newIdx = NewSWType::GetNewSWTypeIdx(pSWTypeExt->TypeID.data());

	Debug::Log("[Phobos::SW::Active] %s\n", pSWTypeExt->TypeID.data());

	if (newIdx != -1)
		return NewSWType::GetNthItem(newIdx)->Activate(pSuper, cell, isPlayer);

	return false;
}

void SWTypeExt::ExtData::InitializeConstants()
{
	NewSWType::Init();
}

// =============================
// load / save

template <typename T>
void SWTypeExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->TypeID)
		.Process(this->Money_Amount)
		.Process(this->SW_Inhibitors)
		.Process(this->SW_AnyInhibitor)
		.Process(this->SW_Designators)
		.Process(this->SW_AnyDesignator)
		.Process(this->SW_RangeMinimum)
		.Process(this->SW_RangeMaximum)
		.Process(this->SW_RequiredHouses)
		.Process(this->SW_ForbiddenHouses)
		.Process(this->SW_AuxBuildings)
		.Process(this->SW_NegBuildings)
		.Process(this->UIDescription)
		.Process(this->CameoPriority)
		.Process(this->LimboDelivery_Types)
		.Process(this->LimboDelivery_IDs)
		.Process(this->LimboDelivery_RandomWeightsData)
		.Process(this->LimboDelivery_RollChances)
		.Process(this->LimboKill_Affected)
		.Process(this->LimboKill_IDs)
		.Process(this->RandomBuffer)
		.Process(this->Detonate_Warhead)
		.Process(this->Detonate_Weapon)
		.Process(this->Detonate_Damage)
		.Process(this->SW_Next)
		.Process(this->SW_Next_RealLaunch)
		.Process(this->SW_Next_IgnoreInhibitors)
		.Process(this->SW_Next_IgnoreDesignators)
		.Process(this->SW_Next_RandomWeightsData)
		.Process(this->SW_Next_RollChances)
		.Process(this->GScreenAnimType)
		.Process(this->CreateBuilding)
		.Process(this->CreateBuilding_Type)
		.Process(this->CreateBuilding_Duration)
		.Process(this->CreateBuilding_Reload)
		.Process(this->CreateBuilding_AutoCreate)
		.Process(this->ScreenSW)
		.Process(this->ScreenSW_Offset)
		.Process(this->ScreenSW_Duration)
		.Process(this->ScreenSW_Reload)
		.Process(this->ScreenSW_AutoLaunch)
		.Process(this->NextSuperWeapon)

		.Process(this->SW_AffectsHouse)
		.Process(this->SW_AffectsTarget)
		.Process(this->SW_VirtualCharge)
		.Process(this->SW_Proirity)

		.Process(this->MultipleSWFirer_FireSW_Types)
		.Process(this->MultipleSWFirer_FireSW_Deferments)
		.Process(this->MultipleSWFirer_RandomPick)
		.Process(this->MultipleSWFirer_RealLaunch)

		.Process(this->WeaponDetonate_Weapons)
		.Process(this->WeaponDetonate_TechnoTypes)
		.Process(this->WeaponDetonate_TechnoTypes_Ignore)
		.Process(this->WeaponDetonate_RandomPick_Weapon)
		.Process(this->WeaponDetonate_RandomPick_TechnoType)
		.Process(this->WeaponDetonate_RandomPick_Weapon_Weights)
		.Process(this->WeaponDetonate_RandomPick_TechnoType_Weights)
		.Process(this->WeaponDetonate_PerSum_WeaponWeights)
		.Process(this->WeaponDetonate_PerSum_TechnoTypeWeights)
		;
}

void SWTypeExt::ExtData::LoadFromINIFile(CCINIClass* const pINI)
{
	auto pThis = this->OwnerObject();
	const char* pSection = pThis->ID;

	if (!pINI->GetSection(pSection))
	{
		return;
	}

	// from ares
	INI_EX exINI(pINI);

	this->TypeID.Read(pINI, pSection, "Type");

	this->Money_Amount.Read(exINI, pSection, "Money.Amount");
	this->SW_Inhibitors.Read(exINI, pSection, "SW.Inhibitors");
	this->SW_AnyInhibitor.Read(exINI, pSection, "SW.AnyInhibitor");
	this->SW_Designators.Read(exINI, pSection, "SW.Designators");
	this->SW_AnyDesignator.Read(exINI, pSection, "SW.AnyDesignator");
	this->SW_RangeMinimum.Read(exINI, pSection, "SW.RangeMinimum");
	this->SW_RangeMaximum.Read(exINI, pSection, "SW.RangeMaximum");
	this->SW_RequiredHouses = pINI->ReadHouseTypesList(pSection, "SW.RequiredHouses", this->SW_RequiredHouses);
	this->SW_ForbiddenHouses = pINI->ReadHouseTypesList(pSection, "SW.ForbiddenHouses", this->SW_ForbiddenHouses);
	this->SW_AuxBuildings.Read(exINI, pSection, "SW.AuxBuildings");
	this->SW_NegBuildings.Read(exINI, pSection, "SW.NegBuildings");

	this->UIDescription.Read(exINI, pSection, "UIDescription");
	this->CameoPriority.Read(exINI, pSection, "CameoPriority");
	this->LimboDelivery_Types.Read(exINI, pSection, "LimboDelivery.Types");
	this->LimboDelivery_IDs.Read(exINI, pSection, "LimboDelivery.IDs");
	this->LimboDelivery_RollChances.Read(exINI, pSection, "LimboDelivery.RollChances");
	this->LimboKill_Affected.Read(exINI, pSection, "LimboKill.Affected");
	this->LimboKill_IDs.Read(exINI, pSection, "LimboKill.IDs");
	this->SW_Next.Read(exINI, pSection, "SW.Next");
	this->SW_Next_RealLaunch.Read(exINI, pSection, "SW.Next.RealLaunch");
	this->SW_Next_IgnoreInhibitors.Read(exINI, pSection, "SW.Next.IgnoreInhibitors");
	this->SW_Next_IgnoreDesignators.Read(exINI, pSection, "SW.Next.IgnoreDesignators");
	this->SW_Next_RollChances.Read(exINI, pSection, "SW.Next.RollChances");

	char tempBuffer[32];
	// LimboDelivery.RandomWeights
	for (size_t i = 0; ; ++i)
	{
		ValueableVector<int> weights;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "LimboDelivery.RandomWeights%d", i);
		weights.Read(exINI, pSection, tempBuffer);

		if (!weights.size())
			break;

		this->LimboDelivery_RandomWeightsData.push_back(weights);
	}
	ValueableVector<int> weights;
	weights.Read(exINI, pSection, "LimboDelivery.RandomWeights");
	if (weights.size())
	{
		if (this->LimboDelivery_RandomWeightsData.size())
			this->LimboDelivery_RandomWeightsData[0] = weights;
		else
			this->LimboDelivery_RandomWeightsData.push_back(weights);
	}

	// SW.Next.RandomWeights
	for (size_t i = 0; ; ++i)
	{
		ValueableVector<int> weights2;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "SW.Next.RandomWeights%d", i);
		weights2.Read(exINI, pSection, tempBuffer);

		if (!weights2.size())
			break;

		this->SW_Next_RandomWeightsData.push_back(weights2);
	}
	ValueableVector<int> weights2;
	weights2.Read(exINI, pSection, "SW.Next.RandomWeights");
	if (weights2.size())
	{
		if (this->SW_Next_RandomWeightsData.size())
			this->SW_Next_RandomWeightsData[0] = weights2;
		else
			this->SW_Next_RandomWeightsData.push_back(weights2);
	}
	this->Detonate_Warhead.Read(exINI, pSection, "Detonate.Warhead");
	this->Detonate_Weapon.Read(exINI, pSection, "Detonate.Weapon", true);
	this->Detonate_Damage.Read(exINI, pSection, "Detonate.Damage");
	this->GScreenAnimType.Read(exINI, pSection, "GScreenAnimType", true);

	this->CreateBuilding.Read(exINI, pSection, "CreateBuilding");
	this->CreateBuilding_Type.Read(exINI, pSection, "CreateBuilding.Type");
	this->CreateBuilding_Duration.Read(exINI, pSection, "CreateBuilding.Duration");
	this->CreateBuilding_Reload.Read(exINI, pSection, "CreateBuilding.Reload");
	this->CreateBuilding_AutoCreate.Read(exINI, pSection, "CreateBuilding.AutoCreate");

	this->ScreenSW.Read(exINI, pSection, "ScreenSW");
	this->ScreenSW_Offset.Read(exINI, pSection, "ScreenSW.Offset");
	this->ScreenSW_Duration.Read(exINI, pSection, "ScreenSW.Duration");
	this->ScreenSW_Reload.Read(exINI, pSection, "ScreenSW.Reload");
	this->ScreenSW_AutoLaunch.Read(exINI, pSection, "ScreenSW.AutoLaunch");

	this->NextSuperWeapon.Read(exINI, pSection, "NextSuperWeapon");

	this->SW_AffectsHouse.Read(exINI, pSection, "SW.AffectsHouse");
	this->SW_AffectsTarget.Read(exINI, pSection, "SW.AffectsTarget");
	this->SW_VirtualCharge.Read(exINI, pSection, "SW.VirtualCharge");

	this->SW_Proirity.Read(exINI, pSection, "SW.Priority");

	int newidx = NewSWType::GetNewSWTypeIdx(TypeID.data());

	if (newidx != -1)
	{
		NewSWType* pNewSWType = NewSWType::GetNthItem(newidx);
		pNewSWType->Initialize(const_cast<SWTypeExt::ExtData*>(this), OwnerObject());
		pNewSWType->LoadFromINI(const_cast<SWTypeExt::ExtData*>(this), OwnerObject(), pINI);
	}

}

void SWTypeExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<SuperWeaponTypeClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void SWTypeExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<SuperWeaponTypeClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

bool SWTypeExt::LoadGlobals(PhobosStreamReader& Stm)
{
	return Stm
		.Success();
}

bool SWTypeExt::SaveGlobals(PhobosStreamWriter& Stm)
{
	return Stm
		.Success();
}

// =============================
// container

SWTypeExt::ExtContainer::ExtContainer() : Container("SuperWeaponTypeClass")
{
}

SWTypeExt::ExtContainer::~ExtContainer() = default;

// =============================
// container hooks

DEFINE_HOOK(0x6CE6F6, SuperWeaponTypeClass_CTOR, 0x5)
{
	GET(SuperWeaponTypeClass*, pItem, EAX);

	SWTypeExt::ExtMap.FindOrAllocate(pItem);
	return 0;
}

DEFINE_HOOK(0x6CEFE0, SuperWeaponTypeClass_SDDTOR, 0x8)
{
	GET(SuperWeaponTypeClass*, pItem, ECX);

	SWTypeExt::ExtMap.Remove(pItem);
	return 0;
}

DEFINE_HOOK_AGAIN(0x6CE8D0, SuperWeaponTypeClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x6CE800, SuperWeaponTypeClass_SaveLoad_Prefix, 0xA)
{
	GET_STACK(SuperWeaponTypeClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	SWTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x6CE8BE, SuperWeaponTypeClass_Load_Suffix, 0x7)
{
	SWTypeExt::ExtMap.LoadStatic();
	return 0;
}

DEFINE_HOOK(0x6CE8EA, SuperWeaponTypeClass_Save_Suffix, 0x3)
{
	SWTypeExt::ExtMap.SaveStatic();
	return 0;
}

DEFINE_HOOK_AGAIN(0x6CEE50, SuperWeaponTypeClass_LoadFromINI, 0xA)
DEFINE_HOOK(0x6CEE43, SuperWeaponTypeClass_LoadFromINI, 0xA)
{
	GET(SuperWeaponTypeClass*, pItem, EBP);
	GET_STACK(CCINIClass*, pINI, 0x3FC);

	SWTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	return 0;
}

