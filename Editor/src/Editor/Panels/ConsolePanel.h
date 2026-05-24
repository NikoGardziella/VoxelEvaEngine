#pragma once

namespace Editor
{
    class ConsolePanel
    {
    public:
        ConsolePanel() = default;

        void OnImGuiRender();

    private:
        bool m_autoScroll = true;
    };
}