''	FreeBASIC - 32-bit BASIC Compiler.
''	Copyright (C) 2004-2005 Andre Victor T. Vicentini (av1ctor@yahoo.com.br)
''
''	This program is free software; you can redistribute it and/or modify
''	it under the terms of the GNU General Public License as published by
''	the Free Software Foundation; either version 2 of the License, or
''	(at your option) any later version.
''
''	This program is distributed in the hope that it will be useful,
''	but WITHOUT ANY WARRANTY; without even the implied warranty of
''	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
''	GNU General Public License for more details.
''
''	You should have received a copy of the GNU General Public License
''	along with this program; if not, write to the Free Software
''	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA.

'' AST constant nodes
'' l = NULL; r = NULL
''
'' chng: sep/2004 written [v1ctor]

option explicit
option escape

#include once "inc\fb.bi"
#include once "inc\fbint.bi"
#include once "inc\ir.bi"
#include once "inc\ast.bi"

'':::::
function astNewCONSTs( byval v as string ) as ASTNODE ptr static
    dim as FBSYMBOL ptr tc

	tc = hAllocStringConst( v, len( v ) )
    if( tc = NULL ) then
    	exit function
    end if

	function = astNewVAR( tc, NULL, 0, IR_DATATYPE_CHAR )

end function

'':::::
''	!!!FIXME!!!
''	chicken-egg: wstring type needed
''	!!!FIXME!!!
function astNewCONSTws( byval v as string ) as ASTNODE ptr static
''''function astNewCONSTws( byval v as wstring ptr ) as ASTNODE ptr static
    dim as FBSYMBOL ptr tc

	tc = hAllocStringConst( v, len( v ) )
''''tc = hAllocWstringConst( v, len( *v ) )
    if( tc = NULL ) then
    	exit function
    end if

	function = astNewVAR( tc, NULL, 0, IR_DATATYPE_CHAR )
''''function = astNewVAR( tc, NULL, 0, IR_DATATYPE_WCHAR )

end function


'':::::
function astNewCONSTi( byval value as integer, _
					   byval dtype as integer, _
					   byval subtype as FBSYMBOL ptr ) as ASTNODE ptr static
    dim as ASTNODE ptr n

	'' alloc new node
	n = astNewNode( AST_NODECLASS_CONST, dtype, subtype )
	function = n

	if( n = NULL ) then
		exit function
	end if

	n->val.int= value
	n->defined = TRUE

end function

'':::::
function astNewCONSTf( byval value as double, _
					   byval dtype as integer ) as ASTNODE ptr static
    dim as ASTNODE ptr n

	'' alloc new node
	n = astNewNode( AST_NODECLASS_CONST, dtype )
	function = n

	if( n = NULL ) then
		exit function
	end if

	n->val.float= value
	n->defined = TRUE

end function

'':::::
function astNewCONST64( byval value as longint, _
					    byval dtype as integer ) as ASTNODE ptr static
    dim as ASTNODE ptr n

	'' alloc new node
	n = astNewNode( AST_NODECLASS_CONST, dtype )
	function = n

	if( n = NULL ) then
		exit function
	end if

	n->val.long = value
	n->defined   = TRUE

end function

'':::::
function astNewCONST( byval v as FBVALUE ptr, _
					  byval dtype as integer ) as ASTNODE ptr static
    dim as ASTNODE ptr n

	'' alloc new node
	n = astNewNode( AST_NODECLASS_CONST, dtype )
	function = n

	if( n = NULL ) then
		exit function
	end if

	select case as const dtype
	case IR_DATATYPE_LONGINT, IR_DATATYPE_ULONGINT
		n->val.long = v->long
	case IR_DATATYPE_SINGLE, IR_DATATYPE_DOUBLE
		n->val.float = v->float
	case else
		n->val.int = v->int
	end select

	n->defined = TRUE

end function

'':::::
function astLoadCONST( byval n as ASTNODE ptr ) as IRVREG ptr static
	dim as integer dtype
	dim as FBSYMBOL ptr s

	if( ast.doemit ) then
		dtype = n->dtype

		select case dtype
		'' longints?
		case IR_DATATYPE_LONGINT, IR_DATATYPE_ULONGINT
			return irAllocVRIMM64( dtype, n->val.long )

		'' if node is a float, create a temp float var (x86 assumption)
		case IR_DATATYPE_SINGLE, IR_DATATYPE_DOUBLE
			s = hAllocFloatConst( n->val.float, dtype )
			return irAllocVRVAR( dtype, s, s->ofs )

		''
		case else
			return irAllocVRIMM( dtype, n->val.int )
		end select
	end if

end function


