#pragma once

namespace Engine
{
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
      
    };
}