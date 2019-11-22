// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// Minimal subset of SAL annotations to make core code happy under non-Windows compilers

#define _In_
#define _In_z_
#define _In_opt_z_
#define _In_opt_
#define _In_opt_count_(x)
#define _In_count_(x)
#define _Out_z_cap_c_(x)
#define _Out_z_cap_(x)
#define _Out_
#define _Outptr_
#define _Out_opt_
#define _Out_opt_cap_(x)
#define _Out_opt_z_cap_(x)
#define _Inout_
#define _Inout_updates_bytes_all_(x)
#define __inout_bcount(x)
#define _In_opt_bytecount_(x)
#define _Pre_maybenull_
#define _Post_invalid_
#define _Post_z_
#define _Ret_z_
#define _Ret_maybenull_
#define _Ret_notnull_
#define _Ret_range_(x,y)
#define _Post_writable_byte_size_(x)
#define _Deref_pre_valid_
#define _Deref_post_valid_
#define _Deref_post_count_(x)
#define _Success_(x)
#define _Printf_format_string_
#define _Use_decl_annotations_
#define _Null_terminated_
#define _Out_cap_post_count_(x,y)
#define _Out_cap_(x)
#define UNREFERENCED_PARAMETER(x)
#define _In_reads_bytes_(x)
#define __in_ecount(x)
#define __out_ecount_full(x)
#define _Notnull_
#define _COM_Outptr_
