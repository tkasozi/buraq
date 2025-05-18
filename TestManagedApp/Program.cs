// In TestManagedApp/Program.cs
using System;
using System.Runtime.InteropServices; // Required for Marshal class
using ManagedLibrary; // Assuming your MyWorker class is in the ManagedLibrary namespace

// Define the structure we will marshal, matching what Execute expects
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct TestNativeArgs
{
    public IntPtr MessagePtr;
    public int Number;
}

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("--- Testing ManagedLibrary.MyWorker.Execute from C# ---");

        IntPtr unmanagedArgsPtr = IntPtr.Zero;
        IntPtr unmanagedMessagePtr = IntPtr.Zero;
        int sizeOfArgs = 0;

        try
        {
            // 1. Prepare the data
            string testMessage = "Get-ChildItem";
            int testNumber = 123;

            // 2. Marshal the string to unmanaged memory (ANSI, as expected by Execute)
            unmanagedMessagePtr = Marshal.StringToHGlobalAnsi(testMessage);

            // 3. Create an instance of our C# struct
            TestNativeArgs managedArgs = new TestNativeArgs
            {
                MessagePtr = unmanagedMessagePtr,
                Number = testNumber
            };

            // 4. Get the size of the struct for allocating unmanaged memory
            sizeOfArgs = Marshal.SizeOf(typeof(TestNativeArgs));

            // 5. Allocate unmanaged memory for the struct
            unmanagedArgsPtr = Marshal.AllocHGlobal(sizeOfArgs);

            // 6. Marshal the C# struct to the unmanaged memory block
            Marshal.StructureToPtr(managedArgs, unmanagedArgsPtr, false);

            Console.WriteLine($"Calling MyWorker.Execute with IntPtr {unmanagedArgsPtr} and size {sizeOfArgs}");

            // 7. Call the Execute method
            // The [UnmanagedCallersOnly] attribute does not prevent normal C# calls
            string result = CSharpWorker.Execute(unmanagedArgsPtr, sizeOfArgs);

            Console.WriteLine($"Result from CSharpWorker.Execute: {result}");
            // Assuming Execute returns number * 2 as in the previous example
            if (result.StartsWith("Output:"))
            {
                Console.WriteLine("Execute method test PASSED!");
            }
            else
            {
                Console.WriteLine($"Execute method failed.");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"An error occurred: {ex.ToString()}");
        }
        finally
        {
            // 8. IMPORTANT: Free the unmanaged memory
            if (unmanagedMessagePtr != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(unmanagedMessagePtr);
                Console.WriteLine("Freed unmanaged memory for message.");
            }
            if (unmanagedArgsPtr != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(unmanagedArgsPtr);
                Console.WriteLine("Freed unmanaged memory for arguments struct.");
            }
        }

        Console.WriteLine("\nTesting complete. Press any key to exit.");
        Console.ReadKey();
    }
}