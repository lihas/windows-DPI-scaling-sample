#include <iostream>
#include <Windows.h>

using namespace std;


static const UINT32 DpiVals[] = { 100,125,150,175,200,225,250,300,350, 400, 450, 500 };

/*Get default DPI scaling percentage.
The OS recommented value.
*/
int GetRecommendedDPIScaling()
{
    int dpi = 0;
    auto retval = SystemParametersInfo(SPI_GETLOGICALDPIOVERRIDE, 0, (LPVOID)&dpi, 1);

    if (retval != 0)
    {
        int currDPI = DpiVals[dpi * -1];
        return currDPI;
    }

    return -1;
}

void SetDpiScaling(int percentScaleToSet)
{
    int recommendedDpiScale = GetRecommendedDPIScaling();

    if (recommendedDpiScale > 0)
    {
        int index = 0, recIndex = 0, setIndex = 0 ;
        for (const auto& scale : DpiVals)
        {
            if (recommendedDpiScale == scale)
            {
                recIndex = index;
            }
            if (percentScaleToSet == scale)
            {
                setIndex = index;
            }
            index++;
        }
        
        int relativeIndex = setIndex - recIndex;
        SystemParametersInfo(SPI_SETLOGICALDPIOVERRIDE, relativeIndex, (LPVOID)0, 1);
    }
}

int main()
{
    for (;;)
    {
        int n = 0, dpiToSet = 0;
        cout << R"(
            1. Show Recommended DPI
            2. Set DPI
            Anything else to exit
)";
        cin >> n;
        switch (n)
        {
        case 1:
            cout << "recommened scaling: " << GetRecommendedDPIScaling() << "%" << endl;
            break;
        case 2:
            cout << "enter scaling to set in percentage" << endl;
            cin >> dpiToSet;
            SetDpiScaling(dpiToSet);
            break;
        default:
            exit(0);
            break;
        }
    }
    return 0;
}
