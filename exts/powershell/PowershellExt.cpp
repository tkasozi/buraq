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
#include "../../app/IToolsAPI.h"
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
// Windows-specific code here
#include <windows.h>
#include <filesystem> // For path operations, C++17

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
	int number;
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
		std::wcerr << L"get_hostfxr_path failed: " << std::hex << rc << std::endl;
		return false;
	}
	std::wcout << L"hostfxr path: " << buffer << std::endl;

	// Load hostfxr and get exported functions
	HMODULE lib = LoadLibraryW(buffer); // Use LoadLibraryW for wchar_t
	if (lib == nullptr) {
		std::wcerr << L"Failed to load hostfxr library. Error code: " << GetLastError() << std::endl;
		return false;
	}

	init_fptr = (hostfxr_initialize_for_runtime_config_fn) GetProcAddress(lib, "hostfxr_initialize_for_runtime_config");
	get_delegate_fptr = (hostfxr_get_runtime_delegate_fn) GetProcAddress(lib, "hostfxr_get_runtime_delegate");
	close_fptr = (hostfxr_close_fn) GetProcAddress(lib, "hostfxr_close");

	if (init_fptr == nullptr || get_delegate_fptr == nullptr || close_fptr == nullptr) {
		std::cerr << "Failed to get one or more hostfxr exports." << std::endl;
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
		std::wcerr << L"hostfxr_initialize_for_runtime_config failed: " << std::hex << rc << std::endl;
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
		std::cerr << "hostfxr_get_runtime_delegate failed: " << std::hex << rc << std::endl;
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
	void *app_api_;
	execute_method_fn execute_fptr;
	fs::path assembly_path;

public:
	PowershellPlugin() : app_api_(nullptr), execute_fptr(nullptr) {
		std::cout << "PowershellPlugin" << std::endl;

		// --- 1. Load hostfxr and get exported hosting functions ---
		if (!load_hostfxr()) {
			std::cerr << "Failed to load hostfxr." << std::endl;
			exit(-1); // Or handle error appropriately
		}
		std::wcout << L"hostfxr loaded successfully." << std::endl;

		// --- 2. Prepare paths and arguments ---
		// Path to the managed assembly's .runtimeconfig.json
		// IMPORTANT: Adjust this path to where your ManagedLibrary.dll and ManagedLibrary.runtimeconfig.json are located
		// You might want to make this configurable or relative to your executable
		fs::path managed_assembly_dir =
				fs::current_path().parent_path() / "bin" / "plugins" / "ext" /
				"ManagedLibraryPublish"; // Example path
		fs::path config_path = managed_assembly_dir / "ManagedLibrary.runtimeconfig.json";
		assembly_path = managed_assembly_dir / "ManagedLibrary.dll";

		std::wcout << L"Using runtime config: " << config_path.wstring() << std::endl;
		std::wcout << L"Using assembly: " << assembly_path.wstring() << std::endl;

		if (!fs::exists(config_path)) {
			std::wcerr << L"Runtime config file not found: " << config_path.wstring() << std::endl;
			exit(-1); // Or handle error appropriately
		}
		if (!fs::exists(assembly_path)) {
			std::wcerr << L"Managed assembly file not found: " << assembly_path.wstring() << std::endl;
			exit(-1); // Or handle error appropriately
		}

		// --- 3. Initialize and start the .NET runtime ---
		load_assembly_and_get_function_pointer_fn load_assembly_and_get_fp = nullptr;
		load_assembly_and_get_fp = get_dotnet_load_assembly(config_path.c_str());
		if (load_assembly_and_get_fp == nullptr) {
			std::cerr << "Failed to get .NET load assembly function." << std::endl;
			exit(-1); // Or handle error appropriately
		}
		std::wcout << L".NET runtime initialized and load_assembly_and_get_function_pointer delegate obtained."
				   << std::endl;

		int rc = load_assembly_and_get_fp(
				assembly_path.c_str(),
				STR("ManagedLibrary.CSharpWorker, ManagedLibrary"), // Type name (namespace.class, assembly_name)
				STR("Execute"),                                // Method name (must match EntryPoint in UnmanagedCallersOnly)
				UNMANAGEDCALLERSONLY_METHOD,                   // Delegate type name (or UNMANAGEDCALLERSONLY_METHOD for UnmanagedCallersOnly)
				nullptr,                                       // Reserved
				(void **) &execute_fptr                          // Out parameter for the function pointer
		);

		if (rc != 0 || execute_fptr == nullptr) {
			std::cerr << "Failed to get delegate for Execute. RC: " << rc << std::endl;
			exit(-1); // Or handle error appropriately
		}
		std::wcout << L"Delegate for Execute obtained." << std::endl;
	}

	const char *getName() const override {
		return "Powershell Plugin";
	}

	bool initialize(void *app_context) override {
		std::cout << "[PowershellPlugin] Initializing, 12..." << std::endl;
		app_api_ = app_context;
		return true;
	}

	void shutdown() override {
		std::cout << "[PowershellPlugin] Shutting down..." << std::endl;
		// Release any resources
		app_api_ = nullptr;
	}

	ProcessedData performAction(void *cmd) override {
		ProcessedData processedData;

		std::wcout << L"performAction with arguments: " << (const char *)cmd << std::endl;
		// const char *message_to_send = "Get-Process";
		CSharpWorkerArgs native_args;
		native_args.message = (const char *)cmd;
		native_args.number = 42;

		// Call the C# method
		const wchar_t *wide_result_from_cs = nullptr;
		try {
			wide_result_from_cs = execute_fptr(&native_args, sizeof(native_args));

			if (wide_result_from_cs != nullptr) {
				std::wcout << L"Called Execute. C# method returned: \n" << wide_result_from_cs << L"\""
						   << std::endl;

				processedData.resultValue = std::wstring(wide_result_from_cs);

				if (processedData.resultValue.starts_with(reinterpret_cast<const wchar_t *>("Error"))) {
					processedData.isProcessFailed = true;
				}

				// IMPORTANT: Free the memory allocated by the .NET marshaller
				CoTaskMemFree((LPVOID) wide_result_from_cs); // Cast to LPVOID for CoTaskMemFree
				wide_result_from_cs = nullptr; // Good practice to nullify after free
				std::wcout << L"Memory for returned string freed." << std::endl;
				return processedData;
			} else {
				std::wcout << L"Called Execute. C# method returned nullptr." << std::endl;
			}
		} catch (const std::exception &e) {
			std::wcerr << L"Exception during C# Execute call: " << e.what() << std::endl;
			if (wide_result_from_cs != nullptr) { // If exception occurred after allocation but before free
				CoTaskMemFree((LPVOID) wide_result_from_cs);
			}
		} catch (...) {
			std::wcerr << L"Unknown exception during C# Execute call." << std::endl;
			if (wide_result_from_cs != nullptr) {
				CoTaskMemFree((LPVOID) wide_result_from_cs);
			}
		}
		return {};
	}

	void performAction() override {
		std::cout << std::endl;
		if (app_api_) {
			// std::cout << "[" << getName() << "] Accessing application context (e.g., an API)." << std::endl;
			// ((IToolsApi *) app_api_)->someApplicationFunction();

			// --- 5. Prepare arguments and call the managed method ---
			const char *message_to_send = "Get-Process | Sort-Object CPU -Descending | Select-Object -First 5";
			// const char *message_to_send = "Get-Process";
			CSharpWorkerArgs native_args;
			native_args.message = message_to_send;
			native_args.number = 42;

			// Call the C# method
			const wchar_t *wide_result_from_cs = nullptr;
			try {
				wide_result_from_cs = execute_fptr(&native_args, sizeof(native_args));

				if (wide_result_from_cs != nullptr) {
					std::wcout << L"Called Execute. C# method returned: \n" << wide_result_from_cs << L"\""
							   << std::endl;

					// You can convert it to std::wstring if needed for further use
					std::wstring result_str(wide_result_from_cs);
					// Use result_str

					// IMPORTANT: Free the memory allocated by the .NET marshaller
					CoTaskMemFree((LPVOID) wide_result_from_cs); // Cast to LPVOID for CoTaskMemFree
					wide_result_from_cs = nullptr; // Good practice to nullify after free
					std::wcout << L"Memory for returned string freed." << std::endl;
				} else {
					std::wcout << L"Called Execute. C# method returned nullptr." << std::endl;
				}
			} catch (const std::exception &e) {
				std::wcerr << L"Exception during C# Execute call: " << e.what() << std::endl;
				if (wide_result_from_cs != nullptr) { // If exception occurred after allocation but before free
					CoTaskMemFree((LPVOID) wide_result_from_cs);
				}
			} catch (...) {
				std::wcerr << L"Unknown exception during C# Execute call." << std::endl;
				if (wide_result_from_cs != nullptr) {
					CoTaskMemFree((LPVOID) wide_result_from_cs);
				}
			}

			// Note: The hostfxr context is typically closed when the host exits or
			// when hostfxr_close is called on the handle from hostfxr_initialize_for_runtime_config.
			// The load_assembly_and_get_function_pointer_fn delegate itself doesn't manage this lifecycle.
			// Proper cleanup would involve storing the hostfxr_handle from init_fptr and calling close_fptr on it.
			// For this example, we rely on process termination for cleanup of the runtime.
			// See Microsoft docs for more robust cleanup patterns if your host needs to repeatedly init/close.
		}
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
PLUGIN_API IPlugin *create_plugin() {
	return new PowershellPlugin();
}
PLUGIN_API void destroy_plugin(IPlugin *plugin) {
	delete plugin;
}
}
