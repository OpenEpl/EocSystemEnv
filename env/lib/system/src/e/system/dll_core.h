#pragma once
extern "C"
{
	__declspec(dllimport) intptr_t __stdcall LoadLibraryW(const wchar_t* lpLibFileName);
	__declspec(dllimport) void* __stdcall GetProcAddress(intptr_t hModule,const char* lpProcName);

}
#define eoc_DefineMoudleLoader(name, dllname) \
static decltype(::LoadLibraryW(L"")) InternalMoudleHandle_##name;\
static decltype(::LoadLibraryW(L"")) GetMoudleHandle_##name() {\
	auto x = InternalMoudleHandle_##name;\
	if(!x) InternalMoudleHandle_##name = x = ::LoadLibraryW(L ## dllname);\
	return x;\
}

#define eoc_DefineFuncPtrGetter(name, hmod, rname) \
static void* InternalFuncPtr_##name;\
static void* GetFuncPtr_##name() {\
	void* x = InternalFuncPtr_##name;\
	if (!x) InternalFuncPtr_##name = x = ::GetProcAddress(hmod, rname);\
	return x;\
}