" Vim syntax file
" Language:     LPC
" Maintainer:   Shizhu Pan <poet@mudbuilder.net>
" URL:          http://poet.tomud.com/pub/lpc.vim.bz2
" Last Change:  2016 Mar 25 by Tiger
" Comments:     If you are using Vim 6.2 or later, see :h lpc.vim for file type
"               recognizing, if not, you had to use modeline.
" Config:       This is an extended version of the one shipped with vim itself,
"               as such it now recognizes following variables:
"               b:lpc_folding:  Use syntax folding (default 1)
"               b:lpc_mudlib:   Core mudlib you're using (default: "")
"                               Currently supported:
"                               Morgengrauen
"                               Sagenwelt (currently in developmemt)
"               b:lpc_driver:   used driver (default: "FluffOS")
"                               This variable is also set by the choosen
"                               mudlib.
"                               Currently supported:
"                               LDMud
"                               MudOS
"                               FluffOS
"               Depending on the choosen driver following additional settings
"               are available:
"
"               MudOS-driver:
"               b:lpc_pre_v22:  Set to 1 for versions older than V2.2
"                               (default 0)
"               b:lpc_compat_32:
"                               Set to 1 if compiled for compatibility with
"                               LPmud 3.2/3.2.1
"                               (default 0)
"
"               FluffOS-driver:
"               If not otherwise stated these variables conform with
"               compiletime settings from 'local_options'. For a description
"               please consult 'local_options.README':
"               b:lpc_compat_32:
"                               (default 0)
"               b:lpc_sensible_modifiers:
"                               (default 1)
"               b:lpc_no_environment:
"                               (default 0)
"               b:lpc_no_add_action:
"                               (default 1)
"               b:lpc_no_buffer_type:
"                               Compiletime setting from options_internal.h
"                               (default 0)
"               b:lpc_no_snoop:
"                               (default 0)
"               b:lpc_no_resets:
"                               Runtime setting
"                               (default 0)
"               b:lpc_no_shadows:
"                               (default 0)
"               b:lpc_no_wizards:
"                               (default 1)
"               b:lpc_privs:
"                               (default 1)
"               b:lpc_old_ed:
"                               (default 0)
"               b:lpc_use_iconv:
"                               (default 1)
"
"               Additional following settings from 'c.vim' are recognized:
"                   c_comment_strings,c_no_if0,c_no_tab_space_error,
"                   c_no_trail_space_error,c_no_utf,c_space_errors

" Nodule: Todo for this file.  {{{1

" TODO : need to check for LPC4 syntax and other series of LPC besides
" v22, b21 and l32, if you had a good idea, contact me at poet@mudbuilder.net
" and I will be appreciated about that.
"
" - add/complete sefuns for
"   - Sagenwelt
"   - Morgengrauen
" - complete efuns for FluffOS (packages)

" Notes about some FAQ:
"
" About variables : We adopts the same behavior for C because almost all the
" LPC programmers are also C programmers, so we don't need separate settings
" for C and LPC. That is the reason why I don't change variables like
" "c_no_utf"s to "lpc_no_utf"s.
"
" Copy : Some of the following seems to be copied from c.vim but not quite
" the same in details because the syntax for C and LPC is different.
"
" Color scheme : this syntax file had been thouroughly tested to work well
" for all of the dark-backgrounded color schemes Vim has provided officially,
" and it should be quite Ok for all of the bright-backgrounded color schemes,
" of course it works best for the color scheme that I am using, download it
" from http://poet.tomud.com/pub/ps_color.vim.bz2 if you want to try it.
"

" Nodule: This is the start nodule. {{{1

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
    syn clear
elseif exists("b:current_syntax")
    finish
endif

let s:cpo_save = &cpo
set cpo&vim

" Nodule: Initializations {{{1

syn case match               " lpc is case sensitive

if !exists("b:lpc_folding")
  let b:lpc_folding = 1         " default use folding
endif
if b:lpc_folding && has("folding")
  let s:fold = " fold"
  setlocal fdm=syntax
else
  let s:fold = ""
endif

if !exists("b:lpc_mudlib")
    let b:lpc_mudlib = ""       " no default mudlib
endif

" we have a mudlib? then we know which driver...
if b:lpc_mudlib == "Morgengrauen"
    let b:lpc_driver = "LDMud"
elseif b:lpc_mudlib == "Sagenwelt" || b:lpc_mudlib == "lima"
    let b:lpc_driver = "FluffOS"
endif

if !exists("b:lpc_driver")
    let b:lpc_driver = "FluffOS"    " default driver
endif

if b:lpc_driver == "MudOS"
    if !exists("b:lpc_pre_v22")
        let b:lpc_pre_v22 = 0
    endif
    if !exists("b:lpc_compat_32")
        let b:lpc_compat_32 = 0
    endif
elseif b:lpc_driver == "FluffOS"
    let s:lpc_driver_has_closures = 0   " fluffos doesn't use closures
" some default settings for core package {{{2
    if !exists("b:lpc_compat32")
        let b:lpc_compat32 = 0
    endif
    if !exists("b:lpc_sensible_modifiers")
        let b:lpc_sensible_modifiers = 1
    endif
    if !exists("b:lpc_no_environment")
        let b:lpc_no_environment = 0
    endif
    if !exists("b:lpc_no_add_action")
        let b:lpc_no_add_action = 1
    endif
    if !exists("b:lpc_no_buffer_type")
        let b:lpc_no_buffer_type = 0
    endif
    if !exists("b:lpc_no_snoop")
        let b:lpc_no_snoop = 0
    endif
    if !exists("b:lpc_no_resets")
        let b:lpc_no_resets = 0
    endif
    if !exists("b:lpc_no_shadows")
        let b:lpc_no_shadows = 0
    endif
    if !exists("b:lpc_no_wizards")
        let b:lpc_no_wizards = 1
    endif
    if !exists("b:lpc_privs")
        let b:lpc_privs = 1
    endif
    if !exists("b:lpc_old_ed")
        let b:lpc_old_ed = 0
    endif
    if !exists("b:lpc_use_iconv")
        let b:lpc_use_iconv = 1
    endif                               " }}}
elseif b:lpc_driver == "LDMud"
    let s:lpc_driver_has_closures = 1
endif

syn sync fromstart

" Make sure these options take place since we no longer depend on file type
" plugin for C
setlocal cindent
setlocal fo-=t fo+=croql
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://

" Win32 can filter files in the browse dialog
if has("gui_win32") && !exists("b:browsefilter")
    let b:browsefilter = "LPC Source Files (*.c *.d *.h)\t*.c;*.d;*.h\n" .
                \ "LPC Data Files (*.scr *.o *.dat)\t*.scr;*.o;*.dat\n" .
                \ "Text Documentation (*.txt)\t*.txt\n" .
                \ "All Files (*.*)\t*.*\n"
endif

" Nodule: Keywords {{{1

" LPC keywords
" keywords should always be highlighted so "contained" is not used.
syn cluster lpcKeywdGrp     contains=lpcConditional,lpcLabel,lpcOperator,lpcRepeat,lpcStatement,lpcModifier,lpcReserved

syn keyword lpcConditional  if else switch
syn keyword lpcLabel        case default
syn keyword lpcOperator     try catch efun in inherit
syn keyword lpcRepeat       do for foreach while
syn keyword lpcStatement    break continue return

syn match   lpcEfunError    /efun[^:]/ display

" Illegal to use keyword as function
" It's not working, maybe in the next version.
syn keyword lpcKeywdError   contained if for foreach return switch while

" These are keywords only because they take lvalue or type as parameter,
" so these keywords should only be used as function but cannot be names of
" user-defined functions.
" parse_command sscanf
syn keyword lpcKeywdFunc    new time_expression

" Nodule: Type and modifiers {{{1

" Type names list.

" Special types
syn keyword lpcType void mixed unknown
" Scalar/Value types.
syn keyword lpcType int float string
" Pointer types.
syn keyword lpcType function mapping object

" Type modifier.
syn keyword lpcModifier nomask private public
syn keyword lpcModifier varargs

"------------
if b:lpc_driver == "LDMud"
    syn keyword lpcType     array closure status
    syn keyword lpcModifier deprecated extern static
elseif b:lpc_driver =="MudOS"
    " Other types.
    syn keyword lpcType     buffer class
    syn keyword lpcModifier virtual
    if b:lpc_compat_32
        syn keyword lpcType     closure status funcall
    else
        syn keyword lpcError    closure status
        syn keyword lpcType     multiset
    endif
    " sensible modifiers
    if b:lpc_pre_v22
        syn keyword lpcReserved nosave protected ref
        syn keyword lpcModifier static
    else
        syn keyword lpcError    static
        syn keyword lpcModifier nosave protected ref
    endif
elseif b:lpc_driver == "FluffOS"
    if b:lpc_sensible_modifiers
        syn keyword lpcModifier nosave protected ref
    else
        syn keyword lpcModifier static
    endif
    if !b:lpc_no_buffer_type
        syn keyword lpcType buffer
    endif
endif

" Nodule: Applies {{{1

" Match a function declaration or function pointer
syn match          lpcApplyDecl          excludenl /->\h\w*(/me=e-1 contains=lpcApplies transparent display

" We should note that in func_spec.c the efun definition syntax is so
" complicated that I use such a long regular expression to describe.
syn match          lpcLongDecl          excludenl /\(\s\|\*\)\h\+\s\h\+(/me=e-1 contains=@lpcEfunGroup,lpcType,@lpcKeywdGrp transparent display

" this is form for all functions
" ->foo() form had been excluded
syn match          lpcFuncDecl          excludenl /\h\w*(/me=e-1 contains=lpcApplies,@lpcEfunGroup,lpcKeywdError transparent display

if b:lpc_driver == "MudOS" || b:lpc_driver == "FluffOS"
    " The (: :) parenthesis or $() forms a function pointer
    syn match          lpcFuncName          /(:\s*\h\+\s*:)/me=e-1 contains=lpcApplies,@lpcEfunGroup transparent display contained
    syn match          lpcFuncName          /(:\s*\h\+,/ contains=lpcApplies,@lpcEfunGroup transparent display contained
    syn match          lpcFuncName          /\$(\h\+)/ contains=lpcApplies,@lpcEfunGroup transparent display contained
endif

" Applies list. {{{2
if b:lpc_driver == "LDMud"
elseif b:lpc_driver == "MudOS"
    "       system applies
    syn keyword     lpcApplies      contained __INIT clean_up create destructor heart_beat id init move_or_destruct reset
    "       interactive
    syn keyword     lpcApplies      contained catch_tell logon net_dead process_input receive_message receive_snoop telnet_suboption terminal_type window_size write_prompt
    "       master applies
    syn keyword     lpcApplies      contained author_file compile_object connect crash creator_file domain_file epilog error_handler flag get_bb_uid get_root_uid get_save_file_name log_error make_path_absolute object_name preload privs_file retrieve_ed_setup save_ed_setup slow_shutdown
    syn keyword     lpcApplies      contained valid_asm valid_bind valid_compile_to_c valid_database valid_hide valid_link valid_object valid_override valid_read valid_save_binary valid_seteuid valid_shadow valid_socket valid_write
    "       parsing
    syn keyword     lpcApplies      contained inventory_accessible inventory_visible is_living parse_command_adjectiv_id_list parse_command_adjective_id_list parse_command_all_word parse_command_id_list parse_command_plural_id_list parse_command_prepos_list parse_command_users parse_get_environment parse_get_first_inventory parse_get_next_inventory parser_error_message
elseif b:lpc_driver == "FluffOS"
    "       system applies
    syn keyword     lpcApplies      contained clean_up create destructor id init move_or_destruct reset
    "       interactive
    syn keyword     lpcApplies      contained catch_tell gmcp gmcp_enable logon mxp_enable mxp_tag net_dead process_input
    syn keyword     lpcApplies      contained receive_ed receive_environ receive_message receive_snoop telnet_suboption
    syn keyword     lpcApplies      contained terminal_colour_replace terminal_type window_size write_prompt zmp_command
    "       master applies
    syn keyword     lpcApplies      contained author_file compile_object connect crash creator_file domain_file epilog error_handler
    syn keyword     lpcApplies      contained flag get_bb_uid get_include_path get_mud_stats get_root_uid get_save_file_name log_error
    syn keyword     lpcApplies      contained make_path_absolute object_name parse_command_all_word parse_command_prepos_list
    syn keyword     lpcApplies      contained parse_command_prepos_list parse_command_users parse_get_environment
    syn keyword     lpcApplies      contained parse_get_first_inventory parse_get_next_inventory parser_error_message preload
    syn keyword     lpcApplies      contained privs_file retrieve_ed_setup save_ed_setup slow_shutdown valid_bind valid_database
    syn keyword     lpcApplies      contained valid_hide valid_link valid_object valid_override valid_read valid_seteuid valid_shadow
    syn keyword     lpcApplies      contained valid_socket valid_write
    "       parsing
    syn keyword     lpcApplies      contained parse_command_adjectiv_id_list parse_command_adjective_id_list parse_command_id_list
    syn keyword     lpcApplies      contained parse_command_plural_id_list
endif

" Module: Sefuns {{{1
" sorted by mudlib
if b:lpc_mudlib == "Morgengrauen"
"    syn keyword     lpc_sefuns   abs acos add_action add_worth all_environment allocate and_bits apply nextgroup=lpcSEfunParen
elseif b:lpc_mudlib == "Sagenwelt"
    syn keyword lpc_sefuns  contained Dcreatorp Dlordp add_article archp atoi author_of basename blink bold canonical_path
    syn keyword lpc_sefuns  contained clear_line clear_screen cmp creatorp cred_cmp debug_info destruct dirname distinct_array
    syn keyword lpc_sefuns  contained domain_of done_startup elder erase_line exec fib file_name fnmatch gcd get_cwd get_debug
    syn keyword lpc_sefuns  contained getegid getgid getugid getuid glob gsub has_magic i_wrap init_eids insensitive_pattern
    syn keyword lpc_sefuns  contained insensitive_regexp inverse itoa lcm m_syslog move_object normal opposite_dir playerp printf
    syn keyword lpc_sefuns  contained reg_pat_translate regexplode remove_article reset_eval_cost rsearch say scramble_array
    syn keyword lpc_sefuns  contained search set_bg_color set_debug set_eval_limit set_fg_color setegid seteuid shout shutdown
    syn keyword lpc_sefuns  contained simul_efun split sub syslog tell_object tell_room underscore unique_mapping up_line write
endif
" Nodule: Efuns {{{1
" sorted by driver

syn cluster          lpcEfunGroup          contains=lpc_efuns,lpc_sefuns,lpcOldEfuns,lpcNewEfuns,lpcKeywdFunc

if b:lpc_driver == "LDMud"          " {{{2
    syn keyword     lpc_efuns   contained abs acos add_action add_worth all_environment allocate and_bits apply asin assoc atan atan2
    syn keyword     lpc_efuns   contained attach_erq_demon baseof binary_message bind_lambda blueprint break_point break_string
    syn keyword     lpc_efuns   contained call_direct call_direct_resolved call_other call_out call_out_info call_resolved
    syn keyword     lpc_efuns   contained caller_stack caller_stack_depth capitalize cat catch ceil cident clear_bit clone_object
    syn keyword     lpc_efuns   contained clonep clones closureup command command_stack command_stack_depth configure_driver
    syn keyword     lpc_efuns   contained convert_charset copy copy_bits copy_file cos count_bits create_wizard creator crypt ctime
    syn keyword     lpc_efuns   contained db_affected_rows db_close db_coldefs db_connect db_conv_string db_error db_exec db_fetch
    syn keyword     lpc_efuns   contained db_handles db_insert_id debug_info debug_message deep_copy deep_inventory deep_present
    syn keyword     lpc_efuns   contained destruct disable_commands domainshout dtime dump_netdead ed efun enable_commands
    syn keyword     lpc_efuns   contained enable_telnet end_mccp_compress environment exec execute_command exlude_alist exp
    syn keyword     lpc_efuns   contained expand_define explode export_uid extern_call file_size file_time filter filter_indices
    syn keyword     lpc_efuns   contained filter_objects find_call_out find_input_to find_living find_livings find_netdead
    syn keyword     lpc_efuns   contained find_object find_player first_inventory floatp floor funcall function_exists functionlist
    syn keyword     lpc_efuns   contained garbage_collection get_combine_charset get_connection_charset get_dir get_error_file
    syn keyword     lpc_efuns   contained get_eval_cost get_extra_wizinfo get_max_commands get_type_info geteuid getuid getuuid
    syn keyword     lpc_efuns   contained gmtime hash heart_beat_info hmac idna_stringprep idna_to_ascii idna_to_unicode implode
    syn keyword     lpc_efuns   contained include_list inherit_list input_to input_to_info insert_alist insert_alist.orig
    syn keyword     lpc_efuns   contained insert_alist.rej interactive intersect_alist intp invert_bits iso2ascii lambda last_bit
    syn keyword     lpc_efuns   contained last_instructions limited living load_name load_object localtime log log_file lower_case
    syn keyword     lpc_efuns   contained lowerchar lowerstring m_add m_allocate m_contains m_delete m_entry m_indices m_reallocate
    syn keyword     lpc_efuns   contained m_values m_width make_shared_string map map_indices map_objects mappingp master
    syn keyword     lpc_efuns   contained match_command match_living max md5 md5_crypt member min mkdir mkmapping modify_command
    syn keyword     lpc_efuns   contained move_object negate net_connect new_explode next_bit next_inventory notify_fail object_info
    syn keyword     lpc_efuns   contained object_name object_time objectp old_explode or_bits order_alist parse_command people
    syn keyword     lpc_efuns   contained pg_close pg_connect pg_conv_string pg_pending pg_query pointerp pow present present_clone
    syn keyword     lpc_efuns   contained previous_object printf process_call process_string program_name program_time query_actions
    syn keyword     lpc_efuns   contained query_command query_editing query_host_name query_idle query_input_pending query_ip_name
    syn keyword     lpc_efuns   contained query_ip_number query_limits query_load_average query_mccp query_mccp_stats query_mud_port
    syn keyword     lpc_efuns   contained query_next_reset query_notify_fail query_once_interactive query_shadowing query_snoop
    syn keyword     lpc_efuns   contained query_udp_port query_verb query_wiz_grp query_wiz_level quote raise_error random read_bytes
    syn keyword     lpc_efuns   contained read_file receive_udp referencep regexp regexp_package regexplode regmatch regreplace
    syn keyword     lpc_efuns   contained remove_action remove_alist remove_call_out remove_input_to remove_interactive rename
    syn keyword     lpc_efuns   contained rename_object replace_personal replace_program restore_object restore_value reverse rm
    syn keyword     lpc_efuns   contained rmdir rmember rusage save_objects save_value say secure_level send_erq send_udp set_bit
    syn keyword     lpc_efuns   contained set_buffer_size set_combine_charset set_connection_charset set_driver_hook set_environment
    syn keyword     lpc_efuns   contained set_environment set_extra_wizinfo set_extra_wizinfo_size set_heart_beat set_is_wizard
    syn keyword     lpc_efuns   contained set_light set_limits set_living_name set_max_commands set_modify_command set_next_reset
    syn keyword     lpc_efuns   contained set_object_heart_beat set_prompt set_this_object set_this_player seteuid sgn sha1 shadow
    syn keyword     lpc_efuns   contained shout shutdown sin sizeof sl_close sl_exec sl_insert_id sl_open snoop sort_array sprintf
    syn keyword     lpc_efuns   contained sqrt sscanf start_mccp_compress strftime stringp strlen strrstr strstr struct_info structp
    syn keyword     lpc_efuns   contained swap symbol_function symbol_variable symbolp tail tan tell_object tell_room terminal_colour
    syn keyword     lpc_efuns   contained test_bit this_interactive this_object this_player throw time time2string tls_available
    syn keyword     lpc_efuns   contained tls_check_certificate tls_deinit_connection tls_error tls_init_connection
    syn keyword     lpc_efuns   contained tls_query_connection_info tls_query_connection_state to_array to_float to_int to_object
    syn keyword     lpc_efuns   contained to_string to_struct trace traceprefix transfer transpose_array trim typeof unbound_lambda
    syn keyword     lpc_efuns   contained unique_array unmkmapping unquote unshadow update_actions upper_case upperstring uptime
    syn keyword     lpc_efuns   contained users utime walk_mapping widthof wizlist wizlist_info write write_bytes write_file
elseif b:lpc_driver ==  "MudOS"         " {{{2
    " Compat32 efuns
    if b:lpc_compat_32
        syn keyword lpc_efuns   contained closurep heart_beat_info m_delete m_values m_indices query_once_interactive strstr
    else
        syn match   lpcErrFunc  /#`\h\w*/
        " Shell compatible first line comment.
        syn region  lpcCommentFunc  start=/^#!/ end=/$/
    endif

    " pre-v22 efuns which are removed in newer versions.
    syn keyword     lpcOldEfuns     contained tail dump_socket_status

    " new efuns after v22 should be added here!
    syn keyword     lpcNewEfuns     contained socket_status

    " LPC efuns list.
    " DEBUG efuns Not included.
    " New efuns should NOT be added to this list, see v22 efuns above.
    syn keyword     lpc_efuns       contained acos add_action all_inventory all_previous_objects allocate allocate_buffer allocate_mapping apply arrayp asin atan author_stats
    syn keyword     lpc_efuns       contained bind break_string bufferp
    syn keyword     lpc_efuns       contained cache_stats call_other call_out call_out_info call_stack capitalize catch ceil check_memory children classp clear_bit clone_object clonep command commands copy cos cp crc32 crypt ctime
    syn keyword     lpc_efuns       contained db_close db_commit db_connect db_exec db_fetch db_rollback db_status debug_info debugmalloc debug_message deep_inherit_list deep_inventory destruct disable_commands disable_wizard domain_stats dumpallobj dump_file_descriptors dump_prog
    syn keyword     lpc_efuns       contained each ed ed_cmd ed_start enable_commands enable_wizard environment error errorp eval_cost evaluate exec exp explode export_uid external_start
    syn keyword     lpc_efuns       contained fetch_variable file_length file_name file_size filter filter_array filter_mapping find_call_out find_living find_object find_player first_inventory floatp floor flush_messages function_exists function_owner function_profile functionp functions
    syn keyword     lpc_efuns       contained generate_source get_char get_config get_dir geteuid getuid
    syn keyword     lpc_efuns       contained heart_beats
    syn keyword     lpc_efuns       contained id_matrix implode in_edit in_input inherit_list inherits input_to interactive intp
    syn keyword     lpc_efuns       contained keys
    syn keyword     lpc_efuns       contained link living livings load_object localtime log log10 lookat_rotate lower_case lpc_info
    syn keyword     lpc_efuns       contained malloc_check malloc_debug malloc_status map map_array map_delete map_mapping mapp master match_path max_eval_cost member_array memory_info memory_summary message mkdir moncontrol move_object mud_status
    syn keyword     lpc_efuns       contained named_livings network_stats next_bit next_inventory notify_fail nullp
    syn keyword     lpc_efuns       contained objectp objects oldcrypt opcprof origin
    syn keyword     lpc_efuns       contained parse_add_rule parse_add_synonym parse_command parse_dump parse_init parse_my_rules parse_refresh parse_remove parse_sentence pluralize pointerp pow present previous_object printf process_string process_value program_info
    syn keyword     lpc_efuns       contained query_ed_mode query_heart_beat query_host_name query_idle query_ip_name query_ip_number query_ip_port query_load_average query_notify_fail query_privs query_replaced_program query_shadowing query_snoop query_snooping query_verb
    syn keyword     lpc_efuns       contained random read_buffer read_bytes read_file receive reclaim_objects refs regexp reg_assoc reload_object remove_action remove_call_out remove_interactive remove_shadow rename repeat_string replace_program replace_string replaceable reset_eval_cost resolve restore_object restore_variable rm rmdir rotate_x rotate_y rotate_z rusage
    syn keyword     lpc_efuns       contained save_object save_variable say scale set_author set_bit set_eval_limit set_heart_beat set_hide set_light set_living_name set_malloc_mask set_privs set_reset set_this_player set_this_user seteuid shadow shallow_inherit_list shout shutdown sin sizeof snoop socket_accept socket_acquire socket_address socket_bind socket_close socket_connect socket_create socket_error socket_listen socket_release socket_write sort_array sprintf sqrt stat store_variable strcmp stringp strlen strsrch
    syn keyword     lpc_efuns       contained tan tell_object tell_room terminal_colour test_bit this_interactive this_object this_player this_user throw time to_float to_int trace traceprefix translate typeof
    syn keyword     lpc_efuns       contained undefinedp unique_array unique_mapping upper_case uptime userp users
    syn keyword     lpc_efuns       contained values variables virtualp
    syn keyword     lpc_efuns       contained wizardp write write_buffer write_bytes write_file
elseif b:lpc_driver == "FluffOS"         " {{{2
    syn keyword     lpc_efuns   contained act_mxp all_previous_objects allocate allocate_mapping arrayp bind cache_stats call_other
    syn keyword     lpc_efuns   contained call_out call_out_info call_out_walltime call_stack capitalize children classp clear_bit
    syn keyword     lpc_efuns   contained clear_debug_level clone_object clonep cp crc32 crypt ctime debug_levels deep_inherit_list
    syn keyword     lpc_efuns   contained defer destruct dump_file_descriptors dumpallobj error eval_cost evaluate exec explode
    syn keyword     lpc_efuns   contained file_name file_size filter filter_array filter_mapping find_call_out find_object floatp
    syn keyword     lpc_efuns   contained flush_messages function_exists function_profile functionp get_char get_config get_dir
    syn keyword     lpc_efuns   contained has_gmcp has_mxp has_zmp implode in_edit in_input inherit_list inherits input_to
    syn keyword     lpc_efuns   contained interactive intp keys link load_object localtime lower_case malloc_status map map_array
    syn keyword     lpc_efuns   contained map_delete map_mapping mapp master match_path max_eval_cost member_array memory_info
    syn keyword     lpc_efuns   contained message mkdir mud_status next_bit nullp objectp objects oldcrypt origin pointerp
    syn keyword     lpc_efuns   contained previous_object printf query_heart_beat query_host_name query_idle query_ip_name
    syn keyword     lpc_efuns   contained query_ip_number query_load_average random read_bytes read_file receive reclaim_objects
    syn keyword     lpc_efuns   contained reg_assoc regexp reload_object remove_call_out rename replace_program replace_string
    syn keyword     lpc_efuns   contained request_term_size request_term_type reset_eval_cost resolve restore_object restore_variable
    syn keyword     lpc_efuns   contained rm rmdir rusage save_object save_variable send_gmcp send_zmp set_bit set_config
    syn keyword     lpc_efuns   contained set_debug_level set_eval_limit set_heart_beat set_hide shallow_inherit_list shout shutdown
    syn keyword     lpc_efuns   contained sizeof sort_array sprintf start_request_term_type stat strcmp stringp strlen strsrch
    syn keyword     lpc_efuns   contained strwidth tell_object test_bit this_interactive this_object this_player this_user throw time
    syn keyword     lpc_efuns   contained to_float to_int typeof undefinedp unique_array unique_mapping uptime userp users values
    syn keyword     lpc_efuns   contained virtualp websocket_handshake_done write write_bytes write_file
    if b:lpc_compat32
        syn keyword     lpc_efuns   contained closurep funcall get_eval_cost m_delete m_indices m_values
        syn keyword     lpc_efuns   contained query_once_interactive raise_error strstr
    else
        syn keyword     lpcOldEfuns closurep funcall get_eval_cost m_delete m_indices m_values
        syn keyword     lpcOldEfuns query_once_interactive raise_error strstr
    endif
    if !b:lpc_no_environment
        syn keyword     lpc_efuns   contained all_inventory deep_inventory environment first_inventory move_object
        syn keyword     lpc_efuns   contained next_inventory present say tell_room
    else
        syn keyword     lpcOldEfuns all_inventory deep_inventory environment first_inventory move_object
        syn keyword     lpcOldEfuns next_inventory present say tell_room
    endif
    if b:lpc_no_add_action
        syn keyword     lpcOldEfuns add_action command commands disable_commands enable_commands find_living
        syn keyword     lpcOldEfuns find_player living livings notify_fail query_verb remove_action set_living_name
        syn keyword     lpc_efuns   contained set_this_player set_this_user
    else
        syn keyword     lpc_efuns   contained add_action command commands disable_commands enable_commands find_living
        syn keyword     lpc_efuns   contained find_player living livings notify_fail query_verb remove_action set_living_name
        syn keyword     lpcOldEfuns set_this_player set_this_user
    endif
    if !b:lpc_no_buffer_type
        syn keyword     lpc_efuns   contained allocate_buffer bufferp read_buffer write_buffer
    else
        syn keyword     lpcOldEfuns allocate_buffer bufferp read_buffer write_buffer
    endif
    if !b:lpc_no_snoop
        syn keyword     lpc_efuns   contained snoop query_snoop query_snooping
    else
        syn keyword     lpcOldEfuns snoop query_snoop query_snooping
    endif
    if !b:lpc_no_resets
        syn keyword     lpc_efuns   contained set_reset
    else
        syn keyword     lpcOldEfuns set_reset
    endif
    if !b:lpc_no_shadows
        syn keyword     lpc_efuns   contained query_shadowing shadow
    else
        syn keyword     lpcOldEfuns query_shadowing shadow
    endif
    if !b:lpc_no_wizards
        syn keyword     lpc_efuns   contained disable_wizard enable_wizard wizardp
    else
        syn keyword     lpcOldEfuns disable_wizard enable_wizard wizardp
    endif
    if !b:lpc_privs
        syn keyword     lpc_efuns   contained query_privs set_privs
    else
        syn keyword     lpcOldEfuns query_privs set_privs
    endif
    if b:lpc_old_ed
        syn keyword     lpc_efuns   contained ed
        syn keyword     lpcOldEfuns ed_cmd ed_start query_ed_mode
    else
        syn keyword     lpcOldEfuns ed
        syn keyword     lpc_efuns   contained ed_cmd ed_start query_ed_mode
    endif
    if b:lpc_use_iconv
        syn keyword     lpc_efuns   contained arr_to_str set_encoding str_to_arr to_utf8 utf8_to
    else
        syn keyword     lpcOldEfuns arr_to_str set_encoding str_to_arr to_utf8 utf8_to
    endif
endif
" Nodule: Constants {{{1
" sorted by driver

" LPC Constants.
" like keywords, constants are always highlighted
if b:lpc_driver == "LDMud"              " {{{2
elseif b:lpc_driver == "MudOS"          " {{{2
    " be careful to choose only
    " the constants we used to add to this list.
    syn keyword     lpcConstant     __ARCH__ __COMPILER__ __DIR__ __FILE__ __OPTIMIZATION__ __PORT__ __VERSION__
    "       Defines in options.h are all predefined in LPC sources surrounding by
    "       two underscores. Do we need to include all of that?
    syn keyword     lpcConstant     __SAVE_EXTENSION__ __HEARTBEAT_INTERVAL__
    "       from the documentation we know that these constants remains only for
    "       backward compatibility and should not be used any more.
    syn keyword     lpcConstant     HAS_ED HAS_PRINTF HAS_RUSAGE HAS_DEBUG_LEVEL
    syn keyword     lpcConstant     MUD_NAME F__THIS_OBJECT
elseif b:lpc_driver == "FluffOS"        " {{{2
    syn keyword     lpcConstant     FLUFFOS HAS_DEBUG_LEVEL HAS_ED HAS_PRINTF HAS_RUSAGE MAX_FLOAT MAX_INT MIN_FLOAT MIN_INT
    syn keyword     lpcConstant     MUDOS MUD_NAME SIZEOFINT __ARCH__ __ARGUMENTS_IN_TRACEBACK__ __ARRAY_RESERVED_WORD__
    syn keyword     lpcConstant     __ARRAY_STATS__ __AUTO_SETEUID__ __AUTO_TRUST_BACKBONE__ __CACHE_STATS__ __CALLOUT_HANDLES__
    syn keyword     lpcConstant     __CALL_OTHER_TYPE_CHECK__ __CALL_OTHER_WARN__ __CFG_COMPILER_STACK_SIZE__
    syn keyword     lpcConstant     __CFG_EVALUATOR_STACK_SIZE__ __CFG_LIVING_HASH_SIZE__ __CFG_MAX_CALL_DEPTH__
    syn keyword     lpcConstant     __CFG_MAX_GLOBAL_VARIABLES__ __CFG_MAX_LOCAL_VARIABLES__ __CHECK_MEMORY__ __CLASS_STATS__
    syn keyword     lpcConstant     __COMMAND_BUF_SIZE__ __COMPAT_32__ __COMPILER__ __CUSTOM_CRYPT__ __CXXFLAGS__
    syn keyword     lpcConstant     __DEBUGMALLOC_EXTENSIONS__ __DEBUGMALLOC__ __DEBUG_MACRO__ __DEBUG__ __DEFAULT_DB__
    syn keyword     lpcConstant     __DEFAULT_PRAGMAS__ __DSLIB__ __DTRACE__ __DWLIB__ __ED_INDENT_CASE__ __ED_INDENT_SPACES__
    syn keyword     lpcConstant     __GET_CHAR_IS_BUFFERED__ __HAS_CONSOLE__ __INTERACTIVE_CATCH_TELL__ __IPV6__
    syn keyword     lpcConstant     __LARGEST_PRINTABLE_STRING__ __LARGE_STRING_SIZE__ __LAZY_RESETS__ __LOCALS_IN_TRACEBACK__
    syn keyword     lpcConstant     __MAX_SAVE_SVALUE_DEPTH__ __MUDLIB_ERROR_HANDLER__ __NONINTERACTIVE_STDERR_WRITE__
    syn keyword     lpcConstant     __NO_ADD_ACTION__ __NO_BUFFER_TYPE__ __NO_ENVIRONMENT__ __NO_LIGHT__ __NO_RESETS__
    syn keyword     lpcConstant     __NO_SHADOWS__ __NO_SNOOP__ __NO_WIZARDS__ __OLD_ED__ __OLD_RANGE_BEHAVIOR__
    syn keyword     lpcConstant     __OLD_TYPE_BEHAVIOR__ __OPTIMIZATION__ __PACKAGE_ASYNC__ __PACKAGE_COMPRESS__
    syn keyword     lpcConstant     __PACKAGE_CONTRIB__ __PACKAGE_CORE__ __PACKAGE_CRYPTO__ __PACKAGE_DB__ __PACKAGE_DB__
    syn keyword     lpcConstant     __PACKAGE_DEVELOP__ __PACKAGE_DWLIB__ __PACKAGE_EXTERNAL__ __PACKAGE_MATH__
    syn keyword     lpcConstant     __PACKAGE_MATRIX__ __PACKAGE_MUDLIB_STATS__ __PACKAGE_OPS__ __PACKAGE_PARSER__
    syn keyword     lpcConstant     __PACKAGE_PCRE__ __PACKAGE_SHA1__ __PACKAGE_SOCKETS__ __PACKAGE_THREAD__ __PACKAGE_TRIM__
    syn keyword     lpcConstant     __PACKAGE_UIDS__ __PORT__ __PRIVS__ __PROFILE_FUNCTIONS__ __RANDOMIZED_RESETS__
    syn keyword     lpcConstant     __RECEIVE_ED__ __RECEIVE_SNOOP__ __REF_RESERVED_WORD__ __RESTRICTED_ED__ __REVERSE_DEFER__
    syn keyword     lpcConstant     __REVERSIBLE_EXPLODE_STRING__ __SANE_EXPLODE_STRING__ __SANE_SORTING__ __SAVE_EXTENSION__
    syn keyword     lpcConstant     __SAVE_GZ_EXTENSION__ __SENSIBLE_MODIFIERS__ __SMALL_STRING_SIZE__ __SNOOP_SHADOWED__
    syn keyword     lpcConstant     __STRING_STATS__ __STRUCT_CLASS__ __STRUCT_STRUCT__ __SUPPRESS_ARGUMENT_WARNINGS__
    syn keyword     lpcConstant     __THIS_PLAYER_IN_CALL_OUT__ __TRACE_CODE__ __TRACE__ __TRAP_CRASHES__ __USE_32BIT_ADDRESSES__
    syn keyword     lpcConstant     __USE_ICONV__ __USE_MSQL__ __USE_MYSQL__ __USE_POSTGRES__ __USE_SQLITE3__ __VERSION__
    syn keyword     lpcConstant     __WARN_OLD_RANGE_BEHAVIOR__ __WOMBLES__
endif
" Nodule: String and Character {{{1

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn match   lpcSpecial          display contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"
if !exists("c_no_utf")
    syn match   lpcSpecial      display contained "\\\(u\x\{4}\|U\x\{8}\)"
endif

" LPC version of sprintf() format,
syn match   lpcFormat          display "%\(\d\+\)\=[-+ |=#@:.]*\(\d\+\)\=\('\I\+'\|'\I*\\'\I*'\)\=[OsdicoxXf]" contained
syn match   lpcFormat          display "%%" contained
syn region  lpcString          start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=lpcSpecial,lpcFormat
" lpcCppString: same as lpcString, but ends at end of line
syn region  lpcCppString        start="L\=\"" skip="\\\\\|\\\"\|\\$" excludenl end="\"" end="$" contains=lpcSpecial,lpcFormat

" LPC preprocessor for the text formatting short cuts
" Thanks to Dr. Charles E. Campbell <cec@gryphon.gsfc.nasa.gov>
"          he suggests the best way to do this.
syn region  lpcTextString       start=/@\z(\h\w*\)$/ end=/^\z1/ contains=lpcSpecial
syn region  lpcArrayString      start=/@@\z(\h\w*\)$/ end=/^\z1/ contains=lpcSpecial

" Character
syn match   lpcCharacter        "L\='[^\\]'"
syn match   lpcCharacter        "L'[^']*'" contains=lpcSpecial
syn match   lpcSpecialError     "L\='\\[^'\"?\\abefnrtv]'"
syn match   lpcSpecialCharacter "L\='\\['\"?\\abefnrtv]'"
syn match   lpcSpecialCharacter display "L\='\\\o\{1,3}'"
syn match   lpcSpecialCharacter display "'\\x\x\{1,2}'"
syn match   lpcSpecialCharacter display "L'\\x\x\+'"

" Nodule: White space {{{1

" when wanted, highlight trailing white space
if exists("c_space_errors")
    if !exists("c_no_trail_space_error")
        syn match          lpcSpaceError          display excludenl "\s\+$"
    endif
    if !exists("c_no_tab_space_error")
        syn match          lpcSpaceError          display " \+\t"me=e-1
    endif
endif

" Nodule: Parenthesis and brackets {{{1

" catch errors caused by wrong parenthesis and brackets
syn cluster     lpcParenGroup       contains=@lpcCommentGroup,lpcBitField,lpcComment2String,lpcCommentSkip,lpcCommentSkip
syn cluster     lpcParenGroup            add=lpcCommentStartError,lpcCommentString,lpcCppOut,lpcCppOut2,lpcCppSkip,lpcCurlyError
syn cluster     lpcParenGroup            add=lpcFloat,lpcFormat,lpcIncluded,lpcKeywdError,lpcNumber,lpcNumbersCom,lpcOctal
syn cluster     lpcParenGroup            add=lpcOctalError,lpcOctalZero,lpcParenError,lpcSpecial,lpcUserCont,lpcUserLabel

" for LPC:
" Here we should consider the array ({ }) parenthesis and mapping ([ ])
" parenthesis and driver dependend multiset (< >) parenthesis and functional
" (: :) parenthesis.

" This should be before cErrInParen to avoid problems with #define ({ xxx })
syn match       lpcCurlyError       "}"
exec 'syn region      lpcBlock            start="{" end="}" contains=ALLBUT,lpcBadBlock,@lpcParenGroup,lpcErrInParen,lpcCppParen,lpcErrInBracket,lpcCppBracket,lpcCppString,@Spell' . s:fold
exec 'syn region      lpcBadBlock         keepend start="{" end="}" contained containedin=lpcParen,lpcBracket,lpcBadBlock,lpcArray,lpcMapping transparent' . s:fold

syn match       lpcParenError       display ")"
syn match       lpcParenError       display "\]"
exec 'syn region      lpcParen            transparent start="(" end=")" contains=ALLBUT,lpcBlock,@lpcParenGroup,lpcCppParen,lpcErrInBracket,lpcCppBracket,lpcCppString,@lpcEfunGroup,lpcApplies' . s:fold
" lpcCppParen: same as lpcParen but ends at end-of-line; used in lpcDefine
syn region      lpcCppParen         transparent start="(" skip="\\$" excludenl end=")" end="$" contained contains=ALLBUT,@lpcParenGroup,lpcErrInBracket,lpcParen,lpcBracket,lpcString,@lpcEfunGroup,lpcApplies
syn match       lpcErrInParen       display contained "[^^]{"ms=s+1
syn match       lpcErrInParen       display contained "\(}\|\]\)[^)]"me=e-1
exec 'syn region      lpcBracket          transparent start="\[" end="]" contains=ALLBUT,lpcBlock,@lpcParenGroup,lpcErrInParen,lpcCppParen,lpcCppBracket,lpcCppString,@lpcEfunGroup,lpcApplies,lpcFuncName' . s:fold
" lpcCppBracket: same as lpcParen but ends at end-of-line; used in lpcDefine
syn region      lpcCppBracket       transparent start="\[" skip="\\$" excludenl end="]" end="$" contained contains=ALLBUT,@lpcParenGroup,lpcErrInParen,lpcParen,lpcBracket,lpcString,@lpcEfunGroup,lpcApplies,lpcFuncName
syn match       lpcErrInBracket     display contained "[);{}]"

" Nodule: Numbers {{{1

" integer number, or floating point number without a dot and with "f".
syn case ignore
syn match       lpcNumbers      display transparent "\<\d\|\.\d" contains=lpcNumber,lpcFloat,lpcOctalError,lpcOctal
" Same, but without octal error (for comments)
syn match       lpcNumbersCom   display contained transparent "\<\d\|\.\d" contains=lpcNumber,lpcFloat,lpcOctal
syn match       lpcNumber       display contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
" hex number
syn match       lpcNumber       display contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match       lpcOctal        display contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=lpcOctalZero
syn match       lpcOctalZero    display contained "\<0"
syn match       lpcFloat        display contained "\d\+f"
" floating point number, with dot, optional exponent
syn match       lpcFloat        display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
" floating point number, starting with a dot, optional exponent
syn match       lpcFloat        display contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
" floating point number, without dot, with exponent
syn match       lpcFloat        display contained "\d\+e[-+]\=\d\+[fl]\=\>"
" flag an octal number with wrong digits
syn match       lpcOctalError   display contained "0\o*[89]\d*"
syn case match

" Nodule: Comment string {{{1

" lpcCommentGroup allows adding matches for special things in comments
syn keyword     lpcTodo                 contained BUG[FIX] TODO FIXME XXX
syn cluster     lpcCommentGroup         contains=lpcTodo

if exists("c_comment_strings")
    " A comment can contain lpcString, lpcCharacter and lpcNumber.
    syn match   lpcCommentSkip          contained "^\s*\*\($\|\s\+\)"
    syn region  lpcCommentString        contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=lpcSpecial,lpcCommentSkip
    syn region  lpcComment2String       contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=lpcSpecial
    syn region  lpcCommentL             start="//" skip="\\$" end="$" keepend contains=@lpcCommentGroup,lpcComment2String,lpcCharacter,lpcNumbersCom,lpcSpaceError
    syn region  lpcComment              matchgroup=lpcCommentStart start="/\*" matchgroup=NONE end="\*/" contains=@lpcCommentGroup,lpcCommentStartError,lpcCommentString,lpcCharacter,lpcNumbersCom,lpcSpaceError
else
    syn region  lpcCommentL             start="//" skip="\\$" end="$" keepend contains=@lpcCommentGroup,lpcSpaceError
    syn region  lpcComment              matchgroup=lpcCommentStart start="/\*" matchgroup=NONE end="\*/" contains=@lpcCommentGroup,lpcCommentStartError,lpcSpaceError
endif
" keep a // comment separately, it terminates a preproc. conditional
syn match       lpcCommentError         display "\*/"
syn match       lpcCommentStartError    display "/\*"me=e-1 contained

" Nodule: Pre-processor {{{1

syn region          lpcPreCondit        start="^\s*#\s*\(if\|ifdef\|ifndef\|elif\)\>" skip="\\$" end="$" end="//"me=s-1 contains=lpcComment,lpcCppString,lpcCharacter,lpcCppParen,lpcParenError,lpcNumbers,lpcCommentError,lpcSpaceError
syn match           lpcPreCondit        display "^\s*#\s*\(else\|endif\)\>"
if !exists("c_no_if0")
    syn region      lpcCppOut           start="^\s*#\s*if\s\+0\+\>" end=".\|$" contains=lpcCppOut2
    syn region      lpcCppOut2          contained start="0" end="^\s*#\s*\(endif\>\|else\>\|elif\>\)" contains=lpcSpaceError,lpcCppSkip
    syn region      lpcCppSkip          contained start="^\s*#\s*\(if\>\|ifdef\>\|ifndef\>\)" skip="\\$" end="^\s*#\s*endif\>" contains=lpcSpaceError,lpcCppSkip
endif
syn region          lpcIncluded         display contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match           lpcIncluded         display contained "<[^>]*>"
syn match           lpcInclude          display "^\s*#\s*include\>\s*["<]" contains=lpcIncluded
syn match           lpcLineSkip          "\\$"
syn cluster         lpcPreProcGroup     contains=@lpcCommentGroup,lpcBracket,lpcComment2String,lpcCommentSkip,lpcCommentStartError
syn cluster         lpcPreProcGroup          add=lpcCommentString,lpcCppOut,lpcCppOut2,lpcCppSkip,lpcDefine,lpcErrInBracket
syn cluster         lpcPreProcGroup          add=lpcErrInParen,lpcFloat,lpcFormat,lpcInclude,lpcIncluded,lpcKeywdError,lpcMulti
syn cluster         lpcPreProcGroup          add=lpcNumber,lpcNumbersCom,lpcOctal,lpcOctalError,lpcOctalZero,lpcParen,lpcPreCondit
syn cluster         lpcPreProcGroup          add=lpcSpecial,lpcString,lpcUserLabel
syn region          lpcDefine           start="^\s*#\s*\(define\|undef\)\>" skip="\\$" end="$" end="//"me=s-1 contains=ALLBUT,@lpcPreProcGroup

" driver dependend {{{2
if b:lpc_driver == "LDMud"
    syn region  lpcPreProc  start="^\s*#\s*pragma\>\)" skip="\\$" end="$" keepend contains=ALLBUT,@lpcPreProcGroup
elseif b:lpc_driver == "MudOS"
    if b:lpc_pre_v22
        syn region  lpcPreProc  start="^\s*#\s*\(pragma\|echo\)\>" skip="\\$" end="$" keepend contains=ALLBUT,@lpcPreProcGroup
    else
        syn region  lpcPreProc  start="^\s*#\s*\(pragma\|echo\|warn\|error\)\>" skip="\\$" end="$" keepend contains=ALLBUT,@lpcPreProcGroup
    endif
elseif b:lpc_driver == "FluffOS"
    syn region  lpcPreProc  start="^\s*#\s*\(pragma\|echo\|warn\|error\|breakpoint\)\>" skip="\\$" end="$" keepend contains=ALLBUT,@lpcPreProcGroup
endif

" Nodule: User labels {{{1

" Highlight Labels
" User labels in LPC is not allowed, only "case x" and "default" is supported
syn cluster     lpcMultiGroup       contains=lpcIncluded,lpcSpecial,lpcCommentSkip,lpcCommentString,lpcComment2String,@lpcCommentGroup,lpcCommentStartError,lpcUserCont,lpcUserLabel,lpcBitField,lpcOctalZero,lpcCppOut,lpcCppOut2,lpcCppSkip,lpcFormat,lpcNumber,lpcFloat,lpcOctal,lpcOctalError,lpcNumbersCom,lpcCppParen,lpcCppBracket,lpcCppString,lpcKeywdError
syn region      lpcMulti            transparent start="\(case\|default\|public\|protected\|private\)" skip="::" end=":" contains=ALLBUT,@lpcMultiGroup

syn cluster     lpcLabelGroup       contains=lpcUserLabel
syn match       lpcUserCont         display "^\s*lpc:$" contains=@lpcLabelGroup

" Don't want to match anything
syn match       lpcUserLabel        display "lpc" contained

" Nodule: Highlight links {{{1

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_lpc_syn_inits")
    if version < 508
        let did_lpc_syn_inits = 1
        command -nargs=+ HiLink hi link <args>
    else
        command -nargs=+ HiLink hi def link <args>
    endif

    HiLink lpcModifier                  lpcStorageClass

    HiLink lpcQuotedFmt                 lpcFormat
    HiLink lpcFormat                    lpcSpecial
    HiLink lpcCppString                 lpcString          " Cpp means
    " C Pre-Processor
    HiLink lpcCommentL                  lpcComment
    HiLink lpcCommentStart              lpcComment
    HiLink lpcUserLabel                 lpcLabel
    HiLink lpcSpecialCharacter          lpcSpecial
    HiLink lpcOctal                     lpcPreProc
    HiLink lpcOctalZero                 lpcSpecial  " LPC will treat octal numbers as decimals, programmers should be aware of that.
    HiLink lpcEfunError                 lpcError
    HiLink lpcKeywdError                lpcError
    HiLink lpcOctalError                lpcError
    HiLink lpcParenError                lpcError
    HiLink lpcErrInParen                lpcError
    HiLink lpcCurlyError                lpcError
    HiLink lpcErrInBracket              lpcError
    HiLink lpcCommentError              lpcError
    HiLink lpcCommentStartError         lpcError
    HiLink lpcSpaceError                lpcError
    HiLink lpcSpecialError              lpcError
    HiLink lpcErrFunc                   lpcError

    if b:lpc_driver == "MudOS"
        if b:lpc_pre_v22
            HiLink lpcOldEfuns          lpc_efuns
            HiLink lpcNewEfuns          lpcError
        else
            HiLink lpcOldEfuns          lpcReserved
            HiLink lpcNewEfuns          lpc_efuns
        endif
    elseif b:lpc_driver == "FluffOS"
        HiLink lpcOldEfuns              lpcError
    endif
    HiLink lpc_efuns                    lpcFunction
    HiLink lpc_sefuns                   lpcFunction

    HiLink lpcReserved                  lpcPreProc
    HiLink lpcTextString                lpcString   " This should be preprocessors, but
    HiLink lpcArrayString               lpcPreProc  " let's make some difference between text and array

    HiLink lpcIncluded                  lpcString
    HiLink lpcCommentString             lpcString
    HiLink lpcComment2String            lpcString
    HiLink lpcCommentSkip               lpcComment
    HiLink lpcCommentFunc               lpcComment

    HiLink lpcCppSkip                   lpcCppOut
    HiLink lpcCppOut2                   lpcCppOut
    HiLink lpcCppOut                    lpcComment

    " Standard type below
    HiLink lpcApplies                   Special
    HiLink lpcCharacter                 Character
    HiLink lpcComment                   Comment
    HiLink lpcConditional               Conditional
    HiLink lpcConstant                  Constant
    HiLink lpcDefine                    Macro
    HiLink lpcError                     Error
    HiLink lpcFloat                     Float
    HiLink lpcFunction                  Function
    HiLink lpcIdentifier                Identifier
    HiLink lpcInclude                   Include
    HiLink lpcLabel                     Label
    HiLink lpcNumber                    Number
    HiLink lpcOperator                  Operator
    HiLink lpcPreCondit                 PreCondit
    HiLink lpcPreProc                   PreProc
    HiLink lpcRepeat                    Repeat
    HiLink lpcStatement                 Statement
    HiLink lpcStorageClass              StorageClass
    HiLink lpcString                    String
    HiLink lpcStructure                 Structure
    HiLink lpcSpecial                   LineNr
    HiLink lpcTodo                      Todo
    HiLink lpcType                      Type

    delcommand HiLink
endif

" Nodule: This is the end nodule. {{{1

let b:current_syntax = "lpc"

let &cpo = s:cpo_save
unlet s:cpo_save

" vim:ts=8:nosta:sw=4:ai:si:
" vim600:set fdm=marker: }}}1
