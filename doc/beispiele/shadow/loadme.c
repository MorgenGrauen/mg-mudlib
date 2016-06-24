void create() {
     object a, b, c;

     if (find_object("/doc/beispiele/shadow/aa"))
       destruct(find_object("/doc/beispiele/shadow/aa"));
     a = load_object("/doc/beispiele/shadow/aa");
     if (find_object("/doc/beispiele/shadow/bb"))
       destruct(find_object("/doc/beispiele/shadow/bb"));
     b = load_object("/doc/beispiele/shadow/bb");
     if (find_object("/doc/beispiele/shadow/cc"))
       destruct(find_object("/doc/beispiele/shadow/cc"));
     c = load_object("/doc/beispiele/shadow/cc");

     b->do_shadow(a);
     c->do_shadow(a);
     printf("--- a->fun() ---\n");
     a->fun();
     printf("--- b->fun() ---\n");
     b->fun();
     printf("--- c->fun() ---\n");
     c->fun();
     printf("--- b->fun2() ---\n");
     b->fun2();
}
