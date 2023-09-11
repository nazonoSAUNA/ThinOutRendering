#include <windows.h>
#include <filter.h>

FILTER_DLL filter = {
    FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_NO_CONFIG,
    NULL,NULL,
    const_cast<char*>("ŠÔˆø‚¢‚Ä•`‰æ"),
    NULL,NULL,NULL,
    NULL,NULL,
    NULL,NULL,NULL,
    NULL,
    func_init,
};
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void) {
    return &filter;
}

static inline void(__cdecl* exedit_rendering)(void* dst, double* polydata, int polynum, void* src, int* sizeinfo, int alpha, int flag); // 79350
static int* exedit_thin_out_process_ptr;

static int exedit_base;
static HWND exedit_hwnd;

FILTER* get_exeditfp(FILTER* fp) {
    SYS_INFO si;
    fp->exfunc->get_sys_info(NULL, &si);

    for (int i = 0; i < si.filter_n; i++) {
        FILTER* tfp = (FILTER*)fp->exfunc->get_filterp(i);
        if (tfp->information != NULL) {
            if (!strcmp(tfp->information, "Šg’£•ÒW(exedit) version 0.92 by ‚j‚d‚m‚­‚ñ")) return tfp;
        }
    }
    return NULL;
}


BOOL exedit_ReplaceCall(DWORD exedit_address, void* new_address) {
    DWORD oldProtect;
    DWORD* address = (DWORD*)(exedit_address + exedit_base);
    if (!VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return FALSE;
    }
    *address = (DWORD)new_address - (DWORD)address - 4;
    return VirtualProtect(address, 4, oldProtect, &oldProtect);
}


void __cdecl exedit_rendering_wrap(void* dst, double* polydata, int polynum, void* src, int* sizeinfo, int alpha, int flag) {
    int exedit_thin_out_process = *exedit_thin_out_process_ptr;
    *exedit_thin_out_process_ptr = 1;
    exedit_rendering(dst, polydata, polynum, src, sizeinfo, alpha, flag);
    *exedit_thin_out_process_ptr = exedit_thin_out_process;
}

BOOL func_init(FILTER* fp) {
    FILTER* exeditfp = get_exeditfp(fp);
    if (exeditfp == NULL) {
        MessageBoxA(fp->hwnd, "Šg’£•ÒW0.92‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ‚Å‚µ‚½", fp->name, MB_OK);
        return TRUE;
    }
    exedit_base = (int)exeditfp->dll_hinst;
    exedit_hwnd = exeditfp->hwnd;

    exedit_rendering = reinterpret_cast<decltype(exedit_rendering)>(exedit_base + 0x79350);
    exedit_thin_out_process_ptr = (int*)(exedit_base + 0x2308a0);

    exedit_ReplaceCall(0x4c600, &exedit_rendering_wrap);
    exedit_ReplaceCall(0x4c7ba, &exedit_rendering_wrap);

    return TRUE;
}
