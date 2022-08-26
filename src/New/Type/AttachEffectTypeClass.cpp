#include "AttachEffectTypeClass.h"

#include <Utilities/TemplateDef.h>
#include <Utilities/SavegameDef.h>

Enumerable<AttachEffectTypeClass>::container_t Enumerable<AttachEffectTypeClass>::Array;

const char* Enumerable<AttachEffectTypeClass>::GetMainSection()
{
	return "PhobosAttachEffectTypes";
}

void AttachEffectTypeClass::LoadFromINI(CCINIClass* pINI)
{
	const char* pSection = Name.data();

	INI_EX exINI(pINI);

	this->FirePower.Read(exINI, pSection, "FirePower");
	this->ROF.Read(exINI, pSection, "ROF");
	this->Armor.Read(exINI, pSection, "Armor");
	this->Speed.Read(exINI, pSection, "Speed");
	this->ROT.Read(exINI, pSection, "ROT");
	this->FirePower_Multiplier.Read(exINI, pSection, "FirePower.Multiplier");
	this->ROF_Multiplier.Read(exINI, pSection, "ROF.Multiplier");
	this->Armor_Multiplier.Read(exINI, pSection, "Armor.Multiplier");
	this->Speed_Multiplier.Read(exINI, pSection, "Speed.Multiplier");
	this->ROT_Multiplier.Read(exINI, pSection, "ROT.Multiplier");
	this->DisableWeapon.Read(exINI, pSection, "DisableWeapon");
	this->DisableWeapon_Category.Read(exINI, pSection, "DisableWeapon.Category");
	this->Cloak.Read(exINI, pSection, "Cloak");
	this->Decloak.Read(exINI, pSection, "Decloak");
	this->Anim.Read(exINI, pSection, "Anim", true);
	this->EndedAnim.Read(exINI, pSection, "EndedAnim", true);
	this->WeaponList.Read(exINI, pSection, "WeaponList");
	this->WeaponList_FireOnAttach.Read(exINI, pSection, "WeaponList.FireOnAttach");
	this->AttackedWeaponList.Read(exINI, pSection, "AttackedWeaponList");
	this->ReplaceArmor.Read(exINI, pSection, "ReplaceArmor");
	this->ReplaceArmor_Shield.Read(exINI, pSection, "ReplaceArmor.Shield");
	this->ReplaceWeapon.Read(exINI, pSection, "ReplaceWeapon");
	this->ReplacePrimary.Read(exINI, pSection, "ReplacePrimary.%s");
	this->ReplaceSecondary.Read(exINI, pSection, "ReplaceSecondary.%s");
	this->ReplaceGattlingWeapon.Read(exINI, pSection, "ReplaceGattlingWeapon.%s");
	this->PenetratesIronCurtain.Read(exINI, pSection, "PenetratesIronCurtain");
	this->DiscardOnEntry.Read(exINI, pSection, "DiscardOnEntry");
	this->Cumulative.Read(exINI, pSection, "Cumulative");
	this->IfExist_AddTimer.Read(exINI, pSection, "IfExist.AddTimer");
	this->IfExist_AddTimer_Cap.Read(exINI, pSection, "IfExist.AddTimer.Cap");
	this->IfExist_ResetTimer.Read(exINI, pSection, "IfExist.ResetTimer");
	this->IfExist_ResetAnim.Read(exINI, pSection, "IfExist.ResetAnim");
	this->ShowAnim_Cloaked.Read(exINI, pSection, "ShowAnim.Cloaked");
	this->Loop_Delay.Read(exINI, pSection, "Loop.Delay");
	this->Loop_Duration.Read(exINI, pSection, "Loop.Duration");
	this->Duration.Read(exINI, pSection, "Duration");
	this->Delay.Read(exINI, pSection, "Delay");
	this->Coexist_Maximum.Read(exINI, pSection, "Coexist.Maximum");
}

template <typename T>
void AttachEffectTypeClass::Serialize(T& stm)
{
	stm
		.Process(this->FirePower)
		.Process(this->ROF)
		.Process(this->Armor)
		.Process(this->Speed)
		.Process(this->ROT)
		.Process(this->FirePower_Multiplier)
		.Process(this->ROF_Multiplier)
		.Process(this->Armor_Multiplier)
		.Process(this->Speed_Multiplier)
		.Process(this->ROT_Multiplier)
		.Process(this->DisableWeapon)
		.Process(this->DisableWeapon_Category)
		.Process(this->Cloak)
		.Process(this->Decloak)
		.Process(this->Anim)
		.Process(this->EndedAnim)
		.Process(this->WeaponList)
		.Process(this->WeaponList_FireOnAttach)
		.Process(this->AttackedWeaponList)
		.Process(this->PenetratesIronCurtain)
		.Process(this->DiscardOnEntry)
		.Process(this->Cumulative)
		.Process(this->Duration)
		.Process(this->Delay)
		.Process(this->Loop_Delay)
		.Process(this->Loop_Duration)
		.Process(this->ShowAnim_Cloaked)
		.Process(this->IfExist_AddTimer)
		.Process(this->IfExist_AddTimer_Cap)
		.Process(this->IfExist_ResetTimer)
		.Process(this->IfExist_ResetAnim)
		.Process(this->ReplaceArmor)
		.Process(this->ReplaceArmor_Shield)
		.Process(this->ReplaceWeapon)
		.Process(this->ReplacePrimary)
		.Process(this->ReplaceSecondary)
		.Process(this->ReplaceGattlingWeapon)
		.Process(this->Coexist_Maximum)
		;
}

void AttachEffectTypeClass::LoadFromStream(PhobosStreamReader& stm)
{
	Serialize(stm);
}

void AttachEffectTypeClass::SaveToStream(PhobosStreamWriter& stm)
{
	Serialize(stm);
}
