#pragma once

class IGameProfile;

// Replaces the profile's reflection offsets with the values found in live
// memory. A field that cannot be found keeps the profile's value. The profile's
// UE vars must be initialized, since objects and names are read through them.
void ProbeOffsets(IGameProfile *profile);
