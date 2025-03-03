#pragma region Ares Copyrights
/*
 *Copyright (c) 2008+, All Ares Contributors
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *3. All advertising materials mentioning features or use of this software
 *   must display the following acknowledgement:
 *   This product includes software developed by the Ares Contributors.
 *4. Neither the name of Ares nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY ITS CONTRIBUTORS ''AS IS'' AND ANY
 *EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *DISCLAIMED. IN NO EVENT SHALL THE ARES CONTRIBUTORS BE LIABLE FOR ANY
 *DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion

#pragma once
#include <Windows.h>

#include "Template.h"

#include "INIParser.h"
#include "Enum.h"
#include "Constructs.h"
#include "SavegameDef.h"

#include <InfantryTypeClass.h>
#include <AircraftTypeClass.h>
#include <UnitTypeClass.h>
#include <BuildingTypeClass.h>
#include <FootClass.h>
#include <VocClass.h>
#include <VoxClass.h>
#include <CRT.h>
#include <LocomotionClass.h>
#include <SuperWeaponTypeClass.h>
#include <ParticleSystemTypeClass.h>
#include <ScriptTypeClass.h>

namespace detail
{
	template <typename T>
	inline bool read(T& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate = false)
	{
		if (parser.ReadString(pSection, pKey))
		{
			using base_type = std::remove_pointer_t<T>;

			auto const pValue = parser.value();
			auto const parsed = (allocate ? base_type::FindOrAllocate : base_type::Find)(pValue);
			if (parsed || INIClass::IsBlank(pValue))
			{
				value = parsed;
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, pValue);
			}
		}
		return false;
	}

	template <>
	inline bool read(ParticleSystemTypeClass*& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto const pValue = parser.value();
			auto const parsed = !allocate ? ParticleSystemTypeClass::Find(pValue) : GameCreate<ParticleSystemTypeClass>(pValue);
			if (parsed || INIClass::IsBlank(pValue))
			{
				value = parsed;
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, pValue);
			}
		}
		return false;
	}

	template <>
	inline bool read(ScriptTypeClass*& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto const pValue = parser.value();
			auto const parsed = !allocate ? ScriptTypeClass::Find(pValue) : GameCreate<ScriptTypeClass>(pValue);
			if (parsed || INIClass::IsBlank(pValue))
			{
				value = parsed;
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, pValue);
			}
		}
		return false;
	}

	template <>
	inline bool read<bool>(bool& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		bool buffer;
		if (parser.ReadBool(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid boolean value [1, true, yes, 0, false, no]");
		}
		return false;
	}

	template <>
	inline bool read<int>(int& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		int buffer;
		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number");
		}
		return false;
	}

	template <>
	inline bool read<ArmorType>(ArmorType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		int buffer = value;

		// Hack cause armor type parser in Ares will return 0 (ArmorType 'none') if armor type is not found instead of -1.
		if (parser.ReadString(pSection, pKey))
		{
			if (!parser.ReadArmor(pSection, pKey, &buffer) || buffer < 0)
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid ArmorType");
				return false;
			}

			value = buffer;
			return true;
		}

		return false;
	}

	template <>
	inline bool read<unsigned short>(unsigned short& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		int buffer;
		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			value = static_cast<unsigned short>(buffer);
			return true;
		}
		return false;
	}

	template <>
	inline bool read<BYTE>(BYTE& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		int buffer;
		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			if (buffer <= 255 && buffer >= 0)
			{
				value = static_cast<BYTE>(buffer); // shut up shut up shut up C4244
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number between 0 and 255 inclusive.");
			}
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number");
		}
		return false;
	}

	template <>
	inline bool read<float>(float& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		double buffer;
		if (parser.ReadDouble(pSection, pKey, &buffer))
		{
			value = static_cast<float>(buffer);
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid floating point number");
		}
		return false;
	}

	template <>
	inline bool read<double>(double& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		double buffer;
		if (parser.ReadDouble(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid floating point number");
		}
		return false;
	}

	template <>
	inline bool read<Point2D>(Point2D& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.Read2Integers(pSection, pKey, (int*)&value))
		{
			return true;
		}
		return false;
	}

	template <>
	inline bool read<Vector2D<double>>(Vector2D<double>& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.Read2Doubles(pSection, pKey, (double*)&value))
		{
			return true;
		}
		return false;
	}

	template <>
	inline bool read<CoordStruct>(CoordStruct& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.Read3Integers(pSection, pKey, (int*)&value))
		{
			return true;
		}

		return false;
	}

	template <>
	inline bool read<ColorStruct>(ColorStruct& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		ColorStruct buffer = { 0, 0, 0 };
		if (parser.Read3Bytes(pSection, pKey, reinterpret_cast<byte*>(&buffer)))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid R,G,B color");
		}
		return false;
	}

	template <>
	inline bool read<PartialVector2D<int>>(PartialVector2D<int>& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		value.ValueCount = parser.ReadMultipleIntegers(pSection, pKey, (int*)&value, 2);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector2D<double>>(PartialVector2D<double>& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		value.ValueCount = parser.ReadMultipleDoubles(pSection, pKey, (double*)&value, 2);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector3D<int>>(PartialVector3D<int>& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		value.ValueCount = parser.ReadMultipleIntegers(pSection, pKey, (int*)&value, 3);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector3D<double>>(PartialVector3D<double>& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		value.ValueCount = parser.ReadMultipleDoubles(pSection, pKey, (double*)&value, 3);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<CSFText>(CSFText& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			value = parser.value();
			return true;
		}
		return false;
	}

	template <>
	inline bool read<SHPStruct*>(SHPStruct*& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{

			auto const pValue = parser.value();
			std::string Result = pValue;

			if (!strstr(pValue, ".shp"))
			{
				Result += ".shp";
			}

			if (auto const pImage = FileSystem::LoadSHPFile(Result.c_str()))
			{
				value = pImage;
				return true;
			}
			else
			{
				Debug::Log("Failed to find file %s referenced by [%s]%s=%s\n", Result.c_str(), pSection, pKey, pValue);
			}
		}
		return false;
	}

	template <>
	inline bool read<MouseCursor>(MouseCursor& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		auto ret = false;

		// compact way to define the cursor in one go
		if (parser.ReadString(pSection, pKey))
		{
			auto const buffer = parser.value();
			char* context = nullptr;
			if (auto const pFrame = strtok_s(buffer, Phobos::readDelims, &context))
			{
				Parser<int>::Parse(pFrame, &value.Frame);
			}
			if (auto const pCount = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				Parser<int>::Parse(pCount, &value.Count);
			}
			if (auto const pInterval = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				Parser<int>::Parse(pInterval, &value.Interval);
			}
			if (auto const pFrame = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				Parser<int>::Parse(pFrame, &value.MiniFrame);
			}
			if (auto const pCount = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				Parser<int>::Parse(pCount, &value.MiniCount);
			}
			if (auto const pHotX = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				MouseCursorHotSpotX::Parse(pHotX, &value.HotX);
			}
			if (auto const pHotY = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				MouseCursorHotSpotY::Parse(pHotY, &value.HotY);
			}

			ret = true;
		}

		char pFlagName[32];
		_snprintf_s(pFlagName, 31, "%s.Frame", pKey);
		ret |= read(value.Frame, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 31, "%s.Count", pKey);
		ret |= read(value.Count, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 31, "%s.Interval", pKey);
		ret |= read(value.Interval, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 31, "%s.MiniFrame", pKey);
		ret |= read(value.MiniFrame, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 31, "%s.MiniCount", pKey);
		ret |= read(value.MiniCount, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 31, "%s.HotSpot", pKey);
		if (parser.ReadString(pSection, pFlagName))
		{
			auto const pValue = parser.value();
			char* context = nullptr;
			auto const pHotX = strtok_s(pValue, ",", &context);
			MouseCursorHotSpotX::Parse(pHotX, &value.HotX);

			if (auto const pHotY = strtok_s(nullptr, ",", &context))
			{
				MouseCursorHotSpotY::Parse(pHotY, &value.HotY);
			}

			ret = true;
		}

		return ret;
	}

	template <>
	inline bool read<RocketStruct>(RocketStruct& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		auto ret = false;

		char pFlagName[0x40];
		_snprintf_s(pFlagName, 0x3F, "%s.PauseFrames", pKey);
		ret |= read(value.PauseFrames, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.TiltFrames", pKey);
		ret |= read(value.TiltFrames, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.PitchInitial", pKey);
		ret |= read(value.PitchInitial, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.PitchFinal", pKey);
		ret |= read(value.PitchFinal, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.TurnRate", pKey);
		ret |= read(value.TurnRate, parser, pSection, pFlagName);

		// sic! integer read like a float.
		_snprintf_s(pFlagName, 0x3F, "%s.RaiseRate", pKey);
		float buffer;
		if (read(buffer, parser, pSection, pFlagName))
		{
			value.RaiseRate = Game::F2I(buffer);
			ret = true;
		}

		_snprintf_s(pFlagName, 0x3F, "%s.Acceleration", pKey);
		ret |= read(value.Acceleration, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.Altitude", pKey);
		ret |= read(value.Altitude, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.Damage", pKey);
		ret |= read(value.Damage, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.EliteDamage", pKey);
		ret |= read(value.EliteDamage, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.BodyLength", pKey);
		ret |= read(value.BodyLength, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.LazyCurve", pKey);
		ret |= read(value.LazyCurve, parser, pSection, pFlagName);

		_snprintf_s(pFlagName, 0x3F, "%s.Type", pKey);
		ret |= read(value.Type, parser, pSection, pFlagName);

		return ret;
	}

	template <>
	inline bool read<Leptons>(Leptons& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		double buffer;
		if (parser.ReadDouble(pSection, pKey, &buffer))
		{
			value = Leptons(Game::F2I(buffer * Unsorted::LeptonsPerCell));
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid floating point number");
		}
		return false;
	}

	template <>
	inline bool read<OwnerHouseKind>(OwnerHouseKind& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "default") == 0)
			{
				value = OwnerHouseKind::Default;
			}
			else if (_strcmpi(parser.value(), "invoker") == 0)
			{
				value = OwnerHouseKind::Invoker;
			}
			else if (_strcmpi(parser.value(), "killer") == 0)
			{
				value = OwnerHouseKind::Killer;
			}
			else if (_strcmpi(parser.value(), "victim") == 0)
			{
				value = OwnerHouseKind::Victim;
			}
			else if (_strcmpi(parser.value(), "civilian") == 0)
			{
				value = OwnerHouseKind::Civilian;
			}
			else if (_strcmpi(parser.value(), "special") == 0)
			{
				value = OwnerHouseKind::Special;
			}
			else if (_strcmpi(parser.value(), "neutral") == 0)
			{
				value = OwnerHouseKind::Neutral;
			}
			else if (_strcmpi(parser.value(), "random") == 0)
			{
				value = OwnerHouseKind::Random;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a owner house kind");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<Mission>(Mission& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto const mission = MissionControlClass::FindIndex(parser.value());
			if (mission != Mission::None)
			{
				value = mission;
				return true;
			}
			else if (!parser.empty())
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Invalid Mission name");
			}
		}
		return false;
	}

	template <>
	inline bool read<SuperWeaponAITargetingMode>(SuperWeaponAITargetingMode& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			static const auto Modes =
			{
				"none", "nuke", "lightningstorm", "psychicdominator", "paradrop",
				"geneticmutator", "forceshield", "notarget", "offensive", "stealth",
				"self", "base", "multimissile", "hunterseeker", "enemybase"
			};

			auto it = Modes.begin();
			for (auto i = 0u; i < Modes.size(); ++i)
			{
				if (_strcmpi(parser.value(), *it++) == 0)
				{
					value = static_cast<SuperWeaponAITargetingMode>(i);
					return true;
				}
			}

			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a targeting mode");
		}
		return false;
	}

	template <>
	inline bool read<AffectedTarget>(AffectedTarget& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = AffectedTarget::None;

			auto str = parser.value();
			char* context = nullptr;
			for (auto cur = strtok_s(str, Phobos::readDelims, &context); cur; cur = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				if (!_strcmpi(cur, "land"))
				{
					parsed |= AffectedTarget::Land;
				}
				else if (!_strcmpi(cur, "water"))
				{
					parsed |= AffectedTarget::Water;
				}
				else if (!_strcmpi(cur, "empty"))
				{
					parsed |= AffectedTarget::NoContent;
				}
				else if (!_strcmpi(cur, "infantry"))
				{
					parsed |= AffectedTarget::Infantry;
				}
				else if (!_strcmpi(cur, "units"))
				{
					parsed |= AffectedTarget::Unit;
				}
				else if (!_strcmpi(cur, "buildings"))
				{
					parsed |= AffectedTarget::Building;
				}
				else if (!_strcmpi(cur, "aircraft"))
				{
					parsed |= AffectedTarget::Aircraft;
				}
				else if (!_strcmpi(cur, "all"))
				{
					parsed |= AffectedTarget::All;
				}
				else if (_strcmpi(cur, "none"))
				{
					Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a super weapon target");
					return false;
				}
			}
			value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<AffectedHouse>(AffectedHouse& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = AffectedHouse::None;

			auto str = parser.value();
			char* context = nullptr;
			for (auto cur = strtok_s(str, Phobos::readDelims, &context); cur; cur = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				if (!_strcmpi(cur, "owner") || !_strcmpi(cur, "self"))
				{
					parsed |= AffectedHouse::Owner;
				}
				else if (!_strcmpi(cur, "allies") || !_strcmpi(cur, "ally"))
				{
					parsed |= AffectedHouse::Allies;
				}
				else if (!_strcmpi(cur, "enemies") || !_strcmpi(cur, "enemy"))
				{
					parsed |= AffectedHouse::Enemies;
				}
				else if (!_strcmpi(cur, "team"))
				{
					parsed |= AffectedHouse::Team;
				}
				else if (!_strcmpi(cur, "others"))
				{
					parsed |= AffectedHouse::NotOwner;
				}
				else if (!_strcmpi(cur, "all"))
				{
					parsed |= AffectedHouse::All;
				}
				else if (_strcmpi(cur, "none"))
				{
					Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a super weapon affected house");
					return false;
				}
			}
			value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<DisableWeaponCate>(DisableWeaponCate& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = DisableWeaponCate::None;

			auto str = parser.value();
			char* context = nullptr;
			for (auto cur = strtok_s(str, Phobos::readDelims, &context); cur; cur = strtok_s(nullptr, Phobos::readDelims, &context))
			{
				if (!_strcmpi(cur, "primary"))
				{
					parsed |= DisableWeaponCate::Primary;
				}
				else if (!_strcmpi(cur, "secondary"))
				{
					parsed |= DisableWeaponCate::Secondary;
				}
				else if (!_strcmpi(cur, "gattlingodd"))
				{
					parsed |= DisableWeaponCate::GattlingOdd;
				}
				else if (!_strcmpi(cur, "gattlingeven"))
				{
					parsed |= DisableWeaponCate::GattlingEven;
				}
				else if (!_strcmpi(cur, "death"))
				{
					parsed |= DisableWeaponCate::Death;
				}
				else if (!_strcmpi(cur, "attach"))
				{
					parsed |= DisableWeaponCate::Attach;
				}
				else if (!_strcmpi(cur, "attacked"))
				{
					parsed |= DisableWeaponCate::Attacked;
				}
				else if (!_strcmpi(cur, "feedback"))
				{
					parsed |= DisableWeaponCate::Feedback;
				}
				else if (!_strcmpi(cur, "passenger"))
				{
					parsed |= DisableWeaponCate::Passenger;
				}
				else if (!_strcmpi(cur, "cantfire"))
				{
					parsed |= DisableWeaponCate::CantFire;
				}
				else if (!_strcmpi(cur, "exceptdeath"))
				{
					parsed |= DisableWeaponCate::ExceptDeath;
				}
				else if (!_strcmpi(cur, "all"))
				{
					parsed |= DisableWeaponCate::All;
				}
				else if (_strcmpi(cur, "none"))
				{
					Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a weapon category");
					return false;
				}
			}
			value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<AttachedAnimFlag>(AttachedAnimFlag& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = AttachedAnimFlag::None;

			auto str = parser.value();

			if (_strcmpi(str, "hides") == 0)
			{
				parsed = AttachedAnimFlag::Hides;
			}
			else if (_strcmpi(str, "temporal") == 0)
			{
				parsed = AttachedAnimFlag::Temporal;
			}
			else if (_strcmpi(str, "paused") == 0)
			{
				parsed = AttachedAnimFlag::Paused;
			}
			else if (_strcmpi(str, "pausedtemporal") == 0)
			{
				parsed = AttachedAnimFlag::PausedTemporal;
			}
			else if (_strcmpi(str, "none"))
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a AttachedAnimFlag");
				return false;
			}
			value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<AreaFireTarget>(AreaFireTarget& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "base") == 0)
			{
				value = AreaFireTarget::Base;
			}
			else if (_strcmpi(parser.value(), "self") == 0)
			{
				value = AreaFireTarget::Self;
			}
			else if (_strcmpi(parser.value(), "random") == 0)
			{
				value = AreaFireTarget::Random;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected an area fire target");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<SelfHealGainType>(SelfHealGainType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "none") == 0)
			{
				value = SelfHealGainType::None;
			}
			else if (_strcmpi(parser.value(), "infantry") == 0)
			{
				value = SelfHealGainType::Infantry;
			}
			else if (_strcmpi(parser.value(), "units") == 0)
			{
				value = SelfHealGainType::Units;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a self heal gain type");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<SlaveChangeOwnerType>(SlaveChangeOwnerType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "suicide") == 0)
			{
				value = SlaveChangeOwnerType::Suicide;
			}
			else if (_strcmpi(parser.value(), "master") == 0)
			{
				value = SlaveChangeOwnerType::Master;
			}
			else if (_strcmpi(parser.value(), "neutral") == 0)
			{
				value = SlaveChangeOwnerType::Neutral;
			}
			else
			{
				if (_strcmpi(parser.value(), "killer") != 0)
					Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a slave ownership option, default killer");
				value = SlaveChangeOwnerType::Killer;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<AutoDeathBehavior>(AutoDeathBehavior& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "sell") == 0)
			{
				value = AutoDeathBehavior::Sell;
			}
			else if (_strcmpi(parser.value(), "vanish") == 0)
			{
				value = AutoDeathBehavior::Vanish;
			}
			else
			{
				if (_strcmpi(parser.value(), "kill") != 0)
					Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a self-destruction behavior, default to kill if set");
				value = AutoDeathBehavior::Kill;
			}

			return true;
		}
		return false;
	}

	template <>
	inline bool read<TextAlign>(TextAlign& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = TextAlign::None;
			auto str = parser.value();
			if (_strcmpi(str, "left") == 0)
			{
				parsed = TextAlign::Left;
			}
			else if (_strcmpi(str, "center") == 0)
			{
				parsed = TextAlign::Center;
			}
			else if (_strcmpi(str, "centre") == 0)
			{
				parsed = TextAlign::Center;
			}
			else if (_strcmpi(str, "right") == 0)
			{
				parsed = TextAlign::Right;
			}
			else if (_strcmpi(str, "none") == 0)
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Text Alignment can be either Left, Center/Centre or Right");
				return false;
			}
			if (parsed != TextAlign::None)
				value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<TextPrintType>(TextPrintType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = TextPrintType::LASTPOINT;
			auto str = parser.value();
			if (_strcmpi(str, "left") == 0)
			{
				parsed = TextPrintType::LASTPOINT;
			}
			else if (_strcmpi(str, "center") == 0)
			{
				parsed = TextPrintType::Center;
			}
			else if (_strcmpi(str, "centre") == 0)
			{
				parsed = TextPrintType::Center;
			}
			else if (_strcmpi(str, "right") == 0)
			{
				parsed = TextPrintType::Right;
			}
			else if (_strcmpi(str, "background") == 0)
			{
				parsed = TextPrintType::Background;
			}
			else if (_strcmpi(str, "gradall") == 0)
			{
				parsed = TextPrintType::GradAll;
			}
			else if (_strcmpi(str, "noshadow") == 0)
			{
				parsed = TextPrintType::NoShadow;
			}
			if (parsed != TextPrintType::LASTPOINT)
				value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<BannerNumberType>(BannerNumberType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = BannerNumberType::None;
			auto str = parser.value();
			if (_strcmpi(str, "variable") == 0)
			{
				parsed = BannerNumberType::Variable;
			}
			else if (_strcmpi(str, "prefixed") == 0)
			{
				parsed = BannerNumberType::Prefixed;
			}
			else if (_strcmpi(str, "suffixed") == 0)
			{
				parsed = BannerNumberType::Suffixed;
			}
			else if (_strcmpi(str, "fraction") == 0)
			{
				parsed = BannerNumberType::Fraction;
			}
			else if (_strcmpi(str, "none") == 0)
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(),
					"Content.VariableFormat can be either none, prefixed, suffixed or fraction");
				return false;
			}
			if (parsed != BannerNumberType::None)
				value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<IronCurtainEffect>(IronCurtainEffect& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto parsed = IronCurtainEffect::Kill;
			auto str = parser.value();
			if (_strcmpi(str, "invulnerable") == 0)
			{
				parsed = IronCurtainEffect::Invulnerable;
			}
			else if (_strcmpi(str, "ignore") == 0)
			{
				parsed = IronCurtainEffect::Ignore;
			}
			else if (_strcmpi(str, "kill") != 0)
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "IronCurtainEffect can be either kill, invulnerable or ignore");
				return false;
			}
			value = parsed;
			return true;
		}
		return false;
	}

	template <>
	inline bool read<HorizontalPosition>(HorizontalPosition& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto str = parser.value();
			if (_strcmpi(str, "left") == 0)
			{
				value = HorizontalPosition::Left;
			}
			else if (_strcmpi(str, "center") == 0 || _strcmpi(str, "centre") == 0)
			{
				value = HorizontalPosition::Center;
			}
			else if (_strcmpi(str, "right") == 0)
			{
				value = HorizontalPosition::Right;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, str, "Horizontal Position can be either Left, Center/Centre or Right");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<VerticalPosition>(VerticalPosition& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto str = parser.value();
			if (_strcmpi(str, "top") == 0)
			{
				value = VerticalPosition::Top;
			}
			else if (_strcmpi(str, "center") == 0 || _strcmpi(str, "centre") == 0)
			{
				value = VerticalPosition::Center;
			}
			else if (_strcmpi(str, "bottom") == 0)
			{
				value = VerticalPosition::Bottom;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, str, "Vertical Position can be either Top, Center/Centre or Bottom");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<BuildingSelectBracketPosition>(BuildingSelectBracketPosition& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto str = parser.value();
			if (_strcmpi(str, "top") == 0)
			{
				value = BuildingSelectBracketPosition::Top;
			}
			else if (_strcmpi(str, "lefttop") == 0)
			{
				value = BuildingSelectBracketPosition::LeftTop;
			}
			else if (_strcmpi(str, "leftbottom") == 0)
			{
				value = BuildingSelectBracketPosition::LeftBottom;
			}
			else if (_strcmpi(str, "bottom") == 0)
			{
				value = BuildingSelectBracketPosition::Bottom;
			}
			else if (_strcmpi(str, "rightbottom") == 0)
			{
				value = BuildingSelectBracketPosition::RightBottom;
			}
			else if (_strcmpi(str, "righttop") == 0)
			{
				value = BuildingSelectBracketPosition::RightTop;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, str, "BuildingPosition is invalid");
				return false;
			}
			return true;
		}

		return false;
	}

	template <>
	inline bool read<DisplayInfoType>(DisplayInfoType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto str = parser.value();
			if (_strcmpi(str, "health") == 0)
			{
				value = DisplayInfoType::Health;
			}
			else if (_strcmpi(str, "shield") == 0)
			{
				value = DisplayInfoType::Shield;
			}
			else if (_strcmpi(str, "ammo") == 0)
			{
				value = DisplayInfoType::Ammo;
			}
			else if (_strcmpi(str, "mindcontrol") == 0)
			{
				value = DisplayInfoType::MindControl;
			}
			else if (_strcmpi(str, "spawns") == 0)
			{
				value = DisplayInfoType::Spawns;
			}
			else if (_strcmpi(str, "passengers") == 0)
			{
				value = DisplayInfoType::Passengers;
			}
			else if (_strcmpi(str, "tiberium") == 0)
			{
				value = DisplayInfoType::Tiberium;
			}
			else if (_strcmpi(str, "experience") == 0)
			{
				value = DisplayInfoType::Experience;
			}
			else if (_strcmpi(str, "occupants") == 0)
			{
				value = DisplayInfoType::Occupants;
			}
			else if (_strcmpi(str, "gattlingstage") == 0)
			{
				value = DisplayInfoType::GattlingStage;
			}
			else if (_strcmpi(str, "temperature")==0)
			{
				value = DisplayInfoType::Temperature;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, str, "Display info type is illegal");
				return false;
			}

			return true;
		}

		return false;
	}

	template <>
	inline bool read<Locomotors>(Locomotors& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if ((_strcmpi(parser.value(), "Drive") == 0)
				|| (_strcmpi(parser.value(), "{4A582741-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Drive;
			}
			else if ((_strcmpi(parser.value(), "Jumpjet") == 0)
				|| (_strcmpi(parser.value(), "{92612C46-F71F-11d1-AC9F-006008055BB5}") == 0))
			{
				value = Locomotors::Jumpjet;
			}
			else if ((_strcmpi(parser.value(), "Hover") == 0)
				|| (_strcmpi(parser.value(), "{4A582742-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Hover;
			}
			else if ((_strcmpi(parser.value(), "Rocket") == 0)
				|| (_strcmpi(parser.value(), "{B7B49766-E576-11d3-9BD9-00104B972FE8}") == 0))
			{
				value = Locomotors::Rocket;
			}
			else if ((_strcmpi(parser.value(), "Tunnel") == 0)
				|| (_strcmpi(parser.value(), "{4A582743-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Tunnel;
			}
			else if ((_strcmpi(parser.value(), "Walk") == 0)
				|| (_strcmpi(parser.value(), "{4A582744-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Walk;
			}
			else if ((_strcmpi(parser.value(), "Droppod") == 0)
				|| (_strcmpi(parser.value(), "{4A582745-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Droppod;
			}
			else if ((_strcmpi(parser.value(), "Fly") == 0)
				|| (_strcmpi(parser.value(), "{4A582746-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Fly;
			}
			else if ((_strcmpi(parser.value(), "Teleport") == 0)
				|| (_strcmpi(parser.value(), "{4A582747-9839-11d1-B709-00A024DDAFD1}") == 0))
			{
				value = Locomotors::Teleport;
			}
			else if ((_strcmpi(parser.value(), "Mech") == 0)
				|| (_strcmpi(parser.value(), "{55D141B8-DB94-11d1-AC98-006008055BB5}") == 0))
			{
				value = Locomotors::Mech;
			}
			else if ((_strcmpi(parser.value(), "Ship") == 0)
				|| (_strcmpi(parser.value(), "{2BEA74E1-7CCA-11d3-BE14-00104B62A16C}") == 0))
			{
				value = Locomotors::Ship;
			}
			else
			{
				return false;
			}

			return true;
		}
		return false;
	}

	template <>
	inline bool read<ShowTimerType>(ShowTimerType& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if ((_strcmpi(parser.value(), "Hour") == 0))
			{
				value = ShowTimerType::Hour;
			}
			else if ((_strcmpi(parser.value(), "Minute") == 0))
			{
				value = ShowTimerType::Minute;
			}
			else if ((_strcmpi(parser.value(), "Second") == 0))
			{
				value = ShowTimerType::Second;
			}
			else
			{
				return false;
			}

			return true;
		}
		return false;
	}

	template <>
	inline bool read<AresOwnerEnum>(AresOwnerEnum& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			const char* str = parser.value();
			if (_strcmpi(str, "default") == 0)
			{
				value = AresOwnerEnum::Default;
			}
			else if (_strcmpi(str, "invoker") == 0)
			{
				value = AresOwnerEnum::Invoker;
			}
			else if (_strcmpi(str, "killer") == 0)
			{
				value = AresOwnerEnum::Killer;
			}
			else if (_strcmpi(str, "victim") == 0)
			{
				value = AresOwnerEnum::Victim;
			}
			else if (_strcmpi(str, "civilian") == 0)
			{
				value = AresOwnerEnum::Civilian;
			}
			else if (_strcmpi(str, "special") == 0)
			{
				value = AresOwnerEnum::Special;
			}
			else if (_strcmpi(str, "neutral") == 0)
			{
				value = AresOwnerEnum::Neutral;
			}
			else if (_strcmpi(str, "random") == 0)
			{
				value = AresOwnerEnum::Random;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, str, "Expected a owner house kind");
				return false;
			}

			return true;
		}
		return false;
	}

	template <>
	inline bool read<AttachmentYSortPosition>(AttachmentYSortPosition& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "default") == 0)
			{
				value = AttachmentYSortPosition::Default;
			}
			else if (_strcmpi(parser.value(), "underparent") == 0)
			{
				value = AttachmentYSortPosition::UnderParent;
			}
			else if (_strcmpi(parser.value(), "overparent") == 0)
			{
				value = AttachmentYSortPosition::OverParent;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected an attachment YSort position");
				return false;
			}
			return true;
		}
		return false;
	}

	template <>
	inline bool read<ShowMessageHouse>(ShowMessageHouse& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (parser.ReadString(pSection, pKey))
		{
			if (_strcmpi(parser.value(), "victim") == 0)
			{
				value = ShowMessageHouse::Victim;
			}
			else if (_strcmpi(parser.value(), "invoker") == 0)
			{
				value = ShowMessageHouse::Invoker;
			}
			else
			{
				value = ShowMessageHouse::All;
			}

			return true;
		}
		return false;
	}

	template <>
	inline bool read<TranslucencyLevel>(TranslucencyLevel& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		return value.Read(parser, pSection, pKey);
	}

	template <>
	inline bool read<CLSID>(CLSID& value, INI_EX& parser, const char* pSection, const char* pKey, bool allocate)
	{
		if (!parser.ReadString(pSection, pKey))
			return false;

		// Semantic locomotor aliases
		if (parser.value()[0] != '{')
		{
#define PARSE_IF_IS_LOCO(name)\
if(_strcmpi(parser.value(), #name) == 0){ value = LocomotionClass::CLSIDs::name; return true; }

			PARSE_IF_IS_LOCO(Drive);
			PARSE_IF_IS_LOCO(Jumpjet);
			PARSE_IF_IS_LOCO(Hover);
			PARSE_IF_IS_LOCO(Rocket);
			PARSE_IF_IS_LOCO(Tunnel);
			PARSE_IF_IS_LOCO(Walk);
			PARSE_IF_IS_LOCO(Fly);
			PARSE_IF_IS_LOCO(Teleport);
			PARSE_IF_IS_LOCO(Mech);
			PARSE_IF_IS_LOCO(Ship);
			PARSE_IF_IS_LOCO(Droppod);

#undef PARSE_IF_IS_LOCO

			return false;
		}

		CHAR bytestr[128];
		WCHAR wcharstr[128];

		strncpy(bytestr, parser.value(), 128);
		bytestr[127] = NULL;
		CRT::strtrim(bytestr);
		if (!strlen(bytestr))
			return false;

		MultiByteToWideChar(0, 1, bytestr, -1, wcharstr, 128);
		if (CLSIDFromString(wcharstr, &value) < 0)
			return false;

		return true;
	}

	template <typename T>
	void parse_values(std::vector<T>& vector, INI_EX& parser, const char* pSection, const char* pKey)
	{
		char* context = nullptr;
		for (auto pCur = strtok_s(parser.value(), Phobos::readDelims, &context); pCur; pCur = strtok_s(nullptr, Phobos::readDelims, &context))
		{
			T buffer = T();
			if (Parser<T>::Parse(pCur, &buffer))
			{
				vector.push_back(buffer);
			}
			else if (!INIClass::IsBlank(pCur))
			{
				Debug::INIParseFailed(pSection, pKey, pCur);
			}
		}
	}

	template <>
	inline void parse_values<Leptons>(std::vector<Leptons>& vector, INI_EX& parser, const char* pSection, const char* pKey)
	{
		char* context = nullptr;
		for (auto pCur = strtok_s(parser.value(), Phobos::readDelims, &context); pCur; pCur = strtok_s(nullptr, Phobos::readDelims, &context))
		{
			double value = atof(pCur);
			vector.emplace_back(Game::F2I(value * Unsorted::LeptonsPerCell));
		}
	}

	template <typename Lookuper, typename T>
	void parse_indexes(std::vector<T>& vector, INI_EX& parser, const char* pSection, const char* pKey)
	{
		char* context = nullptr;
		for (auto pCur = strtok_s(parser.value(), Phobos::readDelims, &context); pCur; pCur = strtok_s(nullptr, Phobos::readDelims, &context))
		{
			int idx = Lookuper::FindIndex(pCur);
			if (idx != -1)
			{
				vector.push_back(idx);
			}
			else if (!INIClass::IsBlank(pCur))
			{
				Debug::INIParseFailed(pSection, pKey, pCur);
			}
		}
	}
}


// Valueable

template <typename T>
void __declspec(noinline) Valueable<T>::Read(INI_EX& parser, const char* pSection, const char* pKey, bool Allocate)
{
	detail::read(this->Value, parser, pSection, pKey, Allocate);
}

template <typename T>
bool Valueable<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadPhobosStream(Stm, this->Value, RegisterForChange);
}

template <typename T>
bool Valueable<T>::Save(PhobosStreamWriter& Stm) const
{
	return Savegame::WritePhobosStream(Stm, this->Value);
}


// ValueableIdx

template <typename Lookuper>
void __declspec(noinline) ValueableIdx<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		const char* val = parser.value();
		int idx = Lookuper::FindIndex(val);
		if (idx != -1 || INIClass::IsBlank(val))
		{
			this->Value = idx;
		}
		else
		{
			Debug::INIParseFailed(pSection, pKey, val);
		}
	}
}


// Nullable

template <typename T>
void __declspec(noinline) Nullable<T>::Read(INI_EX& parser, const char* pSection, const char* pKey, bool Allocate)
{
	if (detail::read(this->Value, parser, pSection, pKey, Allocate))
	{
		this->HasValue = true;
	}
}

template <typename T>
bool Nullable<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->Reset();
	auto ret = Savegame::ReadPhobosStream(Stm, this->HasValue);
	if (ret && this->HasValue)
	{
		ret = Savegame::ReadPhobosStream(Stm, this->Value, RegisterForChange);
	}
	return ret;
}

template <typename T>
bool Nullable<T>::Save(PhobosStreamWriter& Stm) const
{
	auto ret = Savegame::WritePhobosStream(Stm, this->HasValue);
	if (this->HasValue)
	{
		ret = Savegame::WritePhobosStream(Stm, this->Value);
	}
	return ret;
}


// NullableIdx

template <typename Lookuper>
void __declspec(noinline) NullableIdx<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		const char* val = parser.value();
		int idx = Lookuper::FindIndex(val);
		if (idx != -1 || INIClass::IsBlank(val))
		{
			this->Value = idx;
			this->HasValue = true;
		}
		else
		{
			Debug::INIParseFailed(pSection, pKey, val);
		}
	}
}


// Promotable

template <typename T>
void __declspec(noinline) Promotable<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{

	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");
	if (res > 0 && flagName[res - 1] == '.')
	{
		flagName[res - 1] = '\0';
	}

	T placeholder = T();
	if (detail::read(placeholder, parser, pSection, flagName))
	{
		this->SetAll(placeholder);
	}

	// read specific flags
	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Rookie");
	detail::read(this->Rookie, parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Veteran");
	detail::read(this->Veteran, parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Elite");
	detail::read(this->Elite, parser, pSection, flagName);
};

template <typename T>
bool Promotable<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadPhobosStream(Stm, this->Rookie, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->Veteran, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->Elite, RegisterForChange);
}

template <typename T>
bool Promotable<T>::Save(PhobosStreamWriter& Stm) const
{
	return Savegame::WritePhobosStream(Stm, this->Rookie)
		&& Savegame::WritePhobosStream(Stm, this->Veteran)
		&& Savegame::WritePhobosStream(Stm, this->Elite);
}


// ValueableVector

template <typename T>
void __declspec(noinline) ValueableVector<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		detail::parse_values<T>(*this, parser, pSection, pKey);
	}
}

template <typename T>
bool ValueableVector<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	size_t size = 0;
	if (Savegame::ReadPhobosStream(Stm, size, RegisterForChange))
	{
		this->clear();
		this->resize(size);

		for (size_t i = 0; i < size; ++i)
		{
			T& item = this->at(i);

			if (!Savegame::ReadPhobosStream(Stm, item, RegisterForChange))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template <>
bool ValueableVector<bool>::Load(PhobosStreamReader& stm, bool registerForChange)
{
	size_t size = 0;
	if (Savegame::ReadPhobosStream(stm, size, registerForChange))
	{
		this->clear();

		for (size_t i = 0; i < size; ++i)
		{
			bool value;

			if (!Savegame::ReadPhobosStream(stm, value, false))
				return false;

			this->emplace_back(value);
		}
		return true;
	}
	return false;
}

template <typename T>
bool ValueableVector<T>::Save(PhobosStreamWriter& Stm) const
{
	auto size = this->size();
	if (Savegame::WritePhobosStream(Stm, size))
	{
		for (auto const& item : *this)
		{
			if (!Savegame::WritePhobosStream(Stm, item))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template <>
bool ValueableVector<bool>::Save(PhobosStreamWriter& stm) const
{
	auto size = this->size();
	if (Savegame::WritePhobosStream(stm, size))
	{
		for (bool item : *this)
		{
			if (!Savegame::WritePhobosStream(stm, item))
				return false;
		}
		return true;
	}
	return false;
}

// NullableVector

template <typename T>
void __declspec(noinline) NullableVector<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();

		auto const non_default = _strcmpi(parser.value(), "<default>") != 0;
		this->hasValue = non_default;

		if (non_default)
		{
			detail::parse_values<T>(*this, parser, pSection, pKey);
		}
	}
}

template <typename T>
bool NullableVector<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->clear();
	if (Savegame::ReadPhobosStream(Stm, this->hasValue, RegisterForChange))
	{
		return !this->hasValue || ValueableVector<T>::Load(Stm, RegisterForChange);
	}
	return false;
}

template <typename T>
bool NullableVector<T>::Save(PhobosStreamWriter& Stm) const
{
	if (Savegame::WritePhobosStream(Stm, this->hasValue))
	{
		return !this->hasValue || ValueableVector<T>::Save(Stm);
	}
	return false;
}


// ValueableIdxVector

template <typename Lookuper>
void __declspec(noinline) ValueableIdxVector<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		detail::parse_indexes<Lookuper>(*this, parser, pSection, pKey);
	}
}


// NullableIdxVector

template <typename Lookuper>
void __declspec(noinline) NullableIdxVector<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();

		auto const non_default = _strcmpi(parser.value(), "<default>") != 0;
		this->hasValue = non_default;

		if (non_default)
		{
			detail::parse_indexes<Lookuper>(*this, parser, pSection, pKey);
		}
	}
}

// Damageable

template <typename T>
void __declspec(noinline) Damageable<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{
	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");

	if (res > 0 && flagName[res - 1] == '.')
	{
		flagName[res - 1] = '\0';
	}

	this->BaseValue.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionYellow");
	this->ConditionYellow.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionRed");
	this->ConditionRed.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, "MaxValue");
	this->MaxValue.Read(parser, pSection, flagName);
};

template <typename T>
bool Damageable<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadPhobosStream(Stm, this->BaseValue, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->ConditionYellow, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->ConditionRed, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->MaxValue, RegisterForChange);
}

template <typename T>
bool Damageable<T>::Save(PhobosStreamWriter& Stm) const
{
	return Savegame::WritePhobosStream(Stm, this->BaseValue)
		&& Savegame::WritePhobosStream(Stm, this->ConditionYellow)
		&& Savegame::WritePhobosStream(Stm, this->ConditionRed)
		&& Savegame::WritePhobosStream(Stm, this->MaxValue);
}

// DamageableVector

template <typename T>
void __declspec(noinline) DamageableVector<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{
	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");

	if (res > 0 && flagName[res - 1] == '.')
	{
		flagName[res - 1] = '\0';
	}

	this->BaseValue.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionYellow");
	this->ConditionYellow.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionRed");
	this->ConditionRed.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "MaxValue");
	this->MaxValue.Read(parser, pSection, flagName);
};

template <typename T>
bool DamageableVector<T>::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadPhobosStream(Stm, this->BaseValue, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->ConditionYellow, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->ConditionRed, RegisterForChange)
		&& Savegame::ReadPhobosStream(Stm, this->MaxValue, RegisterForChange);
}

template <typename T>
bool DamageableVector<T>::Save(PhobosStreamWriter& Stm) const
{
	return Savegame::WritePhobosStream(Stm, this->BaseValue)
		&& Savegame::WritePhobosStream(Stm, this->ConditionYellow)
		&& Savegame::WritePhobosStream(Stm, this->ConditionRed)
		&& Savegame::WritePhobosStream(Stm, this->MaxValue);
}

// PromotableVector

template <typename T>
void __declspec(noinline) PromotableVector<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{
	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");

	if (res > 0 && flagName[res - 1] == '.')
	{
		flagName[res - 1] = '\0';
	}

	this->Base.Read(parser, pSection, flagName);

	NullableVector<T> veteran;
	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Veteran");
	veteran.Read(parser, pSection, flagName);

	NullableVector<T> elite;
	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Elite");
	elite.Read(parser, pSection, flagName);

	if (veteran.HasValue())
	{
		for (size_t i = 0; i < veteran.size(); i++)
		{
			this->Veteran[i] = veteran[i];
		}
	}

	if (elite.HasValue())
	{
		for (size_t i = 0; i < elite.size(); i++)
		{
			this->Elite[i] = elite[i];
		}
	}
}

template <typename T>
void __declspec(noinline) PromotableVector<T>::ReadList(INI_EX& parser, const char* pSection, const char* pFlag, bool allocate)
{
	bool numFirst = false;
	int flagLength = strlen(pFlag);

	for (int i = 1; i < flagLength; i++)
	{
		if (pFlag[i - 1] == '%')
		{
			if (pFlag[i] == 'd')
			{
				numFirst = true;
			}
			else
			{
				break;
			}
		}
	}

	char flag[0x40] = { '\0' };

	for (int i = 0;; i++)
	{
		Nullable<T> value;
		int res = 0;

		if (numFirst)
			res = _snprintf_s(flag, _TRUNCATE, pFlag, i, "");
		else
			res = _snprintf_s(flag, _TRUNCATE, pFlag, "", i);

		if (res > 0 && flag[res - 1] == '.')
			flag[res - 1] = '\0';

		if (flag[0] == '.')
			strcpy_s(flag, flag + 1);

		value.Read(parser, pSection, flag, allocate);

		if (!value.isset())
			break;

		this->Base.emplace_back(value.Get());
	}

	int size = static_cast<int>(this->Base.size());

	for (int i = 0; i < size; i++)
	{
		Nullable<T> veteran;
		Nullable<T> elite;

		if (numFirst)
			_snprintf_s(flag, _TRUNCATE, pFlag, i, "Veteran");
		else
			_snprintf_s(flag, _TRUNCATE, pFlag, "Veteran", i);

		veteran.Read(parser, pSection, flag, allocate);

		if (numFirst)
			_snprintf_s(flag, _TRUNCATE, pFlag, i, "Elite");
		else
			_snprintf_s(flag, _TRUNCATE, pFlag, "Elite", i);

		elite.Read(parser, pSection, flag, allocate);

		if (veteran.isset())
			this->Veteran.emplace(i, veteran.Get());

		if (elite.isset())
			this->Elite.emplace(i, elite.Get());
	}
}

template <typename T>
bool PromotableVector<T>::Load(PhobosStreamReader& stm, bool registerForChange)
{
	return Savegame::ReadPhobosStream(stm, this->Base, registerForChange)
		&& Savegame::ReadPhobosStream(stm, this->Veteran, registerForChange)
		&& Savegame::ReadPhobosStream(stm, this->Elite, registerForChange);
}

template <typename T>
bool PromotableVector<T>::Save(PhobosStreamWriter& stm) const
{
	return Savegame::WritePhobosStream(stm, this->Base)
		&& Savegame::WritePhobosStream(stm, this->Veteran)
		&& Savegame::WritePhobosStream(stm, this->Elite);
}
