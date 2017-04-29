int access_rights(string user, string file) {
  if(user == "notstrom" && file == "aquarium.c")
    return 1;
  return 0;
}
