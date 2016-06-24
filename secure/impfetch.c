// Imp Fetcher

#define FTPD "secure/ftpd"
#define FTPIMP "std/shells/filesys/ftpimp"

#define D(msg) if(find_player("rumata"))tell_object(find_player("rumata"),msg)

void create() {}

nomask mixed impFor( string name ) {
    object imp;
    string fname;

    fname = "/ftpimp:" + name;
    imp = find_object( fname );
    if( !objectp(imp) ) {
	seteuid( name );
	imp = clone_object( FTPIMP );
	imp->SetUser( name );
	rename_object( imp, fname );
        destruct( this_object() ); // next imp must have rootid again
    }
    return imp;
}
