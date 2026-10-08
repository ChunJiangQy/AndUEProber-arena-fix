#include <atomic>
#include <jni.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>

#include <KittyUtils.hpp>

#include "Dump.hpp"

// Gives the engine time to register its startup objects after injection.
constexpr unsigned kWaitSeconds = 8;

// AndKittyInjector passes 1337 as the reserved argument; loading the library
// any other way does nothing.
extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM *, void *key)
{
    static std::atomic<bool> started{false};
    if (key == reinterpret_cast<void *>(1337) && !started.exchange(true))
    {
        std::thread([]
        {
            sleep(kWaitSeconds);
            const std::string package = getprogname();
            DumpGame(getpid(), package, KittyUtils::Android::getAppExternalFilesDir(package));
        }).detach();
    }
    return JNI_VERSION_1_6;
}
