#include "Dump.hpp"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <unordered_map>
#include <vector>

#include "Dumper.hpp"
#include "Prober.hpp"
#include "UE/UEMemory.hpp"
#include "Utils/Logger.hpp"

#include "UE/UEGameProfiles/ArkUltimate.hpp"
#include "UE/UEGameProfiles/Auroria.hpp"
#include "UE/UEGameProfiles/BabyYellow.hpp"
#include "UE/UEGameProfiles/BlackClover.hpp"
#include "UE/UEGameProfiles/BladeSoul.hpp"
#include "UE/UEGameProfiles/Case2.hpp"
#include "UE/UEGameProfiles/Century.hpp"
#include "UE/UEGameProfiles/Dislyte.hpp"
#include "UE/UEGameProfiles/Farlight.hpp"
#include "UE/UEGameProfiles/HelloNeighbor.hpp"
#include "UE/UEGameProfiles/HelloNeighborND.hpp"
#include "UE/UEGameProfiles/Injustice2.hpp"
#include "UE/UEGameProfiles/KingArthur.hpp"
#include "UE/UEGameProfiles/Lineage2.hpp"
#include "UE/UEGameProfiles/LineageW.hpp"
#include "UE/UEGameProfiles/MortalKombat.hpp"
#include "UE/UEGameProfiles/NightCrows.hpp"
#include "UE/UEGameProfiles/OdinValhalla.hpp"
#include "UE/UEGameProfiles/PES.hpp"
#include "UE/UEGameProfiles/RLSideswipe.hpp"
#include "UE/UEGameProfiles/RealBoxing2.hpp"
#include "UE/UEGameProfiles/RooftopsParkour.hpp"
#include "UE/UEGameProfiles/SFG2.hpp"
#include "UE/UEGameProfiles/Torchlight.hpp"
#include "UE/UEGameProfiles/TowerFantasy.hpp"
#include "UE/UEGameProfiles/WutheringWaves.hpp"

#include "GameProfiles/ArenaBreakout.hpp"
#include "GameProfiles/DeltaForce.hpp"
#include "GameProfiles/NiZhan.hpp"
#include "GameProfiles/PUBG.hpp"
#include "GameProfiles/PUBGMHD.hpp"
#include "GameProfiles/RocoKingdom.hpp"
#include "GameProfiles/Valorant.hpp"

using namespace UEMemory;

namespace
{
    IGameProfile *FindProfile(const std::string &package)
    {
        static std::vector<IGameProfile *> profiles = {
            new PESProfile(),
            new DislyteProfile(),
            new MortalKombatProfile(),
            new FarlightProfile(),
            new TorchlightProfile(),
            new BlackCloverProfile(),
            new WutheringWavesProfile(),
            new RealBoxing2Profile(),
            new OdinValhallaProfile(),
            new Injustice2Profile(),
            new RooftopParkourProfile(),
            new BabyYellowProfile(),
            new TowerFantasyProfile(),
            new BladeSoulProfile(),
            new Lineage2Profile(),
            new Case2Profile(),
            new CenturyProfile(),
            new KingArthurProfile(),
            new NightCrowsProfile(),
            new HelloNeighborProfile(),
            new HelloNeighborNDProfile(),
            new SFG2Profile(),
            new ArkUltimateProfile(),
            new AuroriaProfile(),
            new LineageWProfile(),
            new RLSideswipeProfile(),
            new ArenaBreakoutProfile(),
            new DeltaForceProfile(),
            new NiZhanProfile(),
            new PUBGProfile(),
            new PUBGMHDProfile(),
            new RocoKingdomProfile(),
            new ValorantProfile(),
        };

        for (auto *profile : profiles)
        {
            for (const auto &id : profile->GetAppIDs())
            {
                if (id == package) return profile;
            }
        }
        return nullptr;
    }
}  // namespace

bool DumpGame(pid_t pid, const std::string &package, const std::string &outputDir)
{
    IGameProfile *profile = FindProfile(package);
    if (!profile)
    {
        LOGE("No profile for %s.", package.c_str());
        return false;
    }
    LOGI("Game: %s | %s | pid %d", profile->GetAppName().c_str(), package.c_str(), pid);

    // /proc/<pid>/mem also reads pages the game protects with PROT_NONE.
    if (!kMgr.initialize(pid, EK_MEM_OP_IO, false) && !kMgr.initialize(pid, EK_MEM_OP_SYSCALL, false))
    {
        LOGE("Failed to initialize KittyMemoryMgr.");
        return false;
    }

    UEVarsInitStatus status = profile->InitUEVars();
    if (status != UEVarsInitStatus::SUCCESS)
    {
        LOGE("InitUEVars: %s", UEVars::InitStatusToStr(status).c_str());
        return false;
    }

    auto start = std::chrono::steady_clock::now();
    ProbeOffsets(profile);

    UEDumper dumper{};
    std::unordered_map<std::string, BufferFmt> buffers;
    if (!dumper.Init(profile) || !dumper.Dump(&buffers) || buffers.empty())
    {
        LOGE("Dump failed: %s", dumper.GetLastError().c_str());
        return false;
    }

    const std::string dumpDir = outputDir + "/UEDump3r/" + package;
    IOUtils::delete_directory(dumpDir);
    if (IOUtils::mkdir_recursive(dumpDir, 0777) == -1)
    {
        LOGE("Couldn't create %s: %s", dumpDir.c_str(), strerror(errno));
        return false;
    }
    for (const auto &[name, buffer] : buffers)
    {
        const std::string path = dumpDir + "/" + name;
        IOUtils::mkdir_recursive(path.substr(0, path.find_last_of('/')), 0777);
        if (!buffer.writeBufferToFile(path))
            LOGE("Couldn't write %s", path.c_str());
    }

    std::chrono::duration<float> duration = std::chrono::steady_clock::now() - start;
    LOGI("Dumped %zu files in %.1fs to %s", buffers.size(), duration.count(), dumpDir.c_str());
    return true;
}
