// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
/*****************************************************************************

   Module  : oacrsal.h

******************************************************************************

   This file declares extensions to the SAL annotation macros in sal.h.

   It adds annotations for the following string types:

   _t: Pascal string
   _tz: both _z and _t

*****************************************************************************/

#include <sal.h>

// Don't allow deprecated Pre Orcas style annotations
#undef OACR_DEPRECATED
#define OACR_DEPRECATED 0

// Annotations for _t (pascal string) and _tz (nullterminated pascal string)

#define _In_t_                                          _Pre_t_      _Deref_pre_readonly_
#define _In_opt_t_                                      _Pre_opt_t_  _Deref_pre_readonly_
#define _In_tz_                                         _Pre_tz_     _Deref_pre_readonly_
#define _In_opt_tz_                                     _Pre_opt_tz_ _Deref_pre_readonly_

#define _Out_t_cap_(size)                               _Pre_cap_(size)           _Pre_invalid_ _Post_t_
#define _Out_opt_t_cap_(size)                           _Pre_opt_cap_(size)       _Pre_invalid_ _Post_t_
#define _Out_t_bytecap_(size)                           _Pre_bytecap_(size)       _Pre_invalid_ _Post_t_
#define _Out_opt_t_bytecap_(size)                       _Pre_opt_bytecap_(size)   _Pre_invalid_ _Post_t_
#define _Out_tz_cap_(size)                              _Pre_cap_(size)           _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_cap_(size)                          _Pre_opt_cap_(size)       _Pre_invalid_ _Post_tz_
#define _Out_tz_bytecap_(size)                          _Pre_bytecap_(size)       _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_bytecap_(size)                      _Pre_opt_bytecap_(size)   _Pre_invalid_ _Post_tz_

#define _Out_t_cap_c_(size)                             _Pre_cap_c_(size)         _Pre_invalid_ _Post_t_
#define _Out_opt_t_cap_c_(size)                         _Pre_opt_cap_c_(size)     _Pre_invalid_ _Post_t_
#define _Out_t_bytecap_c_(size)                         _Pre_bytecap_c_(size)     _Pre_invalid_ _Post_t_
#define _Out_opt_t_bytecap_c_(size)                     _Pre_opt_bytecap_c_(size) _Pre_invalid_ _Post_t_
#define _Out_tz_cap_c_(size)                            _Pre_cap_c_(size)         _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_cap_c_(size)                        _Pre_opt_cap_c_(size)     _Pre_invalid_ _Post_tz_
#define _Out_tz_bytecap_c_(size)                        _Pre_bytecap_c_(size)     _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_bytecap_c_(size)                    _Pre_opt_bytecap_c_(size) _Pre_invalid_ _Post_tz_

#define _Out_t_cap_x_(size)                             _Pre_cap_x_(size)         _Pre_invalid_ _Post_t_
#define _Out_opt_t_cap_x_(size)                         _Pre_opt_cap_x_(size)     _Pre_invalid_ _Post_t_
#define _Out_t_bytecap_x_(size)                         _Pre_bytecap_x_(size)     _Pre_invalid_ _Post_t_
#define _Out_opt_t_bytecap_x_(size)                     _Pre_opt_bytecap_x_(size) _Pre_invalid_ _Post_t_
#define _Out_tz_cap_x_(size)                            _Pre_cap_x_(size)         _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_cap_x_(size)                        _Pre_opt_cap_x_(size)     _Pre_invalid_ _Post_tz_
#define _Out_tz_bytecap_x_(size)                        _Pre_bytecap_x_(size)     _Pre_invalid_ _Post_tz_
#define _Out_opt_tz_bytecap_x_(size)                    _Pre_opt_bytecap_x_(size) _Pre_invalid_ _Post_tz_

#define _Out_tz_cap_post_count_(cap,count)              _Pre_cap_(cap)         _Pre_invalid_ _Post_tz_count_(count)
#define _Out_opt_tz_cap_post_count_(cap,count)          _Pre_opt_cap_(cap)     _Pre_invalid_ _Post_tz_count_(count)
#define _Out_tz_bytecap_post_bytecount_(cap,count)      _Pre_bytecap_(cap)     _Pre_invalid_ _Post_tz_bytecount_(count)
#define _Out_opt_tz_bytecap_post_bytecount_(cap,count)  _Pre_opt_bytecap_(cap) _Pre_invalid_ _Post_tz_bytecount_(count)

#define _Out_tz_capcount_(capcount)                     _Pre_cap_(capcount)         _Pre_invalid_ _Post_tz_count_(capcount)
#define _Out_opt_tz_capcount_(capcount)                 _Pre_opt_cap_(capcount)     _Pre_invalid_ _Post_tz_count_(capcount)
#define _Out_tz_bytecapcount_(capcount)                 _Pre_bytecap_(capcount)     _Pre_invalid_ _Post_tz_bytecount_(capcount)
#define _Out_opt_tz_bytecapcount_(capcount)             _Pre_opt_bytecap_(capcount) _Pre_invalid_ _Post_tz_bytecount_(capcount)

#define _Inout_t_                                       _Prepost_t_
#define _Inout_opt_t_                                   _Prepost_opt_t_
#define _Inout_tz_                                      _Prepost_tz_
#define _Inout_opt_tz_                                  _Prepost_opt_tz_

#define _Inout_t_cap_(size)                             _Pre_t_cap_(size)            _Post_t_
#define _Inout_opt_t_cap_(size)                         _Pre_opt_t_cap_(size)        _Post_t_
#define _Inout_t_bytecap_(size)                         _Pre_t_bytecap_(size)        _Post_t_
#define _Inout_opt_t_bytecap_(size)                     _Pre_opt_t_bytecap_(size)    _Post_t_
#define _Inout_tz_cap_(size)                            _Pre_tz_cap_(size)           _Post_tz_
#define _Inout_opt_tz_cap_(size)                        _Pre_opt_tz_cap_(size)       _Post_tz_
#define _Inout_tz_bytecap_(size)                        _Pre_tz_bytecap_(size)       _Post_tz_
#define _Inout_opt_tz_bytecap_(size)                    _Pre_opt_tz_bytecap_(size)   _Post_tz_

#define _Inout_t_cap_c_(size)                           _Pre_t_cap_c_(size)          _Post_t_
#define _Inout_opt_t_cap_c_(size)                       _Pre_opt_t_cap_c_(size)      _Post_t_
#define _Inout_t_bytecap_c_(size)                       _Pre_t_bytecap_c_(size)      _Post_t_
#define _Inout_opt_t_bytecap_c_(size)                   _Pre_opt_t_bytecap_c_(size)  _Post_t_
#define _Inout_tz_cap_c_(size)                          _Pre_tz_cap_c_(size)         _Post_tz_
#define _Inout_opt_tz_cap_c_(size)                      _Pre_opt_tz_cap_c_(size)     _Post_tz_
#define _Inout_tz_bytecap_c_(size)                      _Pre_tz_bytecap_c_(size)     _Post_tz_
#define _Inout_opt_tz_bytecap_c_(size)                  _Pre_opt_tz_bytecap_c_(size) _Post_tz_

#define _Inout_t_cap_x_(size)                           _Pre_t_cap_x_(size)          _Post_t_
#define _Inout_opt_t_cap_x_(size)                       _Pre_opt_t_cap_x_(size)      _Post_t_
#define _Inout_t_bytecap_x_(size)                       _Pre_t_bytecap_x_(size)      _Post_t_
#define _Inout_opt_t_bytecap_x_(size)                   _Pre_opt_t_bytecap_x_(size)  _Post_t_
#define _Inout_tz_cap_x_(size)                          _Pre_tz_cap_x_(size)         _Post_tz_
#define _Inout_opt_tz_cap_x_(size)                      _Pre_opt_tz_cap_x_(size)     _Post_tz_
#define _Inout_tz_bytecap_x_(size)                      _Pre_tz_bytecap_x_(size)     _Post_tz_
#define _Inout_opt_tz_bytecap_x_(size)                  _Pre_opt_tz_bytecap_x_(size) _Post_tz_

#define _Ret_t_                                         _Ret2_impl_(__notnull_impl,  __count_x_impl([0])) _Ret_valid_impl_ _$RET_STR_TYPE(_$postTStr)
#define _Ret_opt_t_                                     _Ret2_impl_(__maybenull_impl,__count_x_impl([0])) _Ret_valid_impl_ _$RET_STR_TYPE(_$postTStr)
#define _Ret_tz_                                        _Ret2_impl_(__notnull_impl,  __zterm_impl)        _Ret_valid_impl_ _$RET_STR_TYPE(_$postTZStr)
#define _Ret_opt_tz_                                    _Ret2_impl_(__maybenull_impl,__zterm_impl)        _Ret_valid_impl_ _$RET_STR_TYPE(_$postTZStr)

#define _Pre_t_                                         _Pre1_impl_(__notnull_impl_notref)   _Pre1_impl_(__count_x_impl([0])) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_                                     _Pre1_impl_(__maybenull_impl_notref) _Pre1_impl_(__count_x_impl([0])) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_tz_                                        _Pre1_impl_(__notnull_impl_notref)   _Pre1_impl_(__zterm_impl) _Pre_valid_impl_        _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_                                    _Pre1_impl_(__maybenull_impl_notref) _Pre1_impl_(__zterm_impl) _Pre_valid_impl_        _$STR_TYPE(_$preTZStr)

#define _Pre_t_cap_(size)                               _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__cap_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_cap_(size)                           _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__cap_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_t_bytecap_(size)                           _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_bytecap_(size)                       _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_tz_cap_(size)                              _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__cap_impl(size))            _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_cap_(size)                          _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__cap_impl(size))            _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_tz_bytecap_(size)                          _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__bytecap_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_bytecap_(size)                      _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__bytecap_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)

#define _Pre_t_cap_c_(size)                             _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_cap_c_(size)                         _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_t_bytecap_c_(size)                         _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_bytecap_c_(size)                     _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_tz_cap_c_(size)                            _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__cap_c_impl(size))            _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_cap_c_(size)                        _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__cap_c_impl(size))            _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_tz_bytecap_c_(size)                        _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__bytecap_c_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_bytecap_c_(size)                    _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__bytecap_c_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)

#define _Pre_t_cap_x_(size)                             _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_cap_x_(size)                         _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_t_bytecap_x_(size)                         _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_opt_t_bytecap_x_(size)                     _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Pre_valid_impl_ _$STR_TYPE(_$preTStr)
#define _Pre_tz_cap_x_(size)                            _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__cap_x_impl(_csize))          _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_cap_x_(size)                        _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__cap_x_impl(size))            _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_tz_bytecap_x_(size)                        _Pre1_impl_(__notnull_impl_notref)   _Pre2_impl_(__zterm_impl,__bytecap_x_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)
#define _Pre_opt_tz_bytecap_x_(size)                    _Pre1_impl_(__maybenull_impl_notref) _Pre2_impl_(__zterm_impl,__bytecap_x_impl(size))        _Pre_valid_impl_ _$STR_TYPE(_$preTZStr)

#define _Post_t_                                        _Post1_impl_(__count_x_impl([0]))  _Post_valid_impl_ _$STR_TYPE(_$postTStr)
#define _Post_tz_                                       _Post1_impl_(__zterm_impl)         _Post_valid_impl_ _$STR_TYPE(_$postTZStr)

// _Post_t_count family not possible, conflicting WritableElements property            

#define _Post_tz_count_(size)                           _Post2_impl_(__zterm_impl,__count_impl(size))       _Post_valid_impl_ _$STR_TYPE(_$postTZStr)
#define _Post_tz_bytecount_(size)                       _Post2_impl_(__zterm_impl,__bytecount_impl(size))   _Post_valid_impl_ _$STR_TYPE(_$postTZStr)
#define _Post_tz_count_c_(size)                         _Post2_impl_(__zterm_impl,__count_c_impl(size))     _Post_valid_impl_ _$STR_TYPE(_$postTZStr)
#define _Post_tz_bytecount_c_(size)                     _Post2_impl_(__zterm_impl,__bytecount_c_impl(size)) _Post_valid_impl_ _$STR_TYPE(_$postTZStr)
#define _Post_tz_count_x_(size)                         _Post2_impl_(__zterm_impl,__count_x_impl(size))     _Post_valid_impl_ _$STR_TYPE(_$postTZStr)
#define _Post_tz_bytecount_x_(size)                     _Post2_impl_(__zterm_impl,__bytecount_impl_x(size)) _Post_valid_impl_ _$STR_TYPE(_$postTZStr)

#define _Deref_out_t_                                   _Out_ _Deref_post_t_
#define _Deref_out_opt_t_                               _Out_ _Deref_post_opt_t_
#define _Deref_out_tz_                                  _Out_ _Deref_post_tz_
#define _Deref_out_opt_tz_                              _Out_ _Deref_post_opt_tz_

#define _Deref_pre_t_                                   _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre1_impl_(__count_x_impl([0])) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_                               _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre1_impl_(__count_x_impl([0])) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_tz_                                  _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre1_impl_(__zterm_impl) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_                              _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre1_impl_(__zterm_impl) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)

#define _Deref_pre_t_cap_(size)                         _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__cap_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_cap_(size)                     _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__cap_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_t_bytecap_(size)                     _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_bytecap_(size)                 _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_tz_cap_(size)                        _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__cap_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_cap_(size)                    _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__cap_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_tz_bytecap_(size)                    _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__bytecap_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_bytecap_(size)                _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__bytecap_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)

#define _Deref_pre_t_cap_c_(size)                       _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_cap_c_(size)                   _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_t_bytecap_c_(size)                   _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_bytecap_c_(size)               _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_tz_cap_c_(size)                      _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__cap_c_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_cap_c_(size)                  _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__cap_c_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_tz_bytecap_c_(size)                  _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__bytecap_c_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_bytecap_c_(size)              _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__bytecap_c_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)

#define _Deref_pre_t_cap_x_(size)                       _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_cap_x_(size)                   _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_t_bytecap_x_(size)                   _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_opt_t_bytecap_x_(size)               _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTStr)
#define _Deref_pre_tz_cap_x_(size)                      _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__cap_x_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_cap_x_(size)                  _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__cap_x_impl(size))     _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_tz_bytecap_x_(size)                  _Deref_pre1_impl_(__notnull_impl_notref)   _Deref_pre2_impl_(__zterm_impl,__bytecap_x_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)
#define _Deref_pre_opt_tz_bytecap_x_(size)              _Deref_pre1_impl_(__maybenull_impl_notref) _Deref_pre2_impl_(__zterm_impl,__bytecap_x_impl(size)) _Pre_valid_impl_ _$DEREF_STR_TYPE(_$preTZStr)

#define _Deref_post_t_                                  _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post1_impl_(__count_x_impl([0])) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_                              _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post1_impl_(__count_x_impl([0])) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_tz_                                 _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post1_impl_(__zterm_impl) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_                             _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post1_impl_(__zterm_impl) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)

#define _Deref_post_t_cap_(size)                        _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__cap_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_cap_(size)                    _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__cap_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_t_bytecap_(size)                    _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_bytecap_(size)                _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__bytecap_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_tz_cap_(size)                       _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__cap_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_cap_(size)                   _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__cap_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_tz_bytecap_(size)                   _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__bytecap_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_bytecap_(size)               _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__bytecap_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)

#define _Deref_post_t_cap_c_(size)                      _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_cap_c_(size)                  _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__cap_c_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_t_bytecap_c_(size)                  _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_bytecap_c_(size)              _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__bytecap_c_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_tz_cap_c_(size)                     _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__cap_c_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_cap_c_(size)                 _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__cap_c_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_tz_bytecap_c_(size)                 _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__bytecap_c_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_bytecap_c_(size)             _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__bytecap_c_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)

#define _Deref_post_t_cap_x_(size)                      _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_cap_x_(size)                  _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__cap_x_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_t_bytecap_x_(size)                  _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_opt_t_bytecap_x_(size)              _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__count_x_impl([0]),__bytecap_x_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTStr)
#define _Deref_post_tz_cap_x_(size)                     _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__cap_x_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_cap_x_(size)                 _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__cap_x_impl(size))     _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_tz_bytecap_x_(size)                 _Deref_post1_impl_(__notnull_impl_notref)   _Deref_post2_impl_(__zterm_impl),__bytecap_x_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)
#define _Deref_post_opt_tz_bytecap_x_(size)             _Deref_post1_impl_(__maybenull_impl_notref) _Deref_post2_impl_(__zterm_impl),__bytecap_x_impl(size)) _Post_valid_impl_ _$DEREF_STR_TYPE(_$postTZStr)

#define _Prepost_t_                                     _Pre_t_      _Post_t_
#define _Prepost_opt_t_                                 _Pre_opt_t_  _Post_t_
#define _Prepost_tz_                                    _Pre_tz_     _Post_tz_
#define _Prepost_opt_tz_                                _Pre_opt_tz_ _Post_tz_

#define _Deref_prepost_t_                               _Deref_pre_t_      _Deref_post_t_
#define _Deref_prepost_opt_t_                           _Deref_pre_opt_t_  _Deref_post_opt_t_
#define _Deref_prepost_tz_                              _Deref_pre_tz_     _Deref_post_tz_
#define _Deref_prepost_opt_tz_                          _Deref_pre_opt_tz_ _Deref_post_opt_tz_

// These are stronger than the related _opt_ annotations, in that the buffer pointer must be non-null if and only if
// the associated count parameter is non-zero. For example:
//
//    void Read(_In_reads_or_null_if_zero_(cBooks) const Book* rgBooks, unsigned int cBooks);
//
// will accept either of the following:
//
//    Read( nullptr, 0 );
//
//    Book[] rgBooks = { Book("I Robot"), Book("To Kill A Mockingbird") };
//    Read( rgBooks, 3 ); // or more concisely: Read( RgC( rgBooks ) )
//
// but not:
//
//    Read( rgBooks, 0 );
//
#define _In_reads_or_null_if_zero_(size)                _In_reads_(size)      _When_(0 == size, _Pre_null_) _When_(0 < size, _Pre_notnull_)
#define _Out_writes_or_null_if_zero_(size)              _Out_writes_(size)    _When_(0 == size, _Pre_null_) _When_(0 < size, _Pre_notnull_)
#define _Inout_updates_or_null_if_zero_(size)           _Inout_updates_(size) _When_(0 == size, _Pre_null_) _When_(0 < size, _Pre_notnull_)

#define _$STR_TYPE(type)
#define _$RET_STR_TYPE(type)
#define _$DEREF_STR_TYPE(type)

// Macros to support OACRCAT rule 25658(CAPTURE_FUNCTION_CALL)
// The tag parameter is required to disambiguate annotated functions
// The file and line at the annotation site is collected automatically by OACR_CAPTURE_CALLS_FILE_LINE

#if !defined(OACR_CAPTURE_CALLS)
#define OACR_CAPTURE_CALLS(tag) _SA_annotes1(SAL_OACRCapture, tag)
#endif

#if !defined(OACR_CAPTURE_CALLS_FILE_LINE)
#define OACR_CAPTURE_CALLS_FILE_LINE(tag) _SA_annotes3(SAL_OACRCapture, tag, __FILE__, __LINE__)
#endif

#if !defined(OACR_CAPTURE_FIXUP)
#define OACR_CAPTURE_FIXUP(tag,match,replace) _SA_annotes3(SAL_OACRCapture, tag_FIXUP, match, replace)
#endif
