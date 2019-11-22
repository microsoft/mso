// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// <summary>
//		Utility methods for converting tag string to integer value and vice versa
// </summary>
#if !TAGUTILS_SCRIPT
#define TAGUTILS_SCRIPT

using System;

#if SP_STOREFRONT
namespace SP.Storefront
#elif TAGUTILS_ACCSRV
namespace Microsoft.Office.Access.SharedClient
#elif TAGUTILS_STS
namespace SP
#elif TAGUTILS_MSOSERVICEPLATFORM
namespace BrowserDiagnosticsCore
#else
namespace Microsoft.Internal.Diagnostics
#endif
{
    /// <summary>
    /// Tagging Utilities
    /// </summary>
    [ConditionalPreserveName]
    public sealed class TaggingUtilities
    {
        /// <summary>
        /// Private constructor to avoid automatically generated default public constructor
        /// </summary>
        private TaggingUtilities()
        {
        }

        /// <summary>
        /// Converts iTag into string representation
        /// </summary>
        /// <param name="tag">tag uint to be converted</param>
        /// <returns></returns>
        [ConditionalPreserveName]
        public static string GetTagFromiTag(uint tag)
        {
            if (tag <= s_maxNumericTag)
                return tag.ToString();

            if ((tag >> 24) >= s_minOldSchemeHighByteValue)
            {
                // four letter tags are unpacked using direct ASCII equivalent of each byte
                return string.FromCharCode(new int[] {
                                            (int)((tag >> 24) & 0xFF),
                                            (int)((tag >> 16) & 0xFF),
                                            (int)((tag >>  8) & 0xFF),
                                            (int)((tag      ) & 0xFF)
                                            }
                                          );
            }

            string symbolSpace = s_symbolSpace; // s_symbolSpace is a long string, so better copy it to a variable to avoid repeating the literal in code five times

            // five letter tags are unpacked using a different scheme
            // considering the tag as base 64 value,
            // unpacking each of the 6 bits into a letter (only lower 30 bits are used)
            // if any of the 6 bit groups have a value >=36 (i.e. size of the symbol space), this will throw IndexOutOfBoundsException
            // which is *expected*
            return "" + symbolSpace.CharAt((int)((tag >> 24) & 0x3F)) +
                        symbolSpace.CharAt((int)((tag >> 18) & 0x3F)) +
                        symbolSpace.CharAt((int)((tag >> 12) & 0x3F)) +
                        symbolSpace.CharAt((int)((tag >>  6) & 0x3F)) +
                        symbolSpace.CharAt((int)((tag      ) & 0x3F));
        }

        [ConditionalPreserveName]
        public static uint GetiTagFromTag(string sTag)
        {
            uint result = 0;
            result = (uint)Number.ParseInt(sTag);

            // for tag IDs which are all numeric we convert the strings directly into integers
            if (result.ToString(10) == sTag && result <= s_maxNumericTag)
            {
                return result;
            }

            if (sTag.Length == 4)
            {
                // for four letter tags, treat the ASCII values as bytes for the resulting value
                result = (uint)((uint)sTag.CharAt(0) << 24 | (uint)sTag.CharAt(1) << 16 | (uint)sTag.CharAt(2) << 8 | (uint)sTag.CharAt(3));
            }
            else
            {
                string symbolSpace = s_symbolSpace; // s_symbolSpace is a long string, so better copy it to a variable to avoid repeating the literal in code five times

                // five letter tags are packed as base 64 value
                result = (uint)(symbolSpace.IndexOf(sTag.CharAt(0)) << 24 |
                                symbolSpace.IndexOf(sTag.CharAt(1)) << 18 |
                                symbolSpace.IndexOf(sTag.CharAt(2)) << 12 |
                                symbolSpace.IndexOf(sTag.CharAt(3)) <<  6 |
                                symbolSpace.IndexOf(sTag.CharAt(4))
                          );
            }
            return result;
        }

        /// <summary>
        /// Reserve an assert tag for use in a variable.
        /// </summary>
        /// <param name="iTag">tag id that the tagger has reserved</param>
        /// <returns>The uint tag passed in by the tagger.</returns>
        [ConditionalPreserveName]
        public static uint ReserveTag(uint iTag)
        {
            return iTag;
        }

        // the symbol space for 5 letter tags
        private const string s_symbolSpace = "abcdefghijklmnopqrstuvwxyz0123456789";
        // tag ids <= 65535 are reserved for critical events and are converted directly into string
        private const uint s_maxNumericTag = 0x0000FFFF;
        // four letter tags (old scheme) will always have first byte >=36 (since it contains ASCII values a-z0-9$. )
        private const uint s_minOldSchemeHighByteValue = 36;
    }
}

#endif
