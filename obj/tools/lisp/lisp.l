(write "LPC closure parser ready...\n")
(write "LLisp Version 0.6 [")
;;;
;;; automagic quoting of hardcoded functions:
;;; each function has 2 entries in 'memory'
;;; 0 -- the function name or equivalent
;;; 1 -- bits regarding autoquoting
;;;      each argument for the function has two bits 
;;;      0 -- normal quoting
;;;      1 -- program code quoting (lamda requires it)
;;;      only bit 0 or bit 1 must be set, never both (no quoting at all)
;;; lambda needs normal quote for the first argument and special quote for 2nd
;;; 1 0 0 1
;;; |1| |2| -> 9 (if i have calculated it right)
;;;
;;; the function lambda has autoquote, @ is a non-autoquote version
(= ([ (memory) '@) lambda)
(= ([ (memory) lambda 1) 9) 
;;;
;;; explicit quote of a symbol with quote
(= ([ (memory) quote 1) 1) 
;;;
;;; one of the most important functions needed (microcoded):
;;; now we define a set
(= ([ (memory) 'set) (lambda (n v) (, (= ([ (memory) n) v) n))) 
;;; and a autoquote version of set (setq)
(set 'setq set) (= ([ (memory) 'setq 1) 1)
;;;
;;; some people prefer define, it quotomatically quotes its first argument
(setq define (lambda (n v f) (, (= ([ (memory) n 1) f) (set n v) n)))
(= ([ (memory) 'define 1) 1)
;;;
;;; defun for easier definition of functions
(define defun (lambda (n a b f) (, (= ([ (memory) n 1) f) 
                                   (set n (@ a b)) n)) 37)
;;;
;;; if we like to destroy a value use undef
(defun undef (fun) (, (= memory (m_delete (memory) fun)) fun) 1)
;;;
;;; an error function (internal use), notify declares the function to use
;;; when an error message has to be printed
(setq error raise_error)
;;;
;;; other important functions and function aliases
;;; standard lisp functionality (car, cdr, cons)
(defun car (l) (? (pointerp l) 
		  (? (sizeof l) ([ l 0) 
		     (error "car: empty list\n")) 
		  (error "car: not a list\n")))
(defun cdr (l) (? (pointerp l) ([.. l 1) 
		  (error "cdr: not a list\n")))
(defun cons (_car _cdr) (+ (list _car) _cdr))
;;;
;;; some special functionality
(defun addhistory (file) (write_file file (implode (history) "\n")))
(defun savehistory (file) (, (rm file) (addhistory file)))
(defun showhistory () (, (printf "%s\n" (implode (history) "\n"))
			 "-- END OF HISTORY --"))
;;;
(defun showfunc () (m_indices (memory)))
;;;
;;; Try to autodetect mudtype
;;;
(set 'mudtype "")
(defun add (var val) (+= ([ (memory) var) val))
(add 'mudtype (? (symbol_function "getuid") "NATIVE" "COMPAT"))
(add 'mudtype (? (function_exists "SetProp")
                   (? (== (function_exists "Set") "/std/thing/properties") "|MG"
                    "|NF")
                 (&& (== (file_size "/basic") -2) 
		     (== (file_size "/complex") -2))
                   (? (== (file_size "/kernel") -2) "|TUBNEW" "|TUB")
                 (== (function_exists "query_all_v_items") "/i/item/virtual")
                   (? (function_exists "query_bodylocation_pic"
		                       (, (call_other "/obj/player" "?")
				          (find_object "/obj/player")))
				       "|UNI/AVALON" "|UNI")
	        (? (function_exists "RCSId" (find_object "/obj/simul_efun")))
		 "|TAPP"
                 "|UNKNOWN/2.4.5"
               ))
(setq mudtype (explode mudtype "|"))
(defun mud? (str) (sizeof (regexp ([ (memory) 'mudtype) str)))
(printf "%s,%s" ([ ([ (memory) 'mudtype) 0) ([ ([ (memory) 'mudtype) 1))
(write "]\n")
;;; 
;;; load local user init
;;;
(? (mud? "NF|MG")
   (setq owner (getuid (this_player)))
   (setq owner (call_other (this_player) "query_real_name")))
(? (mud? "MG|NF|TUB")
   (define localinit (+ (+ "/players/" owner) "/lisp.l"))
   (define localinit (+ (+ "/w/" owner) "/lisp.l")))
(? (> (file_size localinit) 0)
   (? (mud? "TUB|UNI|UNKNOWN") 
      (call_out load 0 localinit)
      (load localinit)))
(printf "Welcome %s, today is %s!\n" (capitalize owner) (ctime (time)))
