     int fun() {
         printf("%O [c] fun()\n", this_object());
         find_object("/doc/beispiele/shadow/aa")->fun();
     }

     void fun3() {
         printf("%O [c] fun3()\n", this_object());
     }

     void do_shadow(object target) { shadow(target, 1); }

