/* Klerusgilde: Platzhalter

   Die Klerusgilde steht als git-Repository bereit und kann mit

     ssh-agent bash -c
       'ssh-add ~/.ssh/id_morgengrauen_rsa;
        git clone ssh://Gloinson@mg.mud.de:29418/gilden/files.klerus'
     ('Gloinson' und 'id_morgengrauen_rsa' mit eurem User/Keyfile ersetzen)

   bzw nach entsprechendem Setup eurer ~/.ssh/config (siehe MUD-Doku)

     git clone ssh://mgg/gilden/files.klerus

   ausgecheckt werden.

   31. Aug 2011 Gloinson
 */
inherit "/gilden/files.klerus/klerus.c";

protected void create() {
  replace_program("/gilden/files.klerus/klerus.c");
  ::create();
}
