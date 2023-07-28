#pragma once
#include <Windows.h>
#include <vector>


/*
* OS reports DPI scaling values in relative terms, and not absolute terms.
* eg. if current DPI value is 250%, and recommended value is 200%, then
* OS will give us integer 2 for DPI scaling value (starting from recommended
* DPI scaling move 2 steps to the right in this list).
* values observed (and extrapolated) from system settings app (immersive control panel).
*/
static const UINT32 DpiVals[] = { 100,125,150,175,200,225,250,300,350, 400, 450, 500 };

class DpiHelper
{
public:
    template<class T, size_t sz>
    static size_t CountOf(const T (&arr)[sz])
    {
        UNREFERENCED_PARAMETER(arr);
        return sz;
    }

    /*
    * @brief : Use QueryDisplayConfig() to get paths, and modes.
    * @param[out] pathsV : reference to a vector which will contain paths
    * @param[out] modesV : reference to a vector which will contain modes
    * @param[in] flags : determines the kind of paths to retrieve (only active paths by default)
    * return : false in case of failure, else true
    */
    static bool GetPathsAndModes(std::vector<DISPLAYCONFIG_PATH_INFO>& pathsV, std::vector<DISPLAYCONFIG_MODE_INFO>& modesV, int flags = QDC_ONLY_ACTIVE_PATHS);

    //out own enum, similar to DISPLAYCONFIG_DEVICE_INFO_TYPE enum in wingdi.h
    enum class DISPLAYCONFIG_DEVICE_INFO_TYPE_CUSTOM : int
    {
        DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE = -3, //returns min, max, suggested, and currently applied DPI scaling values.
        DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE = -4, //set current dpi scaling value for a display
        DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_BRIGHTNESS_INFO = -7, //Get monitor brightness info
        DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_INTERNAL_INFO = DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_BRIGHTNESS_INFO, //alias for DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_BRIGHTNESS_INFO since it returns values other than brightness
        DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_UNIQUE_NAME = DISPLAYCONFIG_DEVICE_INFO_GET_MONITOR_BRIGHTNESS_INFO, //Another Alias since we are using the parameter mainly for getting the display unique name
    };

    /*
    * struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET
    * @brief used to fetch min, max, suggested, and currently applied DPI scaling values.
    * All values are relative to the recommended DPI scaling value
    * Note that DPI scaling is a property of the source, and not of target.
    */
    struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET
    {
        DISPLAYCONFIG_DEVICE_INFO_HEADER            header;
        /*
        * @brief min value of DPI scaling is always 100, minScaleRel gives no. of steps down from recommended scaling
        * eg. if minScaleRel is -3 => 100 is 3 steps down from recommended scaling => recommended scaling is 175%
        */
        std::int32_t minScaleRel;

        /*
        * @brief currently applied DPI scaling value wrt the recommended value. eg. if recommended value is 175%,
        * => if curScaleRel == 0 the current scaling is 175%, if curScaleRel == -1, then current scale is 150%
        */
        std::int32_t curScaleRel;

        /*
        * @brief maximum supported DPI scaling wrt recommended value
        */
        std::int32_t maxScaleRel;
    };

    /*
    * struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET
    * @brief set DPI scaling value of a source
    * Note that DPI scaling is a property of the source, and not of target.
    */
    struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET
    {
        DISPLAYCONFIG_DEVICE_INFO_HEADER            header;
        /*
        * @brief The value we want to set. The value should be relative to the recommended DPI scaling value of source.
        * eg. if scaleRel == 1, and recommended value is 175% => we are trying to set 200% scaling for the source
        */
        int32_t scaleRel;
    };

    /*
    * struct DPIScalingInfo
    * @brief DPI info about a source
    * mininum :     minumum DPI scaling in terms of percentage supported by source. Will always be 100%.
    * maximum :     maximum DPI scaling in terms of percentage supported by source. eg. 100%, 150%, etc.
    * current :     currently applied DPI scaling value
    * recommended : DPI scaling value reommended by OS. OS takes resolution, physical size, and expected viewing distance
    *               into account while calculating this, however exact formula is not known, hence must be retrieved from OS
    *               For a system in which user has not explicitly changed DPI, current should eqaul recommended.
    * bInitDone :   If true, it means that the members of the struct contain values, as fetched from OS, and not the default
    *               ones given while object creation.
    */
    struct DPIScalingInfo
    {
        UINT32 mininum = 100;
        UINT32 maximum = 100;
        UINT32 current = 100;
        UINT32 recommended = 100;
        bool bInitDone = false;
    };

    DpiHelper();
    ~DpiHelper();

    /*
    * When DPI scaling value is stored by OS in registry location Computer\HKEY_CURRENT_USER\Control Panel\Desktop\PerMonitorSettings
    * a unique string is generated for displays.
    * eg. Computer\HKEY_CURRENT_USER\Control Panel\Desktop\PerMonitorSettings\LEN41410_00_07E3_24^A8DD7E34BCF1555F032E26E990ABC597
    * For every display which has its DPI scaling value set to a non custom value an entry like this would be there.
    * If we want to set the registry value for a display manually when no such registry entry exists for it, then getting the unique
    * string isn't a straighforward process. The algorithm is analyzed partially here - https://stackoverflow.com/a/57397039/981766
    * but it doesn't cover all scenarios.
    * I recently stumbled across another undocumented Windows API which seems to give us this string for a display - DisplayConfigGetDeviceInfo(-7 or 0xfffffff9),
    * ie. type parameter set to -7.
    */
    static std::wstring GetDisplayUniqueName(LUID adapterID, UINT32 sourceID);
    static DpiHelper::DPIScalingInfo GetDPIScalingInfo(LUID adapterID, UINT32 sourceID);
    static bool SetDPIScaling(LUID adapterID, UINT32 sourceID, UINT32 dpiPercentToSet);

    /*Data structures for DisplayConfigGetDeviceInfo(-7)*/

    struct _DISPLAYCONFIG_BRIGHTNESS_CAPS_u_312_s_0 {
        unsigned int bLegacySupported : 1;
        unsigned int bNitsSupported : 1;
        unsigned int bCalibrated : 1;
        unsigned int bSmoothBrightnessSupported : 1;
        unsigned int bAdaptiveBrightnessSupported : 1;

    };

    struct _DISPLAYCONFIG_BRIGHTNESS_NIT_RANGE {
        unsigned int MinMillinits;
        unsigned int MaxMillinits;
        unsigned int StepSizeMillinits;
    };

    union _DISPLAYCONFIG_BRIGHTNESS_CAPS_u_312 {
        struct _DISPLAYCONFIG_BRIGHTNESS_CAPS_u_312_s_0 _s_0;
        unsigned int value;
    };

    struct _DISPLAYCONFIG_BRIGHTNESS_CAPS {
        unsigned char LegacyLevels[101];
        unsigned int LegacyLevelCount;
        struct _DISPLAYCONFIG_BRIGHTNESS_NIT_RANGE NitRanges[16];
        unsigned int NormalRangeCount;
        unsigned int TotalRangeCount;
        unsigned int PreferredMaximumBrightness;
        union _DISPLAYCONFIG_BRIGHTNESS_CAPS_u_312 field6_0x138;
    };

    /*From windows.graphics.dll*/
    typedef enum _DISPLAYCONFIG_HDR_CERTIFICATIONS {
        DISPLAYHDR_OVERRIDE_OFF = -2147483648,
        DISPLAYHDR_NONE = 0,
        DISPLAYHDR_GENERIC = 1,
        DISPLAYHDR_10_400 = 2,
        DISPLAYHDR_10_600 = 4,
        DISPLAYHDR_10_1000 = 8,
        DISPLAYHDR_10_1400 = 16,
        DISPLAYHDR_10_400_TRUEBLACK = 32,
        DISPLAYHDR_10_500_TRUEBLACK = 64,
        DISPLAYHDR_11_400 = 128,
        DISPLAYHDR_11_500 = 256,
        DISPLAYHDR_11_600 = 512,
        DISPLAYHDR_11_1000 = 1024,
        DISPLAYHDR_11_1400 = 2048,
        DISPLAYHDR_11_2000 = 4096,
        DISPLAYHDR_11_400_TRUEBLACK = 8192,
        DISPLAYHDR_11_500_TRUEBLACK = 16384,
        DISPLAYHDR_11_600_TRUEBLACK = 32768,
        DISPLAYHDR_11_1000_TRUEBLACK = 65536,
        DOLBYVISION_GENERIC = 131072,
        DOLBYVISION_LOWLATENCY = 262144,
        NVIDIA_HDR = 524288,
        NVIDIA_GSYNC_ULTIMATE = 1048576,
        AMD_FREESYNC_WITH_HDR = 2097152,
        AMD_FREESYNC_PREMIUM_PRO = 4194304,
        DISPLAYHDR_OEM_OVERRIDE_ON = 536870912,
        DISPLAYHDR_OVERRIDE_ON = 1073741824
    } _DISPLAYCONFIG_HDR_CERTIFICATIONS;


    struct _DISPLAYCONFIG_GET_MONITOR_INTERNAL_INFO {
        struct DISPLAYCONFIG_DEVICE_INFO_HEADER header;
        wchar_t monitorUniqueName[260];
        unsigned int RedPrimary[2];
        unsigned int GreenPrimary[2];
        unsigned int BluePrimary[2];
        unsigned int WhitePoint[2];
        unsigned long MinLuminance;
        unsigned long MaxLuminance;
        unsigned long MaxFullFrameLuminance;
        std::int32_t ColorspaceSupport;//4 bytes
        std::int32_t Flags;//4 bytes
        /*
        * FLAGS may internally have the following
        * LuminanceValuesRaw
        */
        struct _DISPLAYCONFIG_BRIGHTNESS_CAPS BrightnessCaps;
        unsigned int UsageSubClass;
        unsigned int DisplayTech;
        unsigned int NativeWidth;
        unsigned int NativeHeight;
        unsigned int PhysicalWidthInMm;
        unsigned int PhysicalHeightInMm;
        enum DISPLAYCONFIG_ROTATION DockedOrientation;
        enum _DISPLAYCONFIG_HDR_CERTIFICATIONS DisplayHdrCertifications;
    };

};

