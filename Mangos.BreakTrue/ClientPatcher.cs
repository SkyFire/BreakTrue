using System;
using System.Globalization;
using System.IO;

namespace Mangos.BreakTrue
{
    public sealed class ClientPatcher
    {
        private static readonly byte?[] _connectionIndexPattern =
            {
                0xE8, null, null, null, null, 0x8B, 0xC8, 0x83, 0xC4, 0x04,
                0x83, 0xE1, 0x01, 0x80, 0xBC, 0x31, null, null, 0x00, 0x00,
            };

        private static readonly byte?[] _connectionIndexCheckPattern =
            {
                0x0F, 0x85, null, null, null, null, 0x8B, 0x4D, 0x10, 0x8B,
                0x55, 0x08, 0x53, 0x51, 0x8B, 0x8F, null, null, null, null,
            };

        private static readonly byte?[] _emailCheckPattern =
            {
                0x74, null, 0xBE, 0x01, 0x00, 0x00, 0x00, 0x8B, 0x0D, null,
                null, null, null, 0x85, 0xC9, 0x74, null, 0x8B, 0x01, 0x8B,
            };

        private readonly PatternScanner _scanner;

        private readonly string _fileName;

        public ClientPatcher(string fileName)
        {
            _fileName = fileName;
            var data = File.ReadAllBytes(fileName);
            _scanner = new PatternScanner(data);
        }

        private bool Patch(string patchName, byte?[] pattern, byte[] replacementBytes)
        {
            var offset = _scanner.Find(pattern);

            if (offset == null)
            {
                Console.WriteLine("{0}: Offset not found.", patchName);
                return false;
            }

            var ofs = (long)offset;

            Console.WriteLine("{0}: Offset found at: 0x{1}", patchName, ofs.ToString("X8", CultureInfo.InvariantCulture));

            try
            {
                var stream = File.Open(_fileName, FileMode.Open, FileAccess.Write, FileShare.None);
                using (var writer = new BinaryWriter(stream))
                {
                    stream.Position = ofs;
                    writer.Write(replacementBytes);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("{0}: Error: {1}", patchName, ex.Message);
                return false;
            }

            return true;
        }

        public bool Patch()
        {
            if (!Patch("Connection index selection", _connectionIndexPattern, new byte[] { 0xB8, 0x00, 0x00, 0x00, 0x00 }))
                return false;

            if (!Patch("Connection index check", _connectionIndexCheckPattern, new byte[] { 0x90, 0x90, 0x90, 0x90, 0x90 }))
                return false;

            if (!Patch("Grunt/Battle.net selection", _emailCheckPattern, new byte[] { 0xEB }))
                return false;

            return true;
        }
    }
}
