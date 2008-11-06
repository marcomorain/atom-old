;; Copyright (c) 1997-2001 Sandro Sigala.  All rights reserved.
;; 
;; Redistribution and use in source and binary forms, with or without
;; modification, are permitted provided that the following conditions
;; are met:
;; 1. Redistributions of source code must retain the above copyright
;; notice, this list of conditions and the following disclaimer.
;; 2. Redistributions in binary form must reproduce the above copyright
;; notice, this list of conditions and the following disclaimer in the
;;    documentation and/or other materials provided with the distribution.
;; 
;; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
;; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
;; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
;; IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
;; INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
;; NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
;; THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;;
;; Simple test routines for the SLisp interpreter.
;; Lisp routines for the SLisp interpreter.
;;
;; Portions copyright (c) 1988, 1990 Roger Rohrbach

(defun assert (exp)
  (or (eval exp)
      (princ "assertion failed: " exp " (:= " (eval exp) ")\n")))

(defun asserteq (exp1 exp2)
  (or (eq (eval exp1) (eval exp2))
      (princ "assertion failed: " exp1 " (:= " (eval exp1) ") == "
	     exp2 " (:= " (eval exp2) ")\n")))

(defun assertn (exp)
  (assert (list 'not exp)))

(assert t)
(assertn nil)
(asserteq '() nil)

;; `car', `cdr' test
(assertn '(car '()))
(assertn '(cdr '()))
(asserteq '(car '(foo bar baz)) ''foo)
(asserteq '(car (cdr '(foo bar baz))) ''bar)
(asserteq '(car (cdr '(foo bar baz))) ''bar)
(asserteq '(car (cdr (cdr '(foo bar baz)))) ''baz)
(asserteq '(car (cdr (cdr (cdr '(foo bar baz))))) nil)

;; `atom' test
(assert '(atom t))
(assert '(atom nil))
(assert '(atom '()))
(assertn '(atom '(foo)))
(assertn '(atom '(foo bar baz)))

;; `cons', `list' test
(asserteq '(car (cons 'foo nil)) ''foo)
(asserteq '(cdr (cons 'foo nil)) nil)
(asserteq '(car (cdr (cons 'foo nil))) nil)
;(asserteq '(car (cdr (cons 'foo (cons 'bar)))) ''bar)
;(asserteq '(car (cons 'foo (cons 'bar))) ''foo) ; seems to be a broken test?
;(asserteq '(cdr (cdr (cons 'foo (cons 'bar)))) nil)
(asserteq '(car (list 'foo)) ''foo)
(asserteq '(cdr (list 'foo)) nil)
(asserteq '(car (cdr (list 'foo))) nil)
(asserteq '(car (cdr (list 'nil 'bar))) ''bar)
(asserteq '(car (list 'foo 'bar)) ''foo)
(asserteq '(cdr (cdr (list 'foo 'bar))) nil)

;; `and', `or', `not' test
(assert '(and t))
(assertn '(and nil))
(assert '(and t t))
(assertn '(and nil nil))
(assertn '(and t nil))
(assertn '(and nil t))
(assert '(or t))
(assertn '(or nil))
(assert '(or t t))
(assertn '(or nil nil))
(assert '(or t nil))
(assert '(or nil t))
(assertn '(not t))
(assert '(not nil))
(assertn '(not 'foo))

;; `if' test
(assertn '(if t nil t t))
(assert '(if nil nil t t))

(assertn '(while nil t))

;; `cond' test
(assertn '(cond (nil t) (t nil)))
(assertn '(cond (nil t) (nil t)))
(assert '(cond (nil nil) (t t)))
(assert '(cond (nil) (t)))
(assertn '(cond (nil) (nil)))
(asserteq '(cond (nil 'foo) (nil 'bar)) nil)
(asserteq '(cond (nil 'foo) (t 'bar)) ''bar)
(asserteq '(cond (t 'foo) (t 'bar)) ''foo)
(asserteq '(cond (nil 'foo) (t 'bar 'baz)) ''baz)

;; `progn' test
(assertn '(progn t nil nil))
(assert '(progn t nil nil t))
(asserteq '(progn t nil nil 'foo) ''foo)
(asserteq '(progn t nil nil 'foo 'bar) ''bar)
(assert '(prog1 t nil nil))
(assertn '(prog2 t nil t t))

;; Recursion test
(defun last* (l)
  (cond ((eq (cdr l) nil) (car l))
	(t (last* (cdr l)))))

(asserteq '(last* '(foo bar baz)) ''baz)
(asserteq '(last* '(foo bar)) ''bar)
(asserteq '(last* '(foo)) ''foo)

;; `cond*' test
(progn (assertn '(cond* '((nil t) (t nil))))
	(assertn '(cond* '((nil t) (nil t))))
	(assert '(cond* '((nil nil) (t t))))
	(assert '(cond* '((nil) (t))))
	(assertn '(cond* '((nil) (nil))))
	(asserteq '(cond* '((nil 'foo) (nil 'bar))) nil)
	(asserteq '(cond* '((nil 'foo) (t 'bar))) ''bar)
	(asserteq '(cond* '((t 'foo) (t 'bar))) ''foo)
	(asserteq '(cond* '((nil 'foo) (t 'bar 'baz))) ''baz)))

;; `progn*' test
(progn (assertn '(progn* '(t nil nil)))
	(assert '(progn* '(t nil nil t)))
	(asserteq '(progn* '(t nil nil 'foo)) ''foo)
	(asserteq '(progn* '(t nil nil 'foo 'bar)) ''bar)))
