CombMask - Combmask2 create filter for Avisynth2.6x/Avisynth+


description:
    CombMask2 is a simple filter that creates a comb mask that can (could) be
    used by other filters like MaskTools2.
    The mask consists of binaries of 0(not combed) and 255(combed).

    MaskedMerge2 is an exclusive masking filter for CombMask. This is often faster
    than MaskTools2's mt_merge().

    IsCombed2 is a is a utility function that can be used within AviSynth's
    conditionalfilter to test whether or not a frame is combed and returns true
    if it is and false if it isn't.

    These filters are written from scratch, but most of logics are come from
    tritical's TIVTC plugin.


syntax:
    CombMask2(clip, int "cthresh", int "mthresh", bool "chroma", bool "expand",
             int "metric", int opt)

        cthresh:
            spatial combing threshold.
            0 to 255, default is 6 (metric=0)
            0 to 65025, default is 10 (metric=1)

        mthresh:
            motion adaptive threshold.
            0 to 255, default is 9.

        chroma:
            Whether processing is performed to UV planes or not.
            default is true.

        expand:
            When set this to true, left and right pixels of combed pixel also
            assumed to combed.
            default is true.

        metric:
            Sets which spatial combing metric is used to detect combed pixels.
            Possible options:

              Assume 5 neighboring pixels (a,b,c,d,e) positioned vertically.

                    a
                    b
                    c
                    d
                    e

            0:  d1 = c - b;
                d2 = c - d;
                if ((d1 > cthresh && d2 > cthresh) || (d1 < -cthresh && d2 < -cthresh))
                {
                   if (abs(a+4*c+e-3*(b+d)) > cthresh*6) it's combed;
                }

            1:  val = (b - c) * (d - c);
                if (val > cthresh) it's combed;

            default is 0.

        opt:
            specify which CPU optimization are used.
            0 - Use C++ routine.
            1 - Use SSE2 routin if possible. When SSE2 can't be used, fallback to 0.
            others(default) - Use AVX2 routine if possible.
                              When AVX2 can't be used, fallback to 1.


    MaskedMerge2(clip base, clip alt, clip mask, int "MI", int "blockx", int "blocky",
                bool "chroma", int opt)

        base: base clip.

        alt: alternate clip which will be merged to base.

        mask: mask clip.

        MI(0 to blockx*blocky , default is 80):
            The # of combed pixels inside any of blockx * blocky size blocks on the Y-plane
            for the frame to be detected as combed.
            if the frame is not combed, merge process will be skipped.

        blockx:
            Sets the x-axis size of the window used during combed frame detection. This has
            to do with the size of the area in which MI number of pixels are required to be
            detected as combed for a frame to be declared combed.
            Possible values are 8, 16(default) or 32.

        blockx:
            Sets the y-axis size of the window used during combed frame detection. This has
            to do with the size of the area in which MI number of pixels are required to be
            detected as combed for a frame to be declared combed.
            Possible values are 8, 16(default) or 32.

        chroma:
            Whether processing is performed to UV planes or not.
            Default is true.

        opt:
            same as CombMask2.


    IsCombed2(clip, int "cthresh", int "mthresh",int "MI", int "blockx", int "blocky",
             int "metric", int "opt")

        cthresh: Same as CombMask2.

        mthresh: Same as CombMask2.

        MI: Same as MaskedMerge2.

        blockx: Same as MaskedMerge2.

        blockx: Same as MaskedMerge2.

        metric: Same as CombMask2.

        opt: same as CombMask2.


note:
    
    - On Avisynth+MT, CombMask2 and MaskedMerge2 are set as MT_NICE_FILTER automatically.
    
    - This plugin's filters require appropriate memory alignments.
      Thus, if you want to crop the left side of your source clip before these filters,
      you have to set crop(align=true).

usage:

    LoadPlugin("CombMask.dll)
    src = SourceFilter("foo\bar\fizz\buzz")
    deint = src.some_deinterlace_filter()
    deint2 = src.another_filter()
    mask = deint.CombMask2()
    last = deint.MaskedMerge2(deint2, mask)
    return last


    LoadPlugin("CombMask.dll")
    src = a_YV12_clip
    combed = src.ConvertToYV16(interlaced=true)
    nocomb = src.ConvertToYV16(interlaced=false)
    ConditionalFilter(src, combed, nocomb, "IsCombed2", "=", "true")


reqirement:

    - Avisynth2.60 or later / Avisynth+ r2005 or greater.
    - Windows Vista sp2 / 7 sp1 / 8.1 / 10.
    - Microsoft Visual C++ 2019 Redistributable Package
    - SSE2 capable CPU


author:
    Oka Motofumi (chikuzen.mo at gmail dot com)
