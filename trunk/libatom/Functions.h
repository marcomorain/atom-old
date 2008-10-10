class Runtime;

Cell* function_setf			( Runtime& runtime, Cell* params );
Cell* function_car			( Runtime& runtime, Cell* params );
Cell* function_cdr			( Runtime& runtime, Cell* params );
Cell* function_stringp		( Runtime& runtime, Cell* params );
Cell* function_if			( Runtime& runtime, Cell* params );
Cell* function_plus			( Runtime& runtime, Cell* params );
Cell* function_minus		( Runtime& runtime, Cell* params );

// Function: QUOTE Value
// Return Value without evaluating it. 
Cell* function_quote		( Runtime& runtime, Cell* params );

// Return T if its arguments are in strictly decreasing order, NIL otherwise. 
Cell* function_less_than	( Runtime& runtime, Cell* params );

// Return T if its arguments are in strictly increasing order, NIL otherwise. 
Cell* function_greater_than ( Runtime& runtime,	Cell* params );

Cell* function_load			( Runtime& runtime,	Cell* params );
Cell* function_defun		( Runtime& runtime, Cell* params );