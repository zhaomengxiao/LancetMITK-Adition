#include "unitls.h"


bool unitls::keyboard::isKeyRelease(int vk_key)
{
    return !IS_KEY_DOWN(vk_key);
}

bool unitls::keyboard::isKeyPress(int vk_key)
{
    return IS_KEY_DOWN(vk_key);
}
