#include "binaryoffsethelper.hpp"
#include "oe/oe_displayversion.hpp"

namespace helpers
{
    char sGameVersion[0x10] = {};
    u32 sGameVersionIndex = 0xffff'ffff;

    u32 InitializeGameVersion()
    {
        /* Get Version */
        nn::oe::DisplayVersion displayVersion;
        nn::oe::GetDisplayVersion(&displayVersion);
        strcpy(sGameVersion, displayVersion.name);

        sGameVersionIndex = 0xffff'ffff;

        if (strncmp(sGameVersion, GAME_VERSION_3_0_2, 16) == 0)
        {
          sGameVersionIndex = 1;
        }
        else if (strncmp(sGameVersion, GAME_VERSION_3_0_3, 16) == 0)
        {
          sGameVersionIndex = 2;
        }
        return sGameVersionIndex;
    }
}
