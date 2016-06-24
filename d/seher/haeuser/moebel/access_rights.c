int access_rights(string user, string file) {
  if(user == "swift" && file!="autoloadertruhe.c")
    return 1;  // Swift hat auf die Moebel Zugriff.
  return 0;
}
