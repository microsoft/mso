// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma warning disable 1587
/// <summary>
///		Utility methods for converting tag string to integer value and vice versa
/// </summary>
#pragma warning restore 1587

using System.Globalization;

#if WSS && !CIRCUITBREAKER
namespace Microsoft.SharePoint.Diagnostics
#elif OSG
namespace Microsoft.Office.Server.Diagnostics
#elif PROJ_SHARED
namespace Microsoft.Office.Project.Server.Diagnostics
#elif SEARCH_FOUNDATION
namespace Microsoft.Ceres.Diagnostics
#elif WAC_SHARED
namespace Microsoft.Office.Web.Environment.Sharepoint.Diagnostics
#elif SERVERCONFIGCOMPONENTLIBRARY
namespace Microsoft.SharePoint.Configuration
#elif SHAREPOINT_ONLINE_COMMON
namespace Microsoft.Online.SharePoint.Common
#elif SPOPATCHINGCOMPONENTLIBRARY
namespace Microsoft.SharePoint.DataCenter.Patching
#elif SPO_CORE
namespace Microsoft.Online.SharePoint.Utility
#elif VROOM_CORE
namespace Microsoft.SharePoint.Diagnostics
#else
namespace Microsoft.Internal.Diagnostics
#endif
{
	/// <summary>
	/// Tagging Utilities
	/// </summary>
	internal sealed class TaggingUtilities
	{
		/// <summary>
		/// Private constructor to avoid automatically generated default public constructor
		/// </summary>
		private TaggingUtilities()
		{
		}

		/// <summary>
		/// Converts iTag into string representation
		/// This implementation is duplicated in \msoserviceplatform\src\browser\scripts\debug.cs.
		/// Please remember to update the duplicated code that targets Script#.
		/// </summary>
		/// <param name="tag">tag uint to be converted</param>
		/// <returns>The string representation of the provided tag.</returns>
		public static string GetTagFromiTag(uint tag)
		{
			if (tag <= s_maxNumericTag)
				return tag.ToString(CultureInfo.InvariantCulture);

			if ((tag >> 24) >= s_minOldSchemeHighByteValue)
			{
				// four letter tags are unpacked using direct ASCII equivalent of each byte
				return string.Format(CultureInfo.InvariantCulture, "{0}{1}{2}{3}",
								(char)((tag >> 24) & 0xFF),
								(char)((tag >> 16) & 0xFF),
								(char)((tag >>  8) & 0xFF),
								(char)((tag      ) & 0xFF)
								);
			}
			else
			{
				// five letter tags are unpacked using a different scheme
				// considering the tag as base 64 value,
				// unpacking each of the 6 bits into a letter (only lower 30 bits are used)
				// if any of the 6 bit groups have a value >=36 (i.e. size of the symbol space), this will throw IndexOutOfBoundsException
				// which is *expected*
				return string.Format(CultureInfo.InvariantCulture, "{0}{1}{2}{3}{4}",
								s_symbolSpace[(int)((tag >> 24) & 0x3F)],
								s_symbolSpace[(int)((tag >> 18) & 0x3F)],
								s_symbolSpace[(int)((tag >> 12) & 0x3F)],
								s_symbolSpace[(int)((tag >>  6) & 0x3F)],
								s_symbolSpace[(int)((tag      ) & 0x3F)]
								);
			}
		}

		/// <summary>
		/// Converts string into its iTag representation
		/// </summary>
		/// <param name="sTag">tag string to be converted</param>
		/// <returns>The uint representation of the provided tag.</returns>
		public static uint GetiTagFromTag(string sTag)
		{
			System.Diagnostics.Debug.Assert(sTag != null && sTag.Length > 0);

			uint result;
			// for tag IDs which are all numeric we convert the strings directly into integers

			if (uint.TryParse(sTag, out result) && result <= s_maxNumericTag)
				return result;

			System.Diagnostics.Debug.Assert(sTag.Length == 4 || sTag.Length == 5);
			if (sTag.Length == 4)
			{
				// for four letter tags, treat the ASCII values as bytes for the resulting value
				result = (uint)(sTag[0] << 24 | sTag[1] << 16 | sTag[2] << 8 | sTag[3]);
			}
			else
			{
				// five letter tags are packed as base 64 value
				result = (uint)(s_symbolSpace.IndexOf(sTag[0]) << 24 |
								s_symbolSpace.IndexOf(sTag[1]) << 18 |
								s_symbolSpace.IndexOf(sTag[2]) << 12 |
								s_symbolSpace.IndexOf(sTag[3]) <<  6 |
								s_symbolSpace.IndexOf(sTag[4])
							   );
			}
			return result;
		}

		/// <summary>
		/// Reserve an assert tag for use in a variable.
		/// </summary>
		/// <param name="iTag">tag id that the tagger has reserved</param>
		/// <returns>The uint tag passed in by the tagger.</returns>
		public static uint ReserveTag(uint iTag)
		{
			return iTag;
		}

		// the symbol space for 5 letter tags
		private const string s_symbolSpace = "abcdefghijklmnopqrstuvwxyz0123456789";
		// tag ids <= 65535 are reserved for critical events and are converted directly into string
		private const int s_maxNumericTag = 0x0000FFFF;
		// four letter tags (old scheme) will always have first byte >=36 (since it contains ASCII values a-z0-9$. )
		private const int s_minOldSchemeHighByteValue = 36;
	}
}
