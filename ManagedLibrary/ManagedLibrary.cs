using System;
using System.Runtime.InteropServices;
using System.Management.Automation;
using System.Management.Automation.Runspaces;
using System.Collections.ObjectModel; // Required for Collection<PSObject>
using System.Text;

namespace ManagedLibrary
{
    public class CSharpWorker
    {

        // This structure to be passed from C++ to managed code
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct NativeArgs
        {
            public IntPtr MessagePtr;
            public int Number;
        }

        // Method to be called from C++
        [UnmanagedCallersOnly(EntryPoint = "Execute")]
        public static IntPtr Execute(IntPtr args, int argSizeBytes)
        {
            Console.WriteLine("C#: Hello from ManagedLibrary.CSharpWorker.Execute!");

            IntPtr unmanagedStringErrPtr = Marshal.StringToCoTaskMemUni("Null arguments pointer received");

            if (args == IntPtr.Zero)
            {
                Console.WriteLine("C#: Received null arguments pointer.");
                return unmanagedStringErrPtr;
            }

            // Marshal the arguments from unmanaged memory
            NativeArgs nativeArgs = Marshal.PtrToStructure<NativeArgs>(args);

            string message = Marshal.PtrToStringAnsi(nativeArgs.MessagePtr);
            // int number = nativeArgs.Number; // ignored

            using (PowerShell ps = PowerShell.Create())
            {
                ps.AddScript(message);

                IntPtr unmanagedStringPtr = Marshal.StringToCoTaskMemUni(CSharpWorker.ExecuteAndDisplayResults(ps, message));
                Console.WriteLine("Returning: " + unmanagedStringPtr);

                return unmanagedStringPtr;
            }

            return unmanagedStringErrPtr;
        }

        public static string ExecuteAndDisplayResults(PowerShell psInstance, string commandDescription)
        {
            StringBuilder sb = new StringBuilder($"Executing PS: '{commandDescription}'");
            Console.WriteLine($"Executing PS: '{commandDescription}'");
            try
            {
                // Invoke the command
                Collection<PSObject> results = psInstance.Invoke();

                // Process results
                if (results.Count > 0)
                {
                    Console.WriteLine("Output:");
                    sb.Append("Output:");
                    foreach (PSObject result in results)
                    {
                        // The result object can be complex. For simple output, .ToString() might be enough.
                        // Or, you can access its properties: result.Properties["PropertyName"].Value
                        if (result != null)
                        {
                            Console.WriteLine($"- {result.ToString()}");
                            sb.Append($"- {result.ToString()}");
                            // Example of accessing a specific property if you know it exists
                            // if (result.Properties["ProcessName"] != null) {
                            //     Console.WriteLine($"  Process Name: {result.Properties["ProcessName"].Value}");
                            // }
                        }
                    }
                }

                // Check for errors in the error stream
                if (psInstance.Streams.Error.Count > 0)
                {
                    Console.WriteLine("Errors:");
                    foreach (ErrorRecord error in psInstance.Streams.Error)
                    {
                        Console.WriteLine($"- Error: {error.ToString()}");
                        Console.WriteLine($"  Exception: {error.Exception.Message}");
                        // You can get more details from error.InvocationInfo, error.CategoryInfo, etc.
                        sb.Append($"- Error: {error.ToString()}");
                        sb.Append($"  Exception: {error.Exception.Message}");
                    }
                }

                return sb.ToString();
            }
            catch (Exception ex)
            {
                // This catches exceptions from the Invoke() call itself,
                // not necessarily script errors which go to psInstance.Streams.Error
                Console.WriteLine($"An exception occurred during PowerShell invocation: {ex.Message}");
                return sb.Append($"An exception occurred during PowerShell invocation: {ex.Message}").ToString();
            }
        }

        // A simpler example without arguments for initial testing
        [UnmanagedCallersOnly(EntryPoint = "SayHello")]
        public static void SayHello()
        {
            Console.WriteLine("C#: Hello from ManagedLibrary.CSharpWorker.SayHello!");
        }
    }
}