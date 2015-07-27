#if !defined(HAVE_CORECLR) && !defined(HAVE_NATIVECLR)
#error "CoreCLR and/or a native .NET runtime (native .NET on Windows or Mono on Linux) must be installed in order for Edge.js to compile."
#endif

#include "edge_common.h"

#ifdef HAVE_CORECLR
#include "../CoreCLREmbedding/edge.h"
#endif
#ifdef HAVE_NATIVECLR
#ifdef EDGE_PLATFORM_WINDOWS
#include "../dotnet/edge.h"
#else
#include "../mono/edge.h"
#endif
#endif

BOOL debugMode;
BOOL enableScriptIgnoreAttribute;

NAN_METHOD(initializeClrFunc)
{
#ifdef HAVE_NATIVECLR
#ifdef HAVE_CORECLR
	if (HasEnvironmentVariable("EDGE_USE_CORECLR"))
	{
		return CoreClrFunc::Initialize(args);
	}

#endif
	return ClrFunc::Initialize(args);
#else
	return CoreClrFunc::Initialize(args);
#endif
}

#pragma unmanaged
void init(Handle<Object> target)
{
    debugMode = HasEnvironmentVariable("EDGE_DEBUG");
    DBG("edge::init");
    V8SynchronizationContext::Initialize();
#ifdef HAVE_NATIVECLR
#ifdef HAVE_CORECLR
    if (!HasEnvironmentVariable("EDGE_USE_CORECLR"))
    {
#ifndef EDGE_PLATFORM_WINDOWS
    	MonoEmbedding::Initialize();
#endif
    }

    else if (FAILED(CoreClrEmbedding::Initialize(debugMode)))
    {
    	DBG("Error occurred during CoreCLR initialization");
    	return;
    }
#endif
#else
    if (FAILED(CoreClrEmbedding::Initialize(debugMode)))
	{
		DBG("Error occurred during CoreCLR initialization");
		return;
	}
#endif

    enableScriptIgnoreAttribute = HasEnvironmentVariable("EDGE_ENABLE_SCRIPTIGNOREATTRIBUTE");
    NODE_SET_METHOD(target, "initializeClrFunc", initializeClrFunc);
}

#pragma unmanaged
bool HasEnvironmentVariable(const char* variableName)
{
#ifdef EDGE_PLATFORM_WINDOWS
    return 0 < GetEnvironmentVariable(variableName, NULL, 0);
#else
    return getenv(variableName) != NULL;
#endif
}

#pragma unmanaged
NODE_MODULE(edge, init);

// vim: ts=4 sw=4 et:
