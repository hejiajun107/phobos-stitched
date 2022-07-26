#pragma once
#include <Utilities/Template.h>
#include <Ext/Techno/Body.h>
#include <set>
#include <map>

class PhobosGlobal
{
public:
	std::multimap<int, TechnoClass*, std::greater<int>> Techno_HugeBar;
	std::map<int, std::vector<TriggerClass*>> RandomTriggerPool;
	std::vector<QueuedSW> MultipleSWFirer_Queued;
	TechnoClass* GenericStand;

	bool Save(PhobosStreamWriter& stm);
	bool Load(PhobosStreamReader& stm);

	static bool SaveGlobals(PhobosStreamWriter& stm);
	static bool LoadGlobals(PhobosStreamReader& stm);

	static PhobosGlobal* Global();

	PhobosGlobal() :
		Techno_HugeBar()
		, RandomTriggerPool()
		, GenericStand(nullptr)
	{
	}

	~PhobosGlobal() = default;

	static void Clear();
	static void PointerGotInvalid(void* ptr, bool bRemoved);
	void InvalidatePointer(void* ptr);

	TechnoClass* GetGenericStand();
	void CheckSuperQueued();

	void Reset();

private:
	template <typename T>
	bool Serialize(T& stm);

	static PhobosGlobal GlobalObject;
};
