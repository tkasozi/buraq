using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using Buraq.PS; // Your namespace containing PowerShellManager

namespace Buraq.Bridge
{
    class Program
    {
        static async Task Main(string[] args)
        {
            var listener = new TcpListener(IPAddress.Loopback, 12345);
            listener.Start();
            Console.WriteLine("PowerShell Host Server is listening on port 12345...");

            // Create a single instance of your PowerShell manager.
            var psManager = new PowerShellManager();

            while (true)
            {
                using (TcpClient client = await listener.AcceptTcpClientAsync())
                {
                    await using (NetworkStream stream = client.GetStream())
                    using (var reader = new StreamReader(stream))
                    using (var writer = new StreamWriter(stream))
                    {
                        string? script = await reader.ReadLineAsync();
                        if (script != null)
                        {
                            // Use the PowerShellManager to run the script.
                            string result = psManager.RunScript(script);

                            Console.WriteLine("TcpClient: ", result);

                            // Send the result back to the C++ client.
                            await writer.WriteLineAsync(result);
                            await writer.FlushAsync();
                        }
                    }
                }
            }
        }
    }
}
