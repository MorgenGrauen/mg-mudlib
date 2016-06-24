static efun_regexp( arg1, arg2 )
{
	return regexp( arg1, arg2 );
}

static efun_all_inventory( arg )
{
	return all_inventory( arg );
}

static efun_break_string( arg1, arg2, arg3 )
{
	return break_string( arg1, arg2, arg3 );
}

static efun_capitalize( arg )
{
	return capitalize( arg );
}

static efun_cat( arg1, arg2, arg3 )
{
	return cat( arg1, arg2, arg3 );
}

static efun_clear_bit( arg1, arg2 )
{
	return clear_bit( arg1, arg2 );
}

static efun_crypt( arg1, arg2 )
{
	return crypt( arg1, arg2 );
}

static efun_ctime( arg )
{
	return ctime( arg );
}

static efun_debug_info( arg1, arg2 )
{
	return debug_info( arg1, arg2 );
}

static efun_deep_inventory( arg )
{
	return deep_inventory( arg );
}

static efun_environment( arg )
{
	return environment( arg );
}

static efun_explode( arg1, arg2 )
{
	return old_explode( arg1, arg2 );
}

static efun_file_name( arg )
{
	return object_name( arg );
}

static efun_file_size( arg )
{
	return file_size( arg );
}

static efun_filter_array( arg1, arg2, arg3, arg4 )
{
	return filter( arg1, arg2, arg3, arg4 );
}

static efun_find_call_out( arg )
{
	return find_call_out( arg );
}

static efun_find_living( arg )
{
	return find_living( arg );
}

static efun_find_object( arg )
{
	return find_object( arg );
}

static efun_find_player( arg )
{
	return find_player( arg );
}

static efun_function_exists( arg1, arg2 )
{
	return function_exists( arg1, arg2 );
}

static efun_implode( arg1, arg2 )
{
	return implode( arg1, arg2 );
}

static efun_interactive( arg )
{
	return interactive( arg );
}

static efun_intp( arg )
{
	return intp( arg );
}

static efun_last_reboot_time()
{
	return last_reboot_time();
}

static efun_living( arg )
{
	return living( arg );
}

static efun_lower_case( arg )
{
	return lower_case( arg );
}

static efun_get_dir( arg1, arg2 )
{
	return get_dir( arg1, arg2 );
}

static efun_map_array( arg1, arg2, arg3, arg4 )
{
	return map( arg1, arg2, arg3, arg4 );
}

static efun_member( arg1, arg2 )
{
	return member( arg1, arg2 );
}

static efun_objectp( arg )
{
	return objectp( arg );
}

static efun_pointerp( arg )
{
	return pointerp( arg );
}

static efun_present( arg1, arg2 )
{
	return present( arg1, arg2 );
}

static efun_process_string( arg )
{
	return process_string( arg );
}

static efun_query_actions( arg1, arg2 )
{
	return query_actions( arg1, arg2 );
}

static efun_query_idle( arg )
{
	return query_idle( arg );
}

static efun_query_ip_name( arg )
{
	return query_ip_name( arg );
}

static efun_query_ip_number( arg )
{
	return query_ip_number( arg );
}

static efun_query_load_average()
{
	return query_load_average();
}

static efun_query_snoop( arg )
{
	return query_snoop( arg );
}

static efun_random( arg )
{
	return random( arg );
}

static efun_read_bytes( arg1, arg2, arg3 )
{
	return read_bytes( arg1, arg2, arg3 );
}

static efun_read_file( arg1, arg2, arg3 )
{
	return read_file( arg1, arg2, arg3 );
}

static efun_remove_call_out( arg )
{
	return remove_call_out( arg );
}

static efun_remove_interactive( arg )
{
	return remove_interactive( arg );
}

static efun_rusage()
{
	return rusage();
}

static efun_say( arg1, arg2 )
{
	if( !arg2 ) return say( arg1 );
	return say( arg1, arg2 );
}

static efun_set_bit( arg1, arg2 )
{
	return set_bit( arg1, arg2 );
}

static efun_set_heart_beat( arg )
{
	return set_heart_beat( arg );
}

static efun_set_living_name( arg )
{
	return set_living_name( arg );
}

static efun_sizeof( arg )
{
	return sizeof( arg );
}

static efun_sort_array( arg1, arg2, arg3 )
{
	return sort_array( arg1, arg2, arg3 );
}

static efun_stringp( arg )
{
	return stringp( arg );
}

static efun_strlen( arg )
{
	return sizeof( arg );
}

static efun_tail( arg )
{
	return tail( arg );
}

static efun_shout( arg )
{
	return shout( arg );
}

static efun_tell_object( arg1, arg2 )
{
	return tell_object( arg1, arg2 );
}

static efun_tell_room( arg1, arg2, arg3 )
{
	return tell_room( arg1, arg2, arg3 );
}

static efun_test_bit( arg1, arg2 )
{
	return test_bit( arg1, arg2 );
}

static efun_time()
{
	return time();
}

static efun_unique_array( arg1, arg2, arg3 )
{
	return unique_array( arg1, arg2, arg3 );
}

static efun_users()
{
	return users();
}

static efun_version()
{
	return version();
}

static efun_write( arg )
{
	return write( arg );
}

static efun_write_bytes( arg1, arg2, arg3 )
{
	return write_bytes( arg1, arg2, arg3 );
}

static efun_write_file( arg1, arg2 )
{
	return write_file( arg1, arg2 );
}

static efun_geteuid( arg )
{
	return geteuid( arg );
}

static efun_getuid( arg )
{
	return getuid( arg );
}

static efun_first_inventory( arg )
{
	return first_inventory( arg );
}

static efun_next_inventory( arg )
{
	return next_inventory( arg );
}

static efun_inherit_list( arg )
{
	return inherit_list( arg );
}

static efun_strstr( arg1, arg2, arg3 )
{
	return strstr( arg1, arg2, arg3 );
}

static efun_program_time( arg )
{
	return program_time( arg );
}

static efun_get_error_file( arg1, arg2 )
{
	return get_error_file( arg1, arg2 );
}

static efun_set_prompt( arg1, arg2 )
{
	return set_prompt( arg1, arg2 );
}

static efun_filter_objects( arg1, arg2, arg3 )
{
	return filter_objects( arg1, arg2, arg3 );
}

static efun_map_objects( arg1, arg2, arg3 )
{
	return map_objects( arg1, arg2, arg3 );
}

static efun_transpose_array( arg )
{
	return transpose_array( arg );
}

static efun_query_once_interactive( arg )
{
	return query_once_interactive( arg );
}

static efun_wizlist( arg )
{
	wizlist( arg );
	return 1;
}

static efun_mkdir( str )
{
	return mkdir( str );
}

static efun_rm( str )
{
	return rm( str );
}

static efun_country( str )
{
	return country( str );
}

static efun_dtime( val )
{
	return dtime( val );
}
