using System.Collections.Generic;

namespace Mangos.BreakTrue
{
    public static class Extensions
    {
        public static T TryGet<T>(this IList<T> list, int index)
        {
            return index >= list.Count ? default(T) : list[index];
        }
    }
}
