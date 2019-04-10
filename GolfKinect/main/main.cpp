#include "Game.h"
#include "Core/Core.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    int iRev = 0;
    if (SUCCEEDED(CoInitialize(NULL)))
    {
        if (!GET_SINGLE(Core)->Init(hInstance))
        {
            DESTROY_SINGLE(Core);
            return 0;
        }

        iRev = GET_SINGLE(Core)->Run();

        DESTROY_SINGLE(Core);

        CoUninitialize();
    }
    return iRev;
}