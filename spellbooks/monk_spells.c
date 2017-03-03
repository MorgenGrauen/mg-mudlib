/* Klerus-Spellbooks: Platzhalter

   Das Klerus-Spellbook steht als git-Repository bereit und kann mit

     ssh-agent bash -c
       'ssh-add ~/.ssh/id_morgengrauen_rsa;
        git clone ssh://Gloinson@mg.mud.de:29418/spellbooks/klerus'
     ('Gloinson' und 'id_morgengrauen_rsa' mit eurem User/Keyfile ersetzen)

   bzw nach entsprechendem Setup eurer ~/.ssh/config (siehe MUD-Doku)

     git clone ssh://mgg/spellbooks/klerus

   ausgecheckt werden.

   1. Sep 2011 Gloinson
 */
inherit "/spellbooks/klerus/monk_spells.c";

protected void create() {
  replace_program("/spellbooks/klerus/monk_spells.c");
  ::create();
}
