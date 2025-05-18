using System;
using System.Runtime.InteropServices;

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
        public static int Execute(IntPtr args, int argSizeBytes)
        {
            Console.WriteLine("C#: Hello from ManagedLibrary.CSharpWorker.Execute!");

            if (args == IntPtr.Zero)
            {
                Console.WriteLine("C#: Received null arguments pointer.");
                return -1;
            }

            // Marshal the arguments from unmanaged memory
            NativeArgs nativeArgs = Marshal.PtrToStructure<NativeArgs>(args);

            string message = Marshal.PtrToStringAnsi(nativeArgs.MessagePtr);

            Console.WriteLine($"C#: Received message: {message}");
            Console.WriteLine($"C#: Received number: {nativeArgs.Number}");

            return 0;
        }

        // A simpler example without arguments for initial testing
        [UnmanagedCallersOnly(EntryPoint = "SayHello")]
        public static void SayHello()
        {
            Console.WriteLine("C#: Hello from ManagedLibrary.CSharpWorker.SayHello!");
        }
    }
}