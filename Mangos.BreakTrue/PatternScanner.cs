using System.Collections.Generic;
using System.Linq;

namespace Mangos.BreakTrue
{
    public sealed class PatternScanner
    {
        private readonly byte[] _data;

        public PatternScanner(byte[] data)
        {
            _data = data;
        }

        public long? Find(byte?[] pattern)
        {
            for (var i = 0; i < _data.Length; i++)
                if (CompareSequences(pattern, i))
                    return i;

            return null;
        }

        private bool CompareSequences(IEnumerable<byte?> seq, int offset)
        {
            return !seq.Where((b, index) => b != null && _data[offset + index] != b).Any();
        }
    }
}
