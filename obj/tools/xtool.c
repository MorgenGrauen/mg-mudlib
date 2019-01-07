void create()
{
  if(member(object_name(this_object()),'#')==-1)
    return;
  clone_object("/obj/tools/MGtool")->move(this_object());
  call_out(#'destruct, 0, this_object());
}
