     int fun() {
         printf("%O [b] fun()\n", this_object());
         find_object("/doc/beispiele/shadow/aa")->fun();
     }

     void fun2() {
         printf("%O [b] fun2()\n", this_object());
         find_object("/doc/beispiele/shadow/aa")->fun3();
         this_object()->fun3();
     }

     void do_shadow(object target) { shadow(target, 1); }

