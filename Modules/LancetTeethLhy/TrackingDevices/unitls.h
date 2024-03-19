#ifndef UNITLS_H
#define UNITLS_H


#if defined(Q_OS_WINDOWS)
    #include <windows.h>
    #define IS_KEY_DOWN(VK_WIN32_KEY)   ((GetAsyncKeyState(VK_WIN32_KEY) & 0x8000) ? 1:0)
#else
    #define IS_KEY_DOWN(VK_WIN32_KEY)    true
#endif

namespace unitls
{
#if defined(__cplusplus)
    extern "C"
    {
#endif
    namespace keyboard
    {
        enum vk
        {
            Key_A = 0x41,
            Key_B,
            Key_C,
            Key_D,
            Key_E,
            Key_F,
            Key_G,
            Key_H,
            Key_I,
            Key_J,
            Key_K,
            Key_L,
            Key_M,
            Key_N,
            Key_O,
            Key_P,
            Key_Q,
            Key_R,
            Key_S,
            Key_T,
            Key_U,
            Key_V,
            Key_W,
            Key_X,
            Key_Y
        };

        bool isKeyPress(int vk_key);

        bool isKeyRelease(int vk_key);

    } // namespace keyboard
#if defined(__cplusplus)
} // extern "C"
#endif

} // namespace unitls

#endif // UNITLS_H
