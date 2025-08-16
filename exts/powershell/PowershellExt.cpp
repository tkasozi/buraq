// MIT License
//
// Copyright (c)  "2025" Talik A. Kasozi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by talik on 5/15/2025.
//

#include "PluginInterface.h"
#include "buraq.h"
#include <string>
#include <vector>

#ifdef _WIN32
// Windows-specific code here
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

#define STR(s) L##s // For wide strings on Windows
using char_t = wchar_t;
#else
// Equivalent code for other platforms here
#include <filesystem> // For path operations, C++17
namespace fs = std::filesystem;
#define STR(s) s
using char_t = char;
#endif

// .NET Hosting headers
// Make sure these paths are correctly set in your project's include directories
#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

// Forward declarations
bool load_hostfxr();

load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path);

// Define the structure to match the C# one for arguments
struct CSharpWorkerArgs {
	const char *message; // C-style string for the message
};

// Function pointer types for hostfxr
hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
hostfxr_close_fn close_fptr = nullptr;

// Type of function with no arguments and no return value
typedef void (CORECLR_DELEGATE_CALLTYPE *void_fptr_t)();

// --- Helper Functions (based on Microsoft samples) ---

bool load_hostfxr() {
	// Pre-allocate a large buffer for the path to hostfxr
	char_t buffer[MAX_PATH];
	size_t buffer_size = sizeof(buffer) / sizeof(char_t);
	int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
	if (rc != 0) {
		return false;
	}

	// Load hostfxr and get exported functions
	HMODULE lib = LoadLibraryW(buffer); // Use LoadLibraryW for wchar_t
	if (lib == nullptr) {
		return false;
	}

	init_fptr = (hostfxr_initialize_for_runtime_config_fn) GetProcAddress(lib, "hostfxr_initialize_for_runtime_config");
	get_delegate_fptr = (hostfxr_get_runtime_delegate_fn) GetProcAddress(lib, "hostfxr_get_runtime_delegate");
	close_fptr = (hostfxr_close_fn) GetProcAddress(lib, "hostfxr_close");

	if (init_fptr == nullptr || get_delegate_fptr == nullptr || close_fptr == nullptr) {
		FreeLibrary(lib); // Clean up
		return false;
	}
	return true;
}

load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path) {
	void *load_assembly_and_get_function_pointer = nullptr;
	hostfxr_handle cxt = nullptr;

	// Initialize hostfxr context
	// hostfxr_initialize_parameters can be nullptr for default behavior
	int rc = init_fptr(config_path, nullptr, &cxt);
	if (rc != 0 || cxt == nullptr) {
		if (cxt != nullptr) {
			close_fptr(cxt); // Close the context on failure
		}
		return nullptr;
	}

	// Get the delegate for loading assemblies and getting function pointers
	rc = get_delegate_fptr(
			cxt,
			hdt_load_assembly_and_get_function_pointer, // Type of delegate to get
			&load_assembly_and_get_function_pointer);

	if (rc != 0 || load_assembly_and_get_function_pointer == nullptr) {
//		std::cerr << "hostfxr_get_runtime_delegate failed: " << std::hex << rc << std::endl;
	}

	// It's important to close the hostfxr context.
	// The returned delegate (load_assembly_and_get_function_pointer) can be used even after the context is closed,
	// as per Microsoft's documentation/samples for this specific delegate type.
	// "The `load_assembly_and_get_function_pointer` delegate is valid until `hostfxr_close` is called on the context from which it was obtained,
	//  OR until the hostfxr library is unloaded."
	// However, samples often show closing it immediately if the delegate is the only thing needed from this particular context.
	// If you need other delegates or to manage the runtime further with 'cxt', keep it open.
	close_fptr(cxt);


	return (load_assembly_and_get_function_pointer_fn) load_assembly_and_get_function_pointer;
}

// For Execute with arguments
// Define the delegate type for the C# method
// Matches: public static int Execute(IntPtr arg, int argSizeBytes)
typedef wchar_t *(CORECLR_DELEGATE_CALLTYPE *execute_method_fn)(void *args, int sizeBytes);

class PowershellPlugin : public IPlugin {
private:
	buraq::buraq_api *app_api_;
	execute_method_fn execute_fptr;
	fs::path assembly_path;

public:
	explicit PowershellPlugin(buraq::buraq_api *searchPath) : IPlugin(searchPath), app_api_(searchPath), execute_fptr(nullptr) {
		// --- 1. Load hostfxr and get exported hosting functions ---
		if (!load_hostfxr()) {
			exit(-1); // Or handle error appropriately
		}
		// hostfxr loaded successfully.

		// --- 2. Prepare paths and arguments ---
		// Should be relative to your executable
		const fs::path managed_assembly_dir = app_api_->searchPath / "managed";
		const fs::path config_path = managed_assembly_dir / "ManagedLibrary.runtimeconfig.json";
		assembly_path = managed_assembly_dir / "ManagedLibrary.dll";

		if (!fs::exists(config_path)) {
			exit(-1); // Or handle error appropriately
		}
		if (!fs::exists(assembly_path)) {
			// Managed assembly file not found:
			exit(-1); // Or handle error appropriately
		}

		// --- 3. Initialize and start the .NET runtime ---
		load_assembly_and_get_function_pointer_fn load_assembly_and_get_fp = nullptr;
		load_assembly_and_get_fp = get_dotnet_load_assembly(config_path.c_str());
		if (load_assembly_and_get_fp == nullptr) {
			exit(-1); // Or handle error appropriately
		}

		//.NET runtime initialized and load_assembly_and_get_function_pointer delegate obtained.
		int rc = load_assembly_and_get_fp(
				assembly_path.c_str(),
				STR("ManagedLibrary.CSharpWorker, ManagedLibrary"), // Type name (namespace.class, assembly_name)
				STR("Execute"),                                // Method name (must match EntryPoint in UnmanagedCallersOnly)
				UNMANAGEDCALLERSONLY_METHOD,                   // Delegate type name (or UNMANAGEDCALLERSONLY_METHOD for UnmanagedCallersOnly)
				nullptr,                                       // Reserved
				(void **) &execute_fptr                          // Out parameter for the function pointer
		);

		if (rc != 0 || execute_fptr == nullptr) {
			// Failed to get delegate for Execute. RC
			exit(-1); // Or handle error appropriately
		}
		// Delegate for Execute obtained.
	}

	const char *getName() const override {
		return "Powershell Plugin";
	}

	bool initialize(buraq::buraq_api *app_context) override {
		app_api_ = app_context;
		return true;
	}

	void shutdown() override {
		// Release any resources
		app_api_ = nullptr;
	}

	ProcessedData performAction(void *cmd) override {
		ProcessedData processedData;

		CSharpWorkerArgs native_args{};
		native_args.message = (const char *)cmd;

		// Call the C# method
		const wchar_t *wide_result_from_cs = nullptr;
		try {
			wide_result_from_cs = execute_fptr(&native_args, sizeof(native_args));

			if (wide_result_from_cs != nullptr) {

				processedData.resultValue = std::wstring(wide_result_from_cs);

				// IMPORTANT: Free the memory allocated by the .NET marshaller
				CoTaskMemFree((LPVOID) wide_result_from_cs); // Cast to LPVOID for CoTaskMemFree
				wide_result_from_cs = nullptr; // Good practice to nullify after free
				// Memory for returned string freed
				return processedData;
			} else {
				// Called Execute. C# method returned nullptr
				return {};
			}
		} catch (const std::exception &e) {
			if (wide_result_from_cs != nullptr) { // If exception occurred after allocation but before free
				CoTaskMemFree((LPVOID) wide_result_from_cs);
			}
		} catch (...) {
			if (wide_result_from_cs != nullptr) {
				CoTaskMemFree((LPVOID) wide_result_from_cs);
			}
		}
		return {};
	}
};

// Define for consistent export/import macros
#if defined(_WIN32) || defined(_WIN64)
#ifdef POWERSHELL_PLUGIN_EXPORTS // This macro should be defined when building the DLL
#define PLUGIN_API extern "C" __declspec(dllexport)
#else
#define PLUGIN_API extern "C" __declspec(dllimport) // For code consuming the DLL
#endif
#else // For GCC/Clang on other platforms (Linux, macOS)
#define PLUGIN_API extern "C" __attribute__((visibility("default")))
#endif
extern "C"
{
// Functions will be looked up by name by the plugin manager
PLUGIN_API IPlugin *create_plugin(buraq::buraq_api *app_context) {
	return new PowershellPlugin(app_context);
}
PLUGIN_API void destroy_plugin(IPlugin *plugin) {
	delete plugin;
}
}
