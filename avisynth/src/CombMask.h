#ifndef COMB_MASK_H
#define COMB_MASK_H

#include <stdexcept>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#define NOGDI
#include <windows.h>
#include <avisynth.h>

#define CMASK_VERSION "1.1.1"
#define __AVX2__

typedef IScriptEnvironment ise_t;

enum class arch_t {
    NO_SIMD,
    USE_SSE2,
    USE_AVX2,
};


class Buffer {
    ise_t* env;
    bool isPlus;
    void* orig;
public:
    uint8_t* buffp;
    Buffer(size_t pitch, int height, int hsize, size_t align, bool ip, ise_t* e);
    ~Buffer();
};


class GVFmod : public GenericVideoFilter {
protected:
    bool isPlus;
    int numPlanes;
    size_t align;
    bool has_at_least_v8;

    GVFmod(PClip c, bool chroma, arch_t a, bool ip) :
        GenericVideoFilter(c), align(a == arch_t::USE_AVX2 ? 32 : 16), isPlus(ip) 
    {
        numPlanes = (vi.IsY8() || !chroma) ? 1 : 3;
    }
    int __stdcall SetCacheHints(int hints, int)
    {
        return hints == CACHE_GET_MTMODE ? MT_NICE_FILTER : 0;
    }
};


class CombMask : public GVFmod {
    int cthresh;
    int mthresh;
    bool expand;
    bool needBuff;
    size_t buffPitch;
    Buffer* buff;

    void (__stdcall *writeCombMask)(
        uint8_t* dstp, const uint8_t* srcp, const int dpitch, const int cpitch,
        const int cthresh, const int width, const int height);

    void (__stdcall *writeMotionMask)(
        uint8_t* tmpp, uint8_t* dstp, const uint8_t* srcp, const uint8_t* prevp,
        const int tpitch, const int dpitch, const int spitch, const int ppitch,
        const int mthresh, const int width, const int height);

    void (__stdcall *andMasks)(
        uint8_t* dstp, const uint8_t* altp, const int dpitch, const int apitch,
        const int width, const int height);

    void (__stdcall *expandMask)(
        uint8_t* dstp, uint8_t* srcp, const int dpitch, const int spitch,
        const int width, const int height);

public:
    CombMask(PClip c, int cth, int mth, bool chroma, arch_t arch, bool expand,
             int metric, bool is_avsplus, ise_t *env);
    ~CombMask();
    PVideoFrame __stdcall GetFrame(int n, ise_t* env);
};


typedef bool (__stdcall *check_combed_t)(
    PVideoFrame& cmask, int mi, int blockx, int blocky, bool is_avsplus,
    ise_t* env);


class MaskedMerge : public GVFmod {
    PClip altc;
    PClip maskc;
    int mi;
    int blockx;
    int blocky;

    check_combed_t checkCombed;

    void (__stdcall *mergeFrames)(
        int mum_planes, PVideoFrame& src, PVideoFrame& alt, PVideoFrame& mask,
        PVideoFrame& dst);

public:
    MaskedMerge(PClip c, PClip a, PClip m, int mi, int blockx, int blocky,
                bool chroma, arch_t arch, bool is_avsplus, ise_t *env);
    ~MaskedMerge() {}
    PVideoFrame __stdcall GetFrame(int n, ise_t* env);
};


check_combed_t get_check_combed(arch_t arch);


static inline void validate(bool cond, const char* msg)
{
    if (cond) throw std::runtime_error(msg);
}


static inline void*
alloc_buffer(size_t size, size_t align, bool is_avsplus, ise_t* env)
{
        bool has_at_least_v8 = true;
        try { env->CheckVersion(8); }
        catch (const AvisynthError&) { has_at_least_v8 = false; }
        if (has_at_least_v8) return env->Allocate(size, align, AVS_POOLED_ALLOC); return _aligned_malloc(size, align);
}


static inline void
free_buffer(void* buff, bool is_avsplus, ise_t* env)
{
        bool has_at_least_v8 = true;
        try { env->CheckVersion(8); }
        catch (const AvisynthError&) { has_at_least_v8 = false; }
        if (has_at_least_v8) { env->Free(buff); return; }
        _aligned_free(buff);
}

#endif

