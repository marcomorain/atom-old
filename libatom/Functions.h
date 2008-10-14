class Runtime;

Cell* function_apply		( Runtime& runtime, Cell* params );
// aref
// backquote
// block
Cell* function_car			( Runtime& runtime, Cell* params );
Cell* function_cdr			( Runtime& runtime, Cell* params );
// ceiling
// char=
Cell* function_cons			( Runtime& runtime, Cell* params );
// defmacro
// documentation
// eq
// error
// expt
// fdefinition
// function
// floor
// gensym
// get-setf-expression
Cell* function_if			( Runtime& runtime, Cell* params );
// imagpart
// labels
Cell* function_length		( Runtime& runtime, Cell* params );
// multiple-value-bind
// nth-value
Cell* function_quote		( Runtime& runtime, Cell* params );
// realpart
// symbol-function
// tagbody
// type-of
// typep
// =
Cell* function_plus			( Runtime& runtime, Cell* params );
Cell* function_minus		( Runtime& runtime, Cell* params );
Cell* function_greater_than ( Runtime& runtime,	Cell* params );
Cell* function_less_than	( Runtime& runtime, Cell* params );

// may not be needed:
Cell* function_load			( Runtime& runtime,	Cell* params );
Cell* function_setf			( Runtime& runtime, Cell* params );
Cell* function_stringp		( Runtime& runtime, Cell* params );

