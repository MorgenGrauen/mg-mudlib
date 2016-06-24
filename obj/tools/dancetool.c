// DANCETOOL.C   Stand: 10.12.94 Dancer 
//
// Kleines Tool zum Schreiben und Nachsehen von Details.
//
//
 
inherit "std/thing";
 
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
 
#define bs( x ) break_string( x, 78 )
 
private int i, counter, first_detail_line, startline, endline; 
private string *detail_shorts, *newstring, *file_lines, *new_lines;
private string detail_long, save_string, old_file, new_file, actual_file,
               detail_text;
       
void create()
{
   ::create();
   SetProp( P_AUTOLOADOBJ,1 );
   SetProp( P_NAME, "Dancers Sandtigerkralle" );
   SetProp( P_GENDER, FEMALE );
   SetProp( P_SHORT, "Dancers Sandtigerkralle" );
   SetProp( P_LONG, bs( "Die Sandtigerkralle von Dancer ist "+
              "ein kleines Tool, mit dem Du gewoehnliche Details "+
              "in allen Raeumen nachschauen und in Deinen eigenen "+
              "hinzufuegen kannst. Weitere Informationen kannst Du "+
              "mit dem Befehl <dancetool> bekommen." ) );
   SetProp( P_ARTICLE, 2 );
   SetProp( P_WEIGHT, 500 );
   SetProp( P_VALUE, 10000 );
   SetProp( P_NOBUY, 1 );
   SetProp( P_NODROP, 1 );
   AddId( "kralle" );
   AddId( "sandtigerkralle" );
   AddId( "tigerkralle" );
   AddId( "dancetool" );
   AddId( "dancertool" );
   seteuid(getuid(this_object()));
}
 
 
void init()
{
  if (!IS_LEARNER(this_player())) return;
  ::init();
  add_action("query_add_details", "show_detail" );
  add_action("query_add_details", "show_details" );
  add_action("query_add_details", "sd" );
  add_action("set_add_detail", "add_detail" );
  add_action("set_add_detail", "add_details" );
  add_action("set_add_detail", "ad" );
  add_action("give_info", "dancetool" );
}
 
 
int give_info()
{
   write( "-----------------------------------------------------------------------------\n" );
   write( "Die Sandtigerkralle ermoeglicht folgende Befehle:\n" );
   write( "\nadd_detail, add_details oder ad:\n" );
   write( bs( "Ruft einen kleinen AddDetail-Editor auf, mit dem Du "+
      "Details in Deine eigenen Raeume einfuegen kannst. Als Argument "+
      "kann hinter den Befehl der Filename eines Raumes eingegeben "+
      "werden. Wird kein Argument eingegeben, so wird der Raum "+
      "genommen, in dem Du gerade stehst. Achtung: Es koennen "+
      "nur Details in Raeume eingefuegt haben, die bereits ein "+
      "AddDetail in ihrem create() haben. Du musst die Lupe haben, "+
      "da der Raum nach dem Einfuegen des Details automatisch "+
      "neu geladen wird." ) );
   write( "\nshow_detail, show_details oder sd:\n" );
   write( bs( "Zeigt Dir alle Zeilen des angesprochenen Raumes bzw. "+
      "des Raumes in dem Du stehst, die ein AddDetail "+
      "beinhalten. So kannst Du kontrollieren, welche Details "+
      "Du schon eingebaut hast." ) );
   write( "-----------------------------------------------------------------------------\n" );
   return 1;
}
 
 
int renew_variables()
{
   i = counter = first_detail_line = startline = endline = 0; 
   detail_shorts = newstring = file_lines = new_lines = ({});
   detail_long = save_string = old_file = new_file = actual_file = detail_text = 0;
   return 1;
} 
 
 
 
int query_add_details( string str )
{
   string old_file, *file_lines, new_file, *new_lines, actual_file;        
   int i, counter, first_detail_line, startline, endline;      
 
   actual_file = "";
                            
   if ( !str ) actual_file = object_name( environment( this_player() ) )+".c";
   else       
   {
      if ( str[0..0] == "/" )
      {
         if ( str[<2..] != ".c" )
             actual_file = str+".c";
         else
             actual_file = str;
      }
      else
      {
         if ( str[<2..] != ".c" )
             actual_file = this_player()->QueryProp( P_CURRENTDIR )+
                           "/"+str+".c";
         else
             actual_file = this_player()->QueryProp( P_CURRENTDIR )+
                           "/"+str;
      }
   }
   if ( file_size( actual_file )==-1 )
   {
     write( "File: "+actual_file+" nicht gefunden!\n" );
     return 1;
   }
   call_other( actual_file, "???" );
   write( "\n-----------------------------------------------------------------------------\n" );
   write( "Aktuelles File:   "+actual_file+"\n" );
   write( "Filegroesse:      "+file_size( actual_file )+"\n" );
   write( "Userid des Files: "+getuid( find_object( actual_file ) )+"\n" );
   new_file = "";
   counter = 0;
   first_detail_line = 0;
   old_file = read_file( actual_file );
   file_lines = ({ "dummy" }) + old_explode( old_file, "\n" );          
   startline = 1;
   endline = sizeof( file_lines )-1;        
   for( i = startline; i <= endline; i++ )
   {                                                        
      new_file = new_file+file_lines[ i ]+"\n";
      if ( strstr( file_lines[i], "AddDetail" ) != -1 )
      {
         write( file_lines[i]+"\n" );
         if ( !first_detail_line ) first_detail_line = i;
          counter = counter + 1;
      }
   }                                
   write( "Details found: "+counter+".\n" ); 
   write( "-----------------------------------------------------------------------------\n" );
   return 1;
}    
 
 
int set_add_detail( string str )
{
 
   renew_variables();
                            
   if ( !str ) actual_file = object_name( environment( this_player() ) )+".c";
   else       
   {
      if ( str[0..0] == "/" )
      {
         if ( str[<2..] != ".c" )
             actual_file = str+".c";
         else
             actual_file = str;
      }
      else
      {
         if ( str[<2..] != ".c" )
             actual_file = this_player()->QueryProp( P_CURRENTDIR )+
                           "/"+str+".c";
         else
             actual_file = this_player()->QueryProp( P_CURRENTDIR )+
                           "/"+str;
      }
   }
   if ( file_size( actual_file )==-1 )
   {
     write( "File: "+actual_file+" nicht gefunden!\n" );
     return 1;
   }
   call_other( actual_file, "???" );
   write( "\n-----------------------------------------------------------------------------\n" );
   write( "Aktuelles File:   "+actual_file+"\n" );
   write( "Filegroesse:      "+file_size( actual_file )+"\n" );
   write( "Userid des Files: "+getuid( find_object( actual_file ) )+"\n" );
   new_file = "";
   new_lines = ({ "dummy" });
   counter = 0;
   first_detail_line = 0;
   old_file = read_file( actual_file );
   file_lines = ({ "dummy" }) + old_explode( old_file, "\n" );          
   startline = 1;
   endline = sizeof( file_lines )-1;        
   for( i = startline; i <= endline; i++ )
   {                                                        
      new_file = new_file+file_lines[ i ]+"\n";
      if ( strstr( file_lines[i], "AddDetail" ) != -1 )
      {
        if ( !first_detail_line ) first_detail_line = i;
        counter = counter + 1;
      }
   }
   if ( !first_detail_line )
   {
      write( bs( "Es wurden im File "+actual_file+" keine "+
         "Details gefunden! Um Details hinzufuegen zu koennen, "+
         "muss mindestens ein AddDetail bereits vorhanden sein!" ) );
      write( "-----------------------------------------------------------------------------\n" );
      return 1;
   }
   write( "Details found:    "+counter+".\n" );
   write( "-----------------------------------------------------------------------------\n" ); 
   write( "Bitte die Detail-Kuerzel eingeben:\n(Bemerkung: "+
          "die Kuerzel muessen mit Leerzeichen getrennt sein, "+
          "Abbruch mit 'q'.)\n->" );
   input_to( "get_detail_short" );
   return 1;
}    
 
 
int get_detail_short( string str )
{
   string dummy;
   if ( str == "q" )
   {
     tell_object( this_player(), "Abgebrochen!\n" );
     return 1;
   }
   detail_shorts = map( old_explode( str, " " ),#'lower_case);
   dummy = break_string(implode(sort_array(map(detail_shorts,#'lower_case),#'>),", ")+":",78);
   write( "-----------------------------------------------------------------------------\n" );
   write( bs ( "Bitte Beschreibung eingeben fuer das Detail mit den Namen \n" ) );
   write( dummy+"( Abbruch mit 'q'.)\n->" );
   input_to( "get_detail_long" );
   return 1;
}
 
 
int get_detail_long( string str )
{
   if ( str == "q" )
   {
     write( "Abgebrochen!\n" );
     write( "-----------------------------------------------------------------------------\n" );
     return 1;
   }
   detail_long = break_string( "break_string( \""+str, 60 ); 
   save_string = "  AddDetail( ({ ";        
   for( i = 0; i < sizeof( detail_shorts ); i ++ )
   {
      if ( !(i%3) && i )
        save_string = save_string + "\n                ";
      if ( i < sizeof( detail_shorts ) - 1 ) 
         detail_shorts[ i ] = "\""+detail_shorts[ i ]+"\", ";
      else
         detail_shorts[ i ] = "\""+detail_shorts[ i ]+"\" ";
      save_string = save_string + detail_shorts[ i ];      
      write( detail_shorts[ i ]+"\n" );
   }
   save_string = save_string + "}), \n";
   newstring = old_explode( detail_long,"\n");
   i = 0; 
   for( i = 0; i<sizeof(newstring); i++) 
   {
      if ( i < sizeof(newstring)-1 )
      {
         if ( i==0||!i )
          save_string = save_string +"                "+newstring[i]+" \"+\n";
         else
          save_string = save_string +"                \""+ newstring[i]+" \"+\n";
 
      }
      else
      {
         if ( i==0||!i )
          save_string = save_string +"                "+ newstring[i]+"\", 78 ) );";
         else
          save_string = save_string +"                \""+ newstring[i]+"\", 78 ) );";
      }
   }
   new_lines += ({ "dummy" });
   for ( i = 1; i < first_detail_line; i++ )
   {
     new_lines += ({ file_lines[i] });
   }                                  
   new_lines += ({ save_string });
   for ( i = first_detail_line + 1; i <= endline+1; i++ )
     new_lines += ({ file_lines[ i - 1 ] });
   write( "-----------------------------------------------------------------------------\n" );
   write( bs( "Folgende Zeilen werden in die Datei "+actual_file+" geschrieben, falls `j` eingegeben wird:\n" ) );
   write( save_string+"\n" );
   write( "\nDetail speichern? (j/n)\n->" );
   input_to( "save_detail" );
   return 1;
}
 
 
int save_detail( string str )
{  
   if ( str != "j" && str != "ja" && str != "Ja" && str != "J" )
   {
     write( "Abgebrochen!\n" );
     write( "-----------------------------------------------------------------------------\n" );
     return 1;
   }   
   if ( !rm( actual_file ) )
   {
      write( "File: "+actual_file+" wurde nicht gefunden!\n" );
      return 1;
   }                   
   for ( i = 2; i <= endline+2; i++ )
     write_file( actual_file, new_lines[i]+"\n" ); 
     //write( new_lines[i]+"\n" ); 
   write( "Das Detail wurde in die Datei geschrieben.\n" );
   this_player()->command_me( "here make" );
   write( "-----------------------------------------------------------------------------\n" );  
   return 1;
}
