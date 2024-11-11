/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   rrv10_bin;
    const int            rrv10_binSize = 16384;

    extern const char*   knob_high_png;
    const int            knob_high_pngSize = 23552;

    extern const char*   knob_low_png;
    const int            knob_low_pngSize = 14862;

    extern const char*   ui_bg_png;
    const int            ui_bg_pngSize = 2228560;

    extern const char*   ui_bg_off_png;
    const int            ui_bg_off_pngSize = 2226007;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
