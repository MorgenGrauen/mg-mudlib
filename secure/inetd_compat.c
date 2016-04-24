#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow

// portability - lfuns implementing efuns from LDMud
protected string _json_serialize(mixed v)
{
  return json_serialize(v);
}

protected mixed _json_parse(string s)
{
  return json_parse(s);
}


