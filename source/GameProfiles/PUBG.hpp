#pragma once
#include "IGameProfileAndroid.hpp"

class PUBGProfile : public IGameProfileAndroid
{
public:
    PUBGProfile() = default;

    std::string GetAppName() const override
    {
        return "PUBG";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {
            "com.tencent.ig",
            "com.rekoo.pubgm",
            "com.pubg.imobile",
            "com.pubg.krmobile",
            "com.vng.pubgmobile",
        };
    }

    bool isUsingCasePreservingName() const override
    {
        return false;
    }

    bool IsUsingFNamePool() const override
    {
        return false;
    }

    bool isUsingOutlineNumberName() const override
    {
        return false;
    }

    bool IsFNamePassedByValue() const override
    {
        return true;
    }

    uintptr_t GetGUObjectArrayPtr() const override
    {
        if (uintptr_t a = FindGUObjectArrayViaFinishDestroy())
            return a;

        std::string ida_pattern = "12 40 B9 ? 3E 40 B9 ? ? ? 6B ? ? ? 54 ? ? ? ? ? ? ? 91";
        const int step = 0xf;

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        uintptr_t adrl = Arm64::DecodeADRL(findIdaPattern(map_type, ida_pattern, step));
        return IsObjectArray(adrl) ? adrl : 0;
    }

    uintptr_t GetNamesPtr() const override
    {
        return GetUnrealELF().base();
    }

    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE4_18_19(isUsingCasePreservingName());

        static bool once = false;
        if (!once)
        {
            once = true;
            offsets.FNameEntry.Index = sizeof(void *);
            offsets.FNameEntry.Name = sizeof(void *) + sizeof(int32_t);
        }

        return &offsets;
    }

    std::string GetNameByID(int32_t id) const override
    {
        if (FindFNameToString())
        {
            return GetNameByID_ViaToString(id);
        }
        return IGameProfile::GetNameByID(id);
    }
};
