(defun third (x)
	(car (cdr (cdr x))))
	
(defun second (x)
	(car (cdr x)))

(defun abs (n)
	(if (typep n 'complex)
		(sqrt (+ (expt (realpart n) 2) (expt (imagpart n) 2)))
		(if (< n 0) (- n) n)))

(defun adjoin (obj lst &rest args)
	(if (apply #'member obj lst args) list (cons obj lst)))
	
(defun atomp (x)
	(not (consp x)))