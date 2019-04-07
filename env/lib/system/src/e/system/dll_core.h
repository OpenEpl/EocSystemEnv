#pragma once
extern "C"
{
	__declspec(dllimport) intptr_t __stdcall LoadLibraryA(const char* lpLibFileName);
	__declspec(dllimport) void* __stdcall GetProcAddress(intptr_t hModule,const char* lpProcName);

}
#define eoc_DefineMoudleLoader(name, dllname) \
static decltype(LoadLibraryA("")) InternalMoudleHandle_##name;\
static decltype(LoadLibraryA("")) GetMoudleHandle_##name() {\
	auto x = InternalMoudleHandle_##name;\
	if(!x) InternalMoudleHandle_##name = x = LoadLibraryA(dllname);\
	return x;\
}

#define eoc_DefineFuncPtrGetter(name, hmod, rname) \
static void* InternalFuncPtr_##name;\
static void* GetFuncPtr_##name() {\
	void* x = InternalFuncPtr_##name;\
	if (!x) InternalFuncPtr_##name = x = GetProcAddress(hmod, rname);\
	return x;\
}