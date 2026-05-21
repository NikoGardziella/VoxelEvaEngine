#include "Engine/Core/Window.h"

#ifdef VE_PLATFORM_WINDOWS
#include "Engine/Platform/WindowsWindow.h"
#endif

#include <stdexcept>

namespace Engine
{
    Window* Window::Create(const WindowSpecification& specification)
    {
#ifdef VE_PLATFORM_WINDOWS
        return new WindowsWindow(specification);
#else
        throw std::runtime_error("Window::Create is not implemented for this platform");
#endif
    }
}