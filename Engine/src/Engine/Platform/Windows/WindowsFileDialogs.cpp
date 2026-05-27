#include "FileDialogs.h"

#include <Windows.h>
#include <commdlg.h>

namespace Engine
{
    std::string FileDialogs::OpenFile(const char* filter)
    {
        char filePath[MAX_PATH] = {};

        OPENFILENAMEA openFileName = {};
        openFileName.lStructSize = sizeof(OPENFILENAMEA);
        openFileName.hwndOwner = nullptr;
        openFileName.lpstrFile = filePath;
        openFileName.nMaxFile = MAX_PATH;
        openFileName.lpstrFilter = filter;
        openFileName.nFilterIndex = 1;
        openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&openFileName) == TRUE)
            return openFileName.lpstrFile;

        return {};
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        char filePath[MAX_PATH] = {};

        OPENFILENAMEA openFileName = {};
        openFileName.lStructSize = sizeof(OPENFILENAMEA);
        openFileName.hwndOwner = nullptr;
        openFileName.lpstrFile = filePath;
        openFileName.nMaxFile = MAX_PATH;
        openFileName.lpstrFilter = filter;
        openFileName.nFilterIndex = 1;
        openFileName.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&openFileName) == TRUE)
            return openFileName.lpstrFile;

        return {};
    }
}