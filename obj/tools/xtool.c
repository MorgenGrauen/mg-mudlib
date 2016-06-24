void create()
{
  if(member(object_name(this_object()),'#')==-1)
    return;
  clone_object("/obj/tools/MGtool")->move(this_object());
  call_out(lambda(({}),({#'destruct,({#'this_object})})),0);
}
