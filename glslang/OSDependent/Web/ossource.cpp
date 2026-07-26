//
// OSDependent/Web/ossource.cpp — implémentation glslang pour Emscripten (wasm).
//
// glslang/OGLCompilersDLL/InitializeDll.cpp et ShaderLang.cpp référencent les
// fonctions OS_* (TLS) et {Init,Get,Release}GlobalLock déclarées dans
// osinclude.h ; chaque plateforme fournit sa propre implémentation
// (Windows/ossource.cpp = TLS API Win32, Unix/ossource.cpp = pthread).
//
// Sur le Web, notre build ne compile PAS avec `-pthread` (pas de
// SharedArrayBuffer / en-têtes COOP-COEP configurés) : le module wasm tourne
// sur un seul thread JS. Le "TLS" et les verrous n'ont donc rien à protéger —
// un tableau de slots simple (indexé par OS_TLSIndex) suffit, sans mutex.
//
#include "../osinclude.h"

#include <cstdint>
#include <vector>

namespace glslang {

namespace {
	std::vector<void *> gTlsSlots;
}

OS_TLSIndex OS_AllocTLSIndex() {
	gTlsSlots.push_back(nullptr);
	// Index 1-based : 0/nullptr reste OS_INVALID_TLS_INDEX.
	return reinterpret_cast<OS_TLSIndex>(static_cast<uintptr_t>(gTlsSlots.size()));
}

bool OS_SetTLSValue(OS_TLSIndex nIndex, void *lpvValue) {
	const uintptr_t i = reinterpret_cast<uintptr_t>(nIndex);
	if (i == 0 || i > gTlsSlots.size())
		return false;
	gTlsSlots[i - 1] = lpvValue;
	return true;
}

void *OS_GetTLSValue(OS_TLSIndex nIndex) {
	const uintptr_t i = reinterpret_cast<uintptr_t>(nIndex);
	if (i == 0 || i > gTlsSlots.size())
		return nullptr;
	return gTlsSlots[i - 1];
}

bool OS_FreeTLSIndex(OS_TLSIndex nIndex) {
	const uintptr_t i = reinterpret_cast<uintptr_t>(nIndex);
	if (i == 0 || i > gTlsSlots.size())
		return false;
	gTlsSlots[i - 1] = nullptr; // pas de "trou" : slot simplement libéré
	return true;
}

// Mono-thread (pas de -pthread) : aucun verrou réel n'est nécessaire.
void InitGlobalLock() {
}

void GetGlobalLock() {
}

void ReleaseGlobalLock() {
}

void OS_DumpMemoryCounters() {
}

} // namespace glslang
